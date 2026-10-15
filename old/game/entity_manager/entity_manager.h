#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "game/entity/entity.h"
#include "game/entity/entity_chunk.h"
#include "game/entity/tile.h"
#include "utils/constants.h"
#include "utils/coordinate.h"

class World;

class EntityManager
{
public:
    EntityManager() = default;
    ~EntityManager() = default;

    EntityManager(const EntityManager &) = delete;
    EntityManager &operator=(const EntityManager &) = delete;

    EntityManager(EntityManager &&) noexcept = default;
    EntityManager &operator=(EntityManager &&) noexcept = default;

    void SetWorld(World *world) { world_ = world; }

    // Чанки
    EntityChunk *GetChunk(int32_t globalX, int32_t globalY);
    EntityChunk &GetOrCreateChunk(int32_t globalX, int32_t globalY);
    void RemoveChunk(int32_t globalX, int32_t globalY);
    EntityChunk *GetChunkByChunkCoords(int32_t chunkX, int32_t chunkY);
    EntityChunk *GetChunkAndLocalCoords(int32_t globalX, int32_t globalY, int32_t &localX, int32_t &localY);
    const std::unordered_map<ChunkKey, std::unique_ptr<EntityChunk>> &GetAllChunks() const { return chunks_; }

    // Тайлы и полы
    Tile *GetTile(int32_t globalX, int32_t globalY);
    void SetFloor(int32_t x, int32_t y, uint16_t type);
    void RemoveFloor(int32_t x, int32_t y);

    // Сущности
    EntityUid SpawnEntity(uint32_t type, uint32_t flags, Coordinate anchor, uint8_t rotation = 0, const EntityShape *footprint = nullptr);
    void RemoveEntity(EntityUid uid);
    bool MoveEntity(EntityUid uid, Coordinate newAnchor);
    Entity *GetEntity(EntityUid uid);
    const std::vector<Entity *> &GetAll() const { return entities_; }

    void SetVelocity(EntityUid uid, double newVelX, double newVelY);
    void UpdateMovement(double dt);

    // Поиск
    std::vector<Entity *> GetEntitiesInRect(int32_t minX, int32_t minY, int32_t maxX, int32_t maxY, uint32_t flags = 0);
    std::vector<Entity *> GetEntitiesInRadius(Coordinate center, int32_t radius, uint32_t flags = 0);
    std::vector<Entity *> GetEntitiesInSquare(Coordinate center, int32_t halfSize, uint32_t flags = 0);
    Entity *RaycastFirst(Coordinate from, Coordinate to, uint32_t flags);

    // Удаление пустых чанков
    void FlushEmptyChunks();

private:
    void AddToTileOccupancy(Entity *entity, const std::vector<Coordinate> &occupiedTiles);
    void RemoveFromTileOccupancy(Entity *entity, const std::vector<Coordinate> &occupiedTiles);
    bool TryMoveEntityToTile(EntityUid uid, Coordinate newAnchor);
    bool IsTileBlocked(Coordinate coord);
    void RemoveChunkByChunkCoords(int32_t chunkX, int32_t chunkY);
    void MarkOrUnmarkChunkEmpty(EntityChunk *chunk);

    World *world_ = nullptr;
    std::unordered_map<ChunkKey, std::unique_ptr<EntityChunk>> chunks_;
    std::unordered_map<EntityUid, Entity *> entityByUid_;
    std::vector<Entity *> entities_;
    std::vector<Entity *> movingEntities_;
    EntityUid nextUid_ = 1;

    std::unordered_set<ChunkKey> pendingEmptyChunks_;
};
