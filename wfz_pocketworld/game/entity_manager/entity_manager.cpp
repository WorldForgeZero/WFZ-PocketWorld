#include "entity_manager/entity_manager.h"

#include <algorithm>
#include <cmath>
#include <utility>

#include "game/world/world.h"
#include "utils/logger.h"

// Чанки
EntityChunk *EntityManager::GetChunk(int32_t globalX, int32_t globalY)
{
    int32_t chunkX = World::TileToChunkCoord(globalX);
    int32_t chunkY = World::TileToChunkCoord(globalY);
    return GetChunkByChunkCoords(chunkX, chunkY);
}

EntityChunk &EntityManager::GetOrCreateChunk(int32_t globalX, int32_t globalY)
{
    int32_t chunkX = World::TileToChunkCoord(globalX);
    int32_t chunkY = World::TileToChunkCoord(globalY);
    auto it = chunks_.find({chunkX, chunkY});
    if (it != chunks_.end())
        return *it->second;

    auto chunk = std::make_unique<EntityChunk>(chunkX, chunkY);
    EntityChunk *ptr = chunk.get();
    chunks_[{chunkX, chunkY}] = std::move(chunk);
    return *ptr;
}

void EntityManager::RemoveChunk(int32_t globalX, int32_t globalY)
{
    int32_t chunkX = World::TileToChunkCoord(globalX);
    int32_t chunkY = World::TileToChunkCoord(globalY);
    RemoveChunkByChunkCoords(chunkX, chunkY);
}

void EntityManager::RemoveChunkByChunkCoords(int32_t chunkX, int32_t chunkY)
{
    auto it = chunks_.find({chunkX, chunkY});
    if (it == chunks_.end())
        return;

    pendingEmptyChunks_.erase({chunkX, chunkY});

    std::vector<EntityUid> uids;
    for (const auto &entity : it->second->GetEntities())
        uids.push_back(entity->uid);

    for (EntityUid uid : uids)
        RemoveEntity(uid);

    chunks_.erase(it);
}

EntityChunk *EntityManager::GetChunkByChunkCoords(int32_t chunkX, int32_t chunkY)
{
    auto it = chunks_.find({chunkX, chunkY});
    return (it != chunks_.end()) ? it->second.get() : nullptr;
}

EntityChunk *EntityManager::GetChunkAndLocalCoords(int32_t globalX, int32_t globalY, int32_t &localX, int32_t &localY)
{
    int32_t chunkX = World::TileToChunkCoord(globalX);
    int32_t chunkY = World::TileToChunkCoord(globalY);
    EntityChunk *chunk = GetChunkByChunkCoords(chunkX, chunkY);
    if (!chunk)
        return nullptr;

    localX = globalX - (chunkX << CHUNK_SHIFT);
    localY = globalY - (chunkY << CHUNK_SHIFT);
    return chunk;
}

Tile *EntityManager::GetTile(int32_t globalX, int32_t globalY)
{
    int32_t lx, ly;
    EntityChunk *chunk = GetChunkAndLocalCoords(globalX, globalY, lx, ly);
    if (!chunk)
        return nullptr;

    return &chunk->GetTile(lx, ly);
}

// Полы
void EntityManager::SetFloor(int32_t x, int32_t y, uint16_t type)
{
    if (type == 0)
    {
        RemoveFloor(x, y);
        return;
    }

    Tile *tile = GetTile(x, y);
    EntityChunk *chunk = nullptr;
    if (!tile)
    {
        chunk = &GetOrCreateChunk(x, y);
        tile = GetTile(x, y);
    }
    else
    {
        chunk = GetChunk(x, y);
    }

    if (tile && tile->floorType == 0)
    {
        tile->floorType = type;
        if (world_)
            world_->NotifyFloorChanged({x, y}, true);

        if (chunk)
            MarkOrUnmarkChunkEmpty(chunk);
    }
}

void EntityManager::RemoveFloor(int32_t x, int32_t y)
{
    Tile *tile = GetTile(x, y);
    if (tile && tile->floorType != 0)
    {
        EntityChunk *chunk = GetChunk(x, y);
        tile->floorType = 0;
        if (world_)
            world_->NotifyFloorChanged({x, y}, false);

        if (chunk)
            MarkOrUnmarkChunkEmpty(chunk);
    }
}

// Сущности

EntityUid EntityManager::SpawnEntity(uint32_t type, uint32_t flags, Coordinate anchor, uint8_t rotation, const EntityShape *footprint)
{
    auto entity = std::make_unique<Entity>(nextUid_++, type, flags, anchor, rotation, footprint);
    Entity *rawEntity = entity.get();

    auto occupied = rawEntity->GetOccupiedTiles();

    for (const auto &tc : occupied)
    {
        if (IsTileBlocked(tc))
            return 0;
    }

    for (const auto &tc : occupied)
    {
        GetOrCreateChunk(tc.x, tc.y);
    }

    EntityChunk &chunk = GetOrCreateChunk(anchor.x, anchor.y);

    rawEntity->id = static_cast<EntityIndex>(entities_.size());
    rawEntity->chunk = &chunk;

    chunk.AddEntity(std::move(entity));
    entities_.push_back(rawEntity);
    entityByUid_[rawEntity->uid] = rawEntity;

    AddToTileOccupancy(rawEntity, occupied);

    pendingEmptyChunks_.erase({chunk.GetX(), chunk.GetY()});

    return rawEntity->uid;
}

void EntityManager::RemoveEntity(EntityUid uid)
{
    auto entIt = entityByUid_.find(uid);
    if (entIt == entityByUid_.end())
        return;

    Entity *entity = entIt->second;
    EntityChunk *chunk = entity->chunk;

    auto occupied = entity->GetOccupiedTiles();
    RemoveFromTileOccupancy(entity, occupied);

    EntityIndex idx = entity->id;
    Entity *last = entities_.back();
    entities_[idx] = last;
    last->id = idx;
    entities_.pop_back();

    if (entity->HasFlag(EntityFlags::MOVING))
    {
        EntityIndex movingIdx = entity->movingIndex;
        Entity *lastMoving = movingEntities_.back();
        movingEntities_[movingIdx] = lastMoving;
        if (lastMoving != entity)
            lastMoving->movingIndex = movingIdx;

        movingEntities_.pop_back();
    }

    chunk->RemoveEntity(entity);
    entityByUid_.erase(entIt);

    MarkOrUnmarkChunkEmpty(chunk);
}

bool EntityManager::MoveEntity(EntityUid uid, Coordinate newAnchor)
{
    if (!TryMoveEntityToTile(uid, newAnchor))
        return false;

    Entity *entity = entityByUid_[uid];
    entity->pos.SetX(newAnchor.x * VECTOR2D_FIXED_SCALE);
    entity->pos.SetY(newAnchor.y * VECTOR2D_FIXED_SCALE);
    return true;
}

Entity *EntityManager::GetEntity(EntityUid uid)
{
    auto it = entityByUid_.find(uid);
    return (it != entityByUid_.end()) ? it->second : nullptr;
}

void EntityManager::SetVelocity(EntityUid uid, double newVelX, double newVelY)
{
    auto it = entityByUid_.find(uid);
    if (it == entityByUid_.end())
        return;

    Entity *entity = it->second;
    bool wasMoving = entity->HasFlag(EntityFlags::MOVING);
    bool willMove = (newVelX != 0.0 || newVelY != 0.0);

    entity->velX = newVelX;
    entity->velY = newVelY;

    if (!wasMoving && willMove)
    {
        entity->AddFlag(EntityFlags::MOVING);
        entity->movingIndex = static_cast<EntityIndex>(movingEntities_.size());
        movingEntities_.push_back(entity);
    }
    else if (wasMoving && !willMove)
    {
        EntityIndex idx = entity->movingIndex;
        Entity *lastMoving = movingEntities_.back();
        movingEntities_[idx] = lastMoving;
        if (lastMoving != entity)
            lastMoving->movingIndex = idx;

        movingEntities_.pop_back();
        entity->RemoveFlag(EntityFlags::MOVING);
    }
}

void EntityManager::UpdateMovement(double dt)
{
    const int32_t MAX_STEP = VECTOR2D_FIXED_SCALE / 2;

    for (size_t i = 0; i < movingEntities_.size();)
    {
        Entity *entity = movingEntities_[i];

        double dx = entity->velX * dt * VECTOR2D_FIXED_SCALE;
        double dy = entity->velY * dt * VECTOR2D_FIXED_SCALE;

        int32_t stepX = static_cast<int32_t>(std::round(dx));
        int32_t stepY = static_cast<int32_t>(std::round(dy));

        if (stepX == 0 && stepY == 0)
        {
            ++i;
            continue;
        }

        int32_t steps = std::max(std::abs(stepX), std::abs(stepY)) / MAX_STEP + 1;
        int32_t subStepX = stepX / steps;
        int32_t subStepY = stepY / steps;

        bool stopped = false;

        for (int s = 0; s < steps && !stopped; ++s)
        {
            int32_t newPosX = entity->pos.GetX() + subStepX;
            int32_t newPosY = entity->pos.GetY() + subStepY;

            int32_t newAnchorX = newPosX / VECTOR2D_FIXED_SCALE;
            int32_t newAnchorY = newPosY / VECTOR2D_FIXED_SCALE;

            bool tileChanged = (newAnchorX != entity->anchor.x || newAnchorY != entity->anchor.y);

            if (tileChanged)
            {
                if (!TryMoveEntityToTile(entity->uid, {newAnchorX, newAnchorY}))
                {
                    entity->velX = 0.0;
                    entity->velY = 0.0;
                    stopped = true;
                    break;
                }
            }

            entity->pos.SetX(newPosX);
            entity->pos.SetY(newPosY);
        }

        if (stopped || (entity->velX == 0.0 && entity->velY == 0.0))
        {
            EntityIndex idx = entity->movingIndex;
            Entity *lastMoving = movingEntities_.back();
            movingEntities_[idx] = lastMoving;
            if (lastMoving != entity)
                lastMoving->movingIndex = idx;

            movingEntities_.pop_back();
            entity->RemoveFlag(EntityFlags::MOVING);
            continue;
        }

        ++i;
    }
}

// Вспомогательные
void EntityManager::AddToTileOccupancy(Entity *entity, const std::vector<Coordinate> &occupiedTiles)
{
    for (const auto &globalCoord : occupiedTiles)
    {
        int32_t lx, ly;
        EntityChunk *chunk = GetChunkAndLocalCoords(globalCoord.x, globalCoord.y, lx, ly);
        if (!chunk)
            continue;

        Tile &tile = chunk->GetTile(lx, ly);
        tile.occupyingEntities.push_back(entity);
        if (entity->HasFlag(EntityFlags::SOLID))
            tile.solidCount++;

        if (entity->HasFlag(EntityFlags::ZONE_BLOCKER) && world_)
            world_->NotifyZoneBlockerChanged(globalCoord);
    }
}

void EntityManager::RemoveFromTileOccupancy(Entity *entity, const std::vector<Coordinate> &occupiedTiles)
{
    for (const auto &globalCoord : occupiedTiles)
    {
        int32_t lx, ly;
        EntityChunk *chunk = GetChunkAndLocalCoords(globalCoord.x, globalCoord.y, lx, ly);
        if (!chunk)
            continue;

        Tile &tile = chunk->GetTile(lx, ly);
        auto &vec = tile.occupyingEntities;
        auto it = std::find(vec.begin(), vec.end(), entity);
        if (it != vec.end())
        {
            if (entity->HasFlag(EntityFlags::SOLID))
                tile.solidCount--;

            std::swap(*it, vec.back());
            vec.pop_back();

            if (entity->HasFlag(EntityFlags::ZONE_BLOCKER) && world_)
                world_->NotifyZoneBlockerChanged(globalCoord);
        }
    }
}

bool EntityManager::IsTileBlocked(Coordinate coord)
{
    int32_t lx, ly;
    EntityChunk *chunk = GetChunkAndLocalCoords(coord.x, coord.y, lx, ly);
    if (!chunk)
        return false;

    Tile &tile = chunk->GetTile(lx, ly);
    return tile.solidCount > 0;
}

bool EntityManager::TryMoveEntityToTile(EntityUid uid, Coordinate newAnchor)
{
    auto entIt = entityByUid_.find(uid);
    if (entIt == entityByUid_.end())
        return false;

    Entity *entity = entIt->second;
    EntityChunk *oldChunk = entity->chunk;

    auto oldOccupied = entity->GetOccupiedTiles();

    Coordinate oldAnchor = entity->anchor;
    entity->anchor = newAnchor;
    auto newOccupied = entity->GetOccupiedTiles();
    entity->anchor = oldAnchor;

    if (!entity->HasFlag(EntityFlags::EPHEMERAL))
    {
        for (const auto &tc : newOccupied)
        {
            if (IsTileBlocked(tc))
                return false;
        }
    }

    for (const auto &tc : newOccupied)
    {
        GetOrCreateChunk(tc.x, tc.y);
    }

    RemoveFromTileOccupancy(entity, oldOccupied);

    entity->anchor = newAnchor;

    int32_t newChunkX = newAnchor.x >> CHUNK_SHIFT;
    int32_t newChunkY = newAnchor.y >> CHUNK_SHIFT;
    bool sameChunk = (oldChunk->GetX() == newChunkX && oldChunk->GetY() == newChunkY);
    if (!sameChunk)
    {
        auto uniqueEntity = oldChunk->RemoveEntity(entity);
        EntityChunk &newChunk = GetOrCreateChunk(newAnchor.x, newAnchor.y);
        entity->chunk = &newChunk;
        newChunk.AddEntity(std::move(uniqueEntity));

        MarkOrUnmarkChunkEmpty(oldChunk);
        MarkOrUnmarkChunkEmpty(&newChunk);
    }

    AddToTileOccupancy(entity, newOccupied);
    return true;
}

std::vector<Entity *> EntityManager::GetEntitiesInRect(int32_t minX, int32_t minY, int32_t maxX, int32_t maxY, uint32_t flags)
{
    std::vector<Entity *> result;
    int32_t chunkMinX = World::TileToChunkCoord(minX);
    int32_t chunkMaxX = World::TileToChunkCoord(maxX);
    int32_t chunkMinY = World::TileToChunkCoord(minY);
    int32_t chunkMaxY = World::TileToChunkCoord(maxY);

    for (int32_t cx = chunkMinX; cx <= chunkMaxX; ++cx)
    {
        for (int32_t cy = chunkMinY; cy <= chunkMaxY; ++cy)
        {
            EntityChunk *chunk = GetChunkByChunkCoords(cx, cy);
            if (!chunk)
                continue;

            for (const auto &unique_entity : chunk->GetEntities())
            {
                Entity *entity = unique_entity.get();
                if (flags != 0 && (entity->flags & flags) == 0)
                    continue;

                if (entity->anchor.x >= minX && entity->anchor.x <= maxX &&
                    entity->anchor.y >= minY && entity->anchor.y <= maxY)
                {
                    result.push_back(entity);
                }
            }
        }
    }
    return result;
}

std::vector<Entity *> EntityManager::GetEntitiesInRadius(Coordinate center, int32_t radius, uint32_t flags)
{
    auto candidates = GetEntitiesInRect(center.x - radius, center.y - radius, center.x + radius, center.y + radius, flags);
    int64_t radiusSq = static_cast<int64_t>(radius) * radius;
    std::vector<Entity *> result;
    result.reserve(candidates.size());

    for (Entity *e : candidates)
    {
        int64_t dx = static_cast<int64_t>(e->anchor.x) - center.x;
        int64_t dy = static_cast<int64_t>(e->anchor.y) - center.y;
        if (dx * dx + dy * dy <= radiusSq)
            result.push_back(e);
    }
    return result;
}

std::vector<Entity *> EntityManager::GetEntitiesInSquare(Coordinate center, int32_t halfSize, uint32_t flags)
{
    return GetEntitiesInRect(center.x - halfSize, center.y - halfSize, center.x + halfSize, center.y + halfSize, flags);
}

Entity *EntityManager::RaycastFirst(Coordinate from, Coordinate to, uint32_t flags)
{
    int32_t x0 = from.x, y0 = from.y;
    int32_t x1 = to.x, y1 = to.y;

    int32_t dx = std::abs(x1 - x0);
    int32_t dy = std::abs(y1 - y0);
    int32_t sx = x0 < x1 ? 1 : -1;
    int32_t sy = y0 < y1 ? 1 : -1;
    int32_t err = dx - dy;

    while (true)
    {
        Tile *tile = GetTile(x0, y0);
        if (tile)
        {
            for (Entity *e : tile->occupyingEntities)
            {
                if ((flags != 0) && (e->flags & flags) == 0)
                    continue;
                return e;
            }
        }

        if (x0 == x1 && y0 == y1)
            break;

        int32_t e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
    return nullptr;
}

void EntityManager::MarkOrUnmarkChunkEmpty(EntityChunk *chunk)
{
    if (!chunk)
        return;

    ChunkKey key{chunk->GetX(), chunk->GetY()};
    if (chunk->IsEmpty())
        pendingEmptyChunks_.insert(key);

    else
        pendingEmptyChunks_.erase(key);
}

void EntityManager::FlushEmptyChunks()
{
    for (const ChunkKey &key : pendingEmptyChunks_)
        chunks_.erase(key);

    pendingEmptyChunks_.clear();
}
