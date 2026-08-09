#pragma once

#include <cstdint>

#include "coordinate.h"
#include "resistance.h"
#include "vector_2d.h"

enum EntityFlags : uint32_t
{
    ENTITY_SOLID = 1 << 0, // блокирует движение
    ENTITY_FLOOR = 1 << 1, // определяет, можно ли здесь стоять и держать газ
};

class Entity
{
private:
    uint32_t id = 0;
    uint32_t type_;
    Coordinate coord_;
    uint32_t flags_;

    uint8_t rotation = 0;
    uint8_t height = 1, width = 1;

    Vector2D vel;

    Resistance res;

    Entity *cell_next_ = nullptr;

public:
    Entity(
        uint32_t type,
        uint32_t flags,
        uint8_t rotation = 0,
        uint8_t height = 1,
        uint8_t width = 1,
        Resistance res = {0, 0, 0, 0});

    ~Entity() = default;

    Entity(const Entity &) = delete;
    Entity &operator=(const Entity &) = delete;

    // Геттеры и сеттеры
    uint32_t GetType() const;
    uint32_t GetFlags() const;
    bool HasFlag(uint32_t flag) const;
    void AddFlag(uint32_t flag);
    void RemoveFlag(uint32_t flag);

    Entity *GetCellNext() const;
    void SetCellNext(Entity *next);
};
