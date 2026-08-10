#pragma once

#include <cstdint>
#include <vector>

#include "coordinate.h"
#include "footprint.h"
#include "vector_2d.h"

enum EntityFlags : uint32_t
{
    ENTITY_SOLID = 1 << 0,
    ENTITY_FLOOR = 1 << 1,
};

class Entity
{
public:
    uint32_t id;    //< уникальный идентификатор
    uint32_t type;  //< ID префаба (ссылка на описание в БД/словаре)
    uint32_t flags; //< битовые флаги (EntityFlags)

    Coordinate anchor; //< опорная клетка. Левый-нижний угол
    uint8_t rotation;  //< 0..3 (0°, 90°, 180°, 270°)
    Vector2D vel;      //< скорость

    uint8_t width;
    uint8_t height;

    const FootprintTemplate *footprint;

public:
    Entity(uint32_t id, uint32_t type, uint32_t flags,
           Coordinate anchor, uint8_t rotation = 0,
           uint8_t width = 1, uint8_t height = 1,
           const FootprintTemplate *footprint = nullptr);

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
