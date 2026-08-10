#pragma once

#include <memory>
#include <vector>

#include "coordinate.h"
#include "entity.h"

class Tile
{

public:
    Coordinate coord;

    std::vector<Entity *> occupyingEntities;

public:
    Tile();
    ~Tile();
};
