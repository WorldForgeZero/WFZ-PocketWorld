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

    bool TryMoveEntityToTile(World &world, EntityUid uid, Coordinate newAnchor);
    bool IsTileBlocked(World &world, Coordinate coord);

    std::unordered_map<EntityUid, Entity *> entityByUid_; // UID -> указатель
    std::vector<Entity *> entities_;                      // плотный список всех сущностей
    std::vector<Entity *> movingEntities_;                // движущиеся сущности

    EntityUid nextUid_ = 1;

public:
    EntityManager() = default;
    ~EntityManager() = default;

    EntityManager(const EntityManager &) = delete;
    EntityManager &operator=(const EntityManager &) = delete;

    EntityManager(EntityManager &&) noexcept = default;
    EntityManager &operator=(EntityManager &&) noexcept = default;

    EntityUid SpawnEntity(World &world, uint32_t type, uint32_t flags, Coordinate anchor, uint8_t rotation = 0, const EntityShape *footprint = nullptr);

    void RemoveEntity(World &world, EntityUid uid);
    bool MoveEntity(World &world, EntityUid uid, Coordinate newAnchor);

    Entity *GetEntity(EntityUid uid);
    const std::vector<Entity *> &GetAll() const { return entities_; }

    void SetVelocity(EntityUid uid, double newVelX, double newVelY);
    void UpdateMovement(World &world, double dt);
};
