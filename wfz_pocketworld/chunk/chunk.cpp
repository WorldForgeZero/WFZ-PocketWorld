#include <stdint.h>
#include <stdlib.h>

#include "chunk.h"

Chunk::Chunk(uint32_t x, uint32_t y)
    : key_{x, y}
{
}

Chunk::~Chunk()
{
}

// Гетеры

uint32_t Chunk::GetX() const
{
    return key_.x;
}

uint32_t Chunk::GetY() const
{
    return key_.y;
}
