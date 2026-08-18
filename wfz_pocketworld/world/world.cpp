#include "world.h"

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

EntityId World::SpawnEntity(uint32_t type, uint32_t flags, Coordinate anchor, uint8_t rotation, const EntityShape *footprint)
{
    return entityManager_.SpawnEntity(*this, type, flags, anchor, rotation, footprint);
}

void World::RemoveEntity(EntityId id)
{
    entityManager_.RemoveEntity(*this, id);
}

bool World::MoveEntity(EntityId id, Coordinate newAnchor)
{
    return entityManager_.MoveEntity(*this, id, newAnchor);
}

Entity *World::GetEntity(EntityId id)
{
    return entityManager_.GetEntity(id);
}

const std::vector<Entity *> &World::GetAllEntities() const
{
    return entityManager_.GetAll();
}

void World::SetVelocity(EntityId id, double newVelX, double newVelY)
{
    entityManager_.SetVelocity(id, newVelX, newVelY);
}

void World::Tick(double dt)
{
    entityManager_.UpdateMovement(*this, dt);
}

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

    std::vector<EntityId> ids;
    for (const auto &entity : it->second->GetEntities())
    {
        ids.push_back(entity->id);
    }

    for (EntityId id : ids)
    {
        entityManager_.RemoveEntity(*this, id);
    }

    chunks_.erase(it);
}
