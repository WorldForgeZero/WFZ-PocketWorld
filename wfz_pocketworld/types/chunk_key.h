#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef struct ChunkKey
{
    uint32_t cx; //< Координата чанка по x
    uint32_t cy; //< Координата чанка по y
} ChunkKey;      // 8 bytes

// Нет методов для этой дресни. Выделяйте её сами
