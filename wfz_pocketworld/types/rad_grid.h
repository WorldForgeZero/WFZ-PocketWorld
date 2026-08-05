#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct RadGrid
{
    uint32_t *level;  //< Уровень радиации в каждй клетке (двумерный массив)
    uint32_t ambient; //< Средний уровень во всём чанке
    uint32_t active;  //< Количество активных клеток
} RadGrid;            // 16 bytes (без динамической части)

// Управление памятью структуры

/// @brief
/// @param ambient
/// @return
RadGrid *AllocateRadGrid(uint32_t ambient);

/// @brief
/// @param p
void FreeRadGrid(RadGrid *p);
