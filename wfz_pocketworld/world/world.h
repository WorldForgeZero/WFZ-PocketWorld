#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "chunk.h"
#include "constants.h"
#include "entity_manager.h"

class World
{
private:
    std::unordered_map<ChunkKey, std::unique_ptr<Chunk>> chunks_;
    EntityManager entityManager_;

public:
    World() = default;
    ~World() = default;

    World(const World &) = delete;
    World &operator=(const World &) = delete;

    World(World &&) noexcept = default;
    World &operator=(World &&) noexcept = default;

    // Чанки
    Chunk *GetChunk(int32_t globalX, int32_t globalY);
    Chunk &GetOrCreateChunk(int32_t globalX, int32_t globalY);
    void RemoveChunk(int32_t globalX, int32_t globalY);

    // Сущности
    EntityId SpawnEntity(uint32_t type, uint32_t flags, Coordinate anchor, uint8_t rotation = 0, const EntityShape *footprint = nullptr);
    void RemoveEntity(EntityId id);
    bool MoveEntity(EntityId id, Coordinate newAnchor);
    Entity *GetEntity(EntityId id);
    const std::vector<Entity *> &GetAllEntities() const;

    // Движение и тик
    void SetVelocity(EntityId id, Vector2D newVel);
    void Tick(double dt);

private:
    Chunk *GetChunkByChunkCoords(int32_t chunkX, int32_t chunkY);
    Chunk &GetOrCreateChunkByChunkCoords(int32_t chunkX, int32_t chunkY);
    void RemoveChunkByChunkCoords(int32_t chunkX, int32_t chunkY);

    static int32_t TileToChunkCoord(int32_t tileCoord)
    {
        int32_t result = tileCoord / CHUNK_SIZE;
        if (tileCoord % CHUNK_SIZE != 0 && tileCoord < 0)
            --result;
        return result;
    }
};
