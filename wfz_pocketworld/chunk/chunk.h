#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "chunk_key.h"

class Chunk
{
private:
    ChunkKey key_;

public:
    Chunk(uint32_t cx, uint32_t cy);
    ~Chunk();

    uint32_t GetCx() const;
    uint32_t GetCy() const;
};
