#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "coordinate.h"
#include "vector_2d.h"

class Entity
{
private:
    // Служебное
    uint32_t id_;
    uint32_t type_;
    Coordinate coord_;
    uint32_t flags_; // see in entity_flags.h

    // Пространственное ориентирование
    uint8_t rotation_; // see in constants.h
    uint8_t height_, width_;

    // Физон
    Vector2D vec_;

    uint8_t gas_isolation_;
    uint8_t mana_isolation_;
    uint8_t rad_isolation_;

    // Хранение
    Entity *cell_next_;

public:
    Entity(uint32_t type,
           uint32_t flags,
           uint8_t rotation = 0,
           uint8_t height = 1,
           uint8_t width = 1,
           uint8_t gas_isolation = 0,
           uint8_t mana_isolation = 0,
           uint8_t rad_isolation = 0);

    ~Entity() = default;
};
