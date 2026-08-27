#include "entity_manager.h"

#include <algorithm>
#include <cmath>
#include <utility>

#include "chunk.h"
#include "constants.h"
#include "tile.h"
#include "world.h"

EntityUid EntityManager::SpawnEntity(World &world, uint32_t type, uint32_t flags, Coordinate anchor, uint8_t rotation, const EntityShape *footprint)
{
    auto entity = std::make_unique<Entity>(nextUid_, type, flags, anchor, rotation, footprint);
    Entity *rawEntity = entity.get();

    auto occupied = rawEntity->GetOccupiedTiles();

    for (const auto &tc : occupied)
    {
        if (IsTileBlocked(world, tc))
            return 0;
    }

    for (const auto &tc : occupied)
    {
        world.GetOrCreateChunk(tc.x, tc.y);
    }

    Chunk &chunk = world.GetOrCreateChunk(anchor.x, anchor.y);

    // Присваиваем индекс в entities_
    rawEntity->id = static_cast<EntityIndex>(entities_.size());
    rawEntity->chunk = &chunk;

    chunk.AddEntity(std::move(entity));
    entities_.push_back(rawEntity);
    entityByUid_[rawEntity->uid] = rawEntity;

    AddToTileOccupancy(world, rawEntity, occupied);

    return rawEntity->uid;
}

void EntityManager::RemoveEntity(World &world, EntityUid uid)
{
    auto entIt = entityByUid_.find(uid);
    if (entIt == entityByUid_.end())
        return;

    Entity *entity = entIt->second;
    Chunk *chunk = entity->chunk;

    auto occupied = entity->GetOccupiedTiles();
    RemoveFromTileOccupancy(world, entity, occupied);

    // Удаляем из entities_ (swap-pop)
    EntityIndex idx = entity->id;
    Entity *last = entities_.back();
    entities_[idx] = last;
    last->id = idx;
    entities_.pop_back();

    // Удаляем из movingEntities_, если движется
    if (entity->HasFlag(ENTITY_MOVING))
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
}

bool EntityManager::TryMoveEntityToTile(World &world, EntityUid uid, Coordinate newAnchor)
{
    auto entIt = entityByUid_.find(uid);
    if (entIt == entityByUid_.end())
        return false;

    Entity *entity = entIt->second;
    Chunk *oldChunk = entity->chunk;

    auto oldOccupied = entity->GetOccupiedTiles();

    Coordinate oldAnchor = entity->anchor;
    entity->anchor = newAnchor;
    auto newOccupied = entity->GetOccupiedTiles();
    entity->anchor = oldAnchor;

    if (!entity->HasFlag(ENTITY_EPHEMERAL))
    {
        for (const auto &tc : newOccupied)
        {
            if (IsTileBlocked(world, tc))
                return false;
        }
    }

    for (const auto &tc : newOccupied)
    {
        world.GetOrCreateChunk(tc.x, tc.y);
    }

    RemoveFromTileOccupancy(world, entity, oldOccupied);

    entity->anchor = newAnchor;

    int32_t newChunkX = newAnchor.x >> CHUNK_SHIFT;
    int32_t newChunkY = newAnchor.y >> CHUNK_SHIFT;
    bool sameChunk = (oldChunk->GetX() == newChunkX && oldChunk->GetY() == newChunkY);
    if (!sameChunk)
    {
        auto uniqueEntity = oldChunk->RemoveEntity(entity);
        Chunk &newChunk = world.GetOrCreateChunk(newAnchor.x, newAnchor.y);
        entity->chunk = &newChunk;
        newChunk.AddEntity(std::move(uniqueEntity));
    }

    AddToTileOccupancy(world, entity, newOccupied);
    return true;
}

bool EntityManager::MoveEntity(World &world, EntityUid uid, Coordinate newAnchor)
{
    if (!TryMoveEntityToTile(world, uid, newAnchor))
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

void EntityManager::AddToTileOccupancy(World &world, Entity *entity, const std::vector<Coordinate> &occupiedTiles)
{
    for (const auto &globalCoord : occupiedTiles)
    {
        Chunk *chunk = world.GetChunk(globalCoord.x, globalCoord.y);
        if (!chunk)
            continue;

        int32_t chunkX = globalCoord.x >> CHUNK_SHIFT;
        int32_t chunkY = globalCoord.y >> CHUNK_SHIFT;
        int32_t localX = globalCoord.x - (chunkX << CHUNK_SHIFT);
        int32_t localY = globalCoord.y - (chunkY << CHUNK_SHIFT);

        Tile &tile = chunk->GetTile(static_cast<uint32_t>(localX), static_cast<uint32_t>(localY));
        tile.occupyingEntities.push_back(entity);
        if (entity->HasFlag(ENTITY_SOLID))
            tile.solidCount++;
    }
}

void EntityManager::RemoveFromTileOccupancy(World &world, Entity *entity, const std::vector<Coordinate> &occupiedTiles)
{
    for (const auto &globalCoord : occupiedTiles)
    {
        Chunk *chunk = world.GetChunk(globalCoord.x, globalCoord.y);
        if (!chunk)
            continue;

        int32_t chunkX = globalCoord.x >> CHUNK_SHIFT;
        int32_t chunkY = globalCoord.y >> CHUNK_SHIFT;
        int32_t localX = globalCoord.x - (chunkX << CHUNK_SHIFT);
        int32_t localY = globalCoord.y - (chunkY << CHUNK_SHIFT);

        Tile &tile = chunk->GetTile(static_cast<uint32_t>(localX), static_cast<uint32_t>(localY));
        auto &vec = tile.occupyingEntities;
        auto it = std::find(vec.begin(), vec.end(), entity);
        if (it != vec.end())
        {
            if (entity->HasFlag(ENTITY_SOLID))
                tile.solidCount--;

            std::swap(*it, vec.back());
            vec.pop_back();
        }
    }
}

bool EntityManager::IsTileBlocked(World &world, Coordinate coord)
{
    Chunk *chunk = world.GetChunk(coord.x, coord.y);
    if (!chunk)
        return false;

    int32_t chunkX = coord.x >> CHUNK_SHIFT;
    int32_t chunkY = coord.y >> CHUNK_SHIFT;
    int32_t localX = coord.x - (chunkX << CHUNK_SHIFT);
    int32_t localY = coord.y - (chunkY << CHUNK_SHIFT);

    Tile &tile = chunk->GetTile(static_cast<uint32_t>(localX), static_cast<uint32_t>(localY));
    return tile.solidCount > 0;
}

void EntityManager::SetVelocity(EntityUid uid, double newVelX, double newVelY)
{
    auto it = entityByUid_.find(uid);
    if (it == entityByUid_.end())
        return;

    Entity *entity = it->second;

    bool wasMoving = entity->HasFlag(ENTITY_MOVING);
    bool willMove = (newVelX != 0.0 || newVelY != 0.0);

    entity->velX = newVelX;
    entity->velY = newVelY;

    if (!wasMoving && willMove)
    {
        entity->AddFlag(ENTITY_MOVING);
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

        entity->RemoveFlag(ENTITY_MOVING);
    }
}

void EntityManager::UpdateMovement(World &world, double dt)
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
                if (!TryMoveEntityToTile(world, entity->uid, {newAnchorX, newAnchorY}))
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

            entity->RemoveFlag(ENTITY_MOVING);
            continue;
        }

        ++i;
    }
}
