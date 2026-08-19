#pragma once

#include <cstdint>
#include <vector>

class Entity; // forward

class Tile
{
public:
    uint8_t localX = 0;
    uint8_t localY = 0;
    uint8_t solidCount = 0;

    std::vector<Entity *> occupyingEntities;

    Tile() = default;
    ~Tile() = default;
};
