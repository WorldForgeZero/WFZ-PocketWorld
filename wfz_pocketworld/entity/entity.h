#pragma once

#include <cstdint>
#include <vector>

#include "coordinate.h"
#include "entity_shape.h"
#include "resistance.h"
#include "vector_2d.h"

class Chunk;

using EntityId = uint32_t;

enum EntityFlags : uint32_t
{
    ENTITY_SOLID = 1 << 0,
    ENTITY_FLOOR = 1 << 1,
    ENTITY_EPHEMERAL = 1 << 2,
};

class Entity
{
public:
    uint32_t id;
    uint32_t type;
    uint32_t flags;

    Coordinate anchor; // тайловая координата
    Vector2D pos;      // fixed-point позиция (1 тайл = VECTOR2D_FIXED_SCALE)
    uint8_t rotation;
    double velX = 0.0;
    double velY = 0.0;
    Resistance res;

    const EntityShape *footprint;

    // Индексы для O(1) удаления из векторов EntityManager
    size_t entityIndex; // позиция в entities_
    size_t movingIndex; // позиция в movingEntities_
    bool isMoving;      // находится ли в movingEntities_

    // Чанк, в котором сущность находится
    Chunk *chunk;

public:
    Entity(
        uint32_t id,
        uint32_t type,
        uint32_t flags,
        Coordinate anchor,
        uint8_t rotation = 0,
        const EntityShape *footprint = nullptr);

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
