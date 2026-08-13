#pragma once

#include <cstdint>
#include <vector>

#include "coordinate.h"
#include "entity.h"

class Tile
{
public:
    Coordinate coord{0, 0};

    std::vector<Entity *> occupyingEntities;

    Tile() = default;
    ~Tile() = default;
};
