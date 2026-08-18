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

    bool TryMoveEntityToTile(World &world, EntityId id, Coordinate newAnchor);

    bool IsTileBlocked(World &world, Coordinate coord);

    std::unordered_map<EntityId, Entity *> entityIndex_; // ID -> указатель
    std::unordered_map<EntityId, Chunk *> entityChunk_;  // ID -> чанк
    std::vector<Entity *> entities_;                     // все активные сущности

    std::vector<Entity *> movingEntities_; // сущности, которые сейчас движутся

    EntityId nextEntityId_ = 1;

public:
    EntityManager() = default;
    ~EntityManager() = default;

    EntityManager(const EntityManager &) = delete;
    EntityManager &operator=(const EntityManager &) = delete;

    EntityManager(EntityManager &&) noexcept = default;
    EntityManager &operator=(EntityManager &&) noexcept = default;

    EntityId SpawnEntity(World &world, uint32_t type, uint32_t flags,
                         Coordinate anchor, uint8_t rotation = 0,
                         const EntityShape *footprint = nullptr);

    void RemoveEntity(World &world, EntityId id);

    bool MoveEntity(World &world, EntityId id, Coordinate newAnchor);

    Entity *GetEntity(EntityId id);
    const std::vector<Entity *> &GetAll() const { return entities_; }

    void SetVelocity(EntityId id, Vector2D newVel);
    void UpdateMovement(World &world, double dt);
};
