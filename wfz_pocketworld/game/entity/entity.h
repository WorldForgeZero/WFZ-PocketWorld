#pragma once

#include <cstdint>
#include <vector>

#include "coordinate.h"
#include "entity_shape.h"
#include "resistance.h"
#include "vector_2d.h"

class Chunk;

using EntityUid = uint32_t;
using EntityIndex = uint32_t;

enum EntityFlags : uint32_t
{
    SOLID = 1 << 0,
    EPHEMERAL = 1 << 1,
    MOVING = 1 << 2,
    ZONE_BLOCKER = 1 << 3,
};

class Entity
{
public:
    EntityUid uid;           // уникальный, не переиспользуется
    EntityIndex id;          // индекс в entities_
    EntityIndex movingIndex; // индекс в movingEntities_

    uint32_t type;
    uint32_t flags;

    Resistance res;
    uint8_t rotation;

    double velX = 0.0;
    double velY = 0.0;

    Coordinate anchor;
    Vector2D pos;

    const EntityShape *footprint;

    Chunk *chunk;

public:
    Entity(EntityUid uid, uint32_t type, uint32_t flags, Coordinate anchor, uint8_t rotation = 0, const EntityShape *footprint = nullptr);

    Entity(const Entity &) = delete;
    Entity &operator=(const Entity &) = delete;

    Entity(Entity &&) noexcept = default;
    Entity &operator=(Entity &&) noexcept = default;

    ~Entity() = default;

    std::vector<Coordinate> GetOccupiedTiles() const;

    bool HasFlag(uint32_t flag) const;
    void AddFlag(uint32_t flag);
    void RemoveFlag(uint32_t flag);
};
