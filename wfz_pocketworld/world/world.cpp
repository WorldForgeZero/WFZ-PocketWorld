#include "world.h"

#include <algorithm>
#include <cstdlib>

// Чанки
Chunk *World::GetChunk(int32_t globalX, int32_t globalY)
{
    int32_t chunkX = TileToChunkCoord(globalX);
    int32_t chunkY = TileToChunkCoord(globalY);
    return GetChunkByChunkCoords(chunkX, chunkY);
}

Chunk &World::GetOrCreateChunk(int32_t globalX, int32_t globalY)
{
    int32_t chunkX = TileToChunkCoord(globalX);
    int32_t chunkY = TileToChunkCoord(globalY);
    return GetOrCreateChunkByChunkCoords(chunkX, chunkY);
}

void World::RemoveChunk(int32_t globalX, int32_t globalY)
{
    int32_t chunkX = TileToChunkCoord(globalX);
    int32_t chunkY = TileToChunkCoord(globalY);
    RemoveChunkByChunkCoords(chunkX, chunkY);
}

Tile *World::GetTile(int32_t globalX, int32_t globalY)
{
    Chunk *chunk = GetChunk(globalX, globalY);
    if (!chunk)
        return nullptr;

    int32_t chunkX = TileToChunkCoord(globalX);
    int32_t chunkY = TileToChunkCoord(globalY);
    int32_t localX = globalX - (chunkX << CHUNK_SHIFT);
    int32_t localY = globalY - (chunkY << CHUNK_SHIFT);

    return &chunk->GetTile(static_cast<uint32_t>(localX), static_cast<uint32_t>(localY));
}

// Полы
void World::SetFloor(int32_t x, int32_t y, uint16_t type)
{
    if (type == 0)
    {
        RemoveFloor(x, y);
        return;
    }

    Tile *tile = GetTile(x, y);
    if (!tile)
    {
        GetOrCreateChunk(x, y);
        tile = GetTile(x, y);
    }

    if (tile)
    {
        tile->floorType = type;
    }
}

void World::RemoveFloor(int32_t x, int32_t y)
{
    Tile *tile = GetTile(x, y);
    if (tile)
    {
        tile->floorType = 0;
    }
}

// Немедленные операции
EntityUid World::SpawnEntityImmediate(uint32_t type, uint32_t flags, Coordinate anchor, uint8_t rotation, const EntityShape *footprint)
{
    return entityManager_.SpawnEntity(*this, type, flags, anchor, rotation, footprint);
}

void World::RemoveEntityImmediate(EntityUid uid)
{
    entityManager_.RemoveEntity(*this, uid);
}

bool World::MoveEntityImmediate(EntityUid uid, Coordinate newAnchor)
{
    return entityManager_.MoveEntity(*this, uid, newAnchor);
}

// Публичные очереди
void World::QueueSpawnEntity(uint32_t type, uint32_t flags, Coordinate anchor,                             uint8_t rotation, const EntityShape *footprint)
{
    spawnQueue_.push_back({type, flags, anchor, rotation, footprint});
}

void World::QueueRemoveEntity(EntityUid uid)
{
    removeQueue_.push_back({uid});
}

void World::QueueMoveEntity(EntityUid uid, Coordinate newAnchor)
{
    moveQueue_.push_back({uid, newAnchor});
}

// Применение очередей
void World::FlushCommands()
{
    for (const auto &cmd : removeQueue_)
    {
        RemoveEntityImmediate(cmd.uid);
    }
    removeQueue_.clear();

    for (const auto &cmd : spawnQueue_)
    {
        SpawnEntityImmediate(cmd.type, cmd.flags, cmd.anchor,                             cmd.rotation, cmd.footprint);
    }
    spawnQueue_.clear();

    for (const auto &cmd : moveQueue_)
    {
        MoveEntityImmediate(cmd.uid, cmd.newAnchor);
    }
    moveQueue_.clear();
}

// Доступ к сущностям
Entity *World::GetEntity(EntityUid uid)
{
    return entityManager_.GetEntity(uid);
}

const std::vector<Entity *> &World::GetAllEntities() const
{
    return entityManager_.GetAll();
}

void World::SetVelocity(EntityUid uid, double newVelX, double newVelY)
{
    entityManager_.SetVelocity(uid, newVelX, newVelY);
}

// Тик
void World::Tick(double dt)
{
    FlushCommands();

    entityManager_.UpdateMovement(*this, dt);
}

// Поиск
std::vector<Entity *> World::GetEntitiesInRect(int32_t minX, int32_t minY, int32_t maxX, int32_t maxY, uint32_t flags)
{
    std::vector<Entity *> result;
    int32_t chunkMinX = TileToChunkCoord(minX);
    int32_t chunkMaxX = TileToChunkCoord(maxX);
    int32_t chunkMinY = TileToChunkCoord(minY);
    int32_t chunkMaxY = TileToChunkCoord(maxY);

    for (int32_t cx = chunkMinX; cx <= chunkMaxX; ++cx)
    {
        for (int32_t cy = chunkMinY; cy <= chunkMaxY; ++cy)
        {
            Chunk *chunk = GetChunkByChunkCoords(cx, cy);
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

std::vector<Entity *> World::GetEntitiesInRadius(Coordinate center, int32_t radius, uint32_t flags)
{
    std::vector<Entity *> candidates = GetEntitiesInRect(center.x - radius, center.y - radius, center.x + radius, center.y + radius, flags);

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

std::vector<Entity *> World::GetEntitiesInSquare(Coordinate center, int32_t halfSize, uint32_t flags)
{
    return GetEntitiesInRect(center.x - halfSize, center.y - halfSize, center.x + halfSize, center.y + halfSize, flags);
}

Entity *World::RaycastFirst(Coordinate from, Coordinate to, uint32_t flags)
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

// Приватные методы чанков
Chunk *World::GetChunkByChunkCoords(int32_t chunkX, int32_t chunkY)
{
    auto it = chunks_.find({chunkX, chunkY});
    return (it != chunks_.end()) ? it->second.get() : nullptr;
}

Chunk &World::GetOrCreateChunkByChunkCoords(int32_t chunkX, int32_t chunkY)
{
    auto it = chunks_.find({chunkX, chunkY});
    if (it != chunks_.end())
        return *it->second;

    auto chunk = std::make_unique<Chunk>(chunkX, chunkY);
    Chunk *ptr = chunk.get();
    chunks_[{chunkX, chunkY}] = std::move(chunk);
    return *ptr;
}

void World::RemoveChunkByChunkCoords(int32_t chunkX, int32_t chunkY)
{
    auto it = chunks_.find({chunkX, chunkY});
    if (it == chunks_.end())
        return;

    std::vector<EntityUid> uids;
    for (const auto &entity : it->second->GetEntities())
    {
        uids.push_back(entity->uid);
    }

    for (EntityUid uid : uids)
    {
        entityManager_.RemoveEntity(*this, uid);
    }

    chunks_.erase(it);
}
