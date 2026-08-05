#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "chunk_key.h"
#include "rad_grid.h"
#include "uthash.h"

typedef struct Chunk
{
    // По факту служебное дерьмище
    ChunkKey *key;     //< Координаты чанка на плоскости
    UT_hash_handle hh; //< Служебное поле под uthash. Ключ - `key` поле

    // Аллоцируемая память на системы

    RadGrid *rad; //< Хитмапа радиации
    // Будет когда будут структуры под эту парашу =)

    // Блоки счёта чтобы понимать можно ли удалить чанк
} Chunk;

// Управление памятью структуры

/// @brief
/// @param cx
/// @param cy
/// @return
Chunk *AllocateChunk(uint32_t cx, uint32_t cy);

/// @brief
/// @param p
void FreeChunk(Chunk *p);
