#include <algorithm>
#include <utility>

#include "chunk.h"
#include "entity_manager.h"
#include "tile.h"
#include "world.h"

EntityId EntityManager::SpawnEntity(World &world, uint32_t type, uint32_t flags, Coordinate anchor, uint8_t rotation, uint8_t width, uint8_t height, const EntityShape *footprint)
{
    auto entity = std::make_unique<Entity>(nextEntityId_, type, flags, anchor, rotation, width, height, footprint);
    Entity *rawEntity = entity.get();

    auto occupied = rawEntity->GetOccupiedTiles();

    for (const auto &tc : occupied)
    {
        if (IsTileBlocked(world, tc, nullptr))
            return 0;
    }

    for (const auto &tc : occupied)
    {
        world.GetOrCreateChunk(tc.x, tc.y);
    }

    Chunk &chunk = world.GetOrCreateChunk(anchor.x, anchor.y);
    chunk.AddEntity(std::move(entity));

    entityIndex_[rawEntity->id] = rawEntity;
    entityChunk_[rawEntity->id] = &chunk;

    AddToTileOccupancy(world, rawEntity, occupied);

    return rawEntity->id;
}

void EntityManager::RemoveEntity(World &world, EntityId id)
{
    auto entIt = entityIndex_.find(id);
    if (entIt == entityIndex_.end())
        return;

    Entity *entity = entIt->second;
    Chunk *chunk = entityChunk_[id];

    auto occupied = entity->GetOccupiedTiles();
    RemoveFromTileOccupancy(world, entity, occupied);

    chunk->RemoveEntity(entity);

    entityIndex_.erase(entIt);
    entityChunk_.erase(id);
}

bool EntityManager::MoveEntity(World &world, EntityId id, Coordinate newAnchor)
{
    auto entIt = entityIndex_.find(id);
    if (entIt == entityIndex_.end())
        return false;

    Entity *entity = entIt->second;
    Chunk *oldChunk = entityChunk_[id];

    auto oldOccupied = entity->GetOccupiedTiles();

    Coordinate oldAnchor = entity->anchor;
    entity->anchor = newAnchor;
    auto newOccupied = entity->GetOccupiedTiles();
    entity->anchor = oldAnchor;

    for (const auto &tc : newOccupied)
    {
        if (IsTileBlocked(world, tc, entity))
            return false;
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
        newChunk.AddEntity(std::move(uniqueEntity));
        entityChunk_[id] = &newChunk;
    }

    AddToTileOccupancy(world, entity, newOccupied);
    return true;
}

Entity *EntityManager::GetEntity(EntityId id)
{
    auto it = entityIndex_.find(id);
    return (it != entityIndex_.end()) ? it->second : nullptr;
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
            std::swap(*it, vec.back());
            vec.pop_back();
        }
    }
}

bool EntityManager::IsTileBlocked(World &world, Coordinate coord, Entity *ignoreEntity)
{
    Chunk *chunk = world.GetChunk(coord.x, coord.y);
    if (!chunk)
        return false;

    int32_t chunkX = coord.x >> CHUNK_SHIFT;
    int32_t chunkY = coord.y >> CHUNK_SHIFT;
    int32_t localX = coord.x - (chunkX << CHUNK_SHIFT);
    int32_t localY = coord.y - (chunkY << CHUNK_SHIFT);

    Tile &tile = chunk->GetTile(static_cast<uint32_t>(localX), static_cast<uint32_t>(localY));
    for (Entity *e : tile.occupyingEntities)
    {
        if (e == ignoreEntity)
            continue;

        if (e->HasFlag(ENTITY_SOLID))
            return true;
    }
    return false;
}
