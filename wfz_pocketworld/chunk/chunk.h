#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "coordinate.h"

class Chunk
{
private:
    Coordinate key_;

public:
    Chunk(uint32_t x, uint32_t y);
    ~Chunk();

    uint32_t GetX() const;
    uint32_t GetY() const;
};
