#include <stdint.h>
#include <stdlib.h>

#include "chunk.h"
#include "chunk_key.h"

Chunk::Chunk(uint32_t cx, uint32_t cy)
    : key_{cx, cy}
{
}

Chunk::~Chunk()
{
}

// Гетеры

uint32_t Chunk::GetCx() const
{
    return key_.cx;
}

uint32_t Chunk::GetCy() const
{
    return key_.cy;
}
