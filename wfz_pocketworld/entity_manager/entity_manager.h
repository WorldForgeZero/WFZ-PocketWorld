#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include "coordinate.h"
#include "entity.h"

class World;
class Chunk;

class EntityManager
{
private:
    void AddToTileOccupancy(World &world, Entity *entity, const std::vector<Coordinate> &occupiedTiles);
    void RemoveFromTileOccupancy(World &world, Entity *entity, const std::vector<Coordinate> &occupiedTiles);
    bool IsTileBlocked(World &world, Coordinate coord, Entity *ignoreEntity);

    std::unordered_map<EntityId, Entity *> entityIndex_; // ID -> указатель
    std::unordered_map<EntityId, Chunk *> entityChunk_;  // ID -> чанк
    std::vector<Entity *> entities_;                     // все активные сущности (для быстрого обхода)

    EntityId nextEntityId_ = 1;

public:
    EntityManager() = default;
    ~EntityManager() = default;

    EntityManager(const EntityManager &) = delete;
    EntityManager &operator=(const EntityManager &) = delete;
    EntityManager(EntityManager &&) noexcept = default;
    EntityManager &operator=(EntityManager &&) noexcept = default;

    EntityId SpawnEntity(World &world, uint32_t type, uint32_t flags, Coordinate anchor, uint8_t rotation = 0, uint8_t width = 1, uint8_t height = 1, const EntityShape *footprint = nullptr);

    void RemoveEntity(World &world, EntityId id);
    bool MoveEntity(World &world, EntityId id, Coordinate newAnchor);

    Entity *GetEntity(EntityId id);
    const std::vector<Entity *> &GetAll() const { return entities_; }
};
