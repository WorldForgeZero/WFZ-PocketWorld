#pragma once

#include <cstdint>
#include <vector>

#include "resistance.h"

class Entity; // forward

class Tile
{
public:
    uint8_t localX = 0;
    uint8_t localY = 0;
    uint8_t solidCount = 0;

    uint16_t floorType = 0; // 0 = нет пола

    std::vector<Entity *> occupyingEntities;

    Tile() = default;
    ~Tile() = default;
};
