#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "entity.h"
#include "uthash.h"

/// @brief Размер чанка как степень двойки (4 -> 16 клеток)
#define CHUNK_SHIFT 4

/// @brief Размер стороны чанка в клетках (16)
#define CHUNK_SIZE (1 << CHUNK_SHIFT)

/// @brief Ключ для поиска чанка по координатам
typedef struct
{
    uint32_t cx; ///< X-координата чанка в сетке
    uint32_t cy; ///< Y-координата чанка в сетке
} ChunkKey;

/// @brief Контейнер, хранящий сущности в пределах квадрата CHUNK_SIZE x CHUNK_SIZE клеток
typedef struct Chunk
{
    /// @brief Ключ (координаты) чанка
    ChunkKey key;

    /// @brief Сетка 16x16 указателей на головы связных списков сущностей в каждой клетке
    Entity *grid[CHUNK_SIZE][CHUNK_SIZE];

    /// @brief Количество сущностей, находящихся в чанке (для быстрой проверки на пустоту)
    uint32_t entity_count;

    /// @brief Внутренний указатель для хеш-таблицы (uthash), связывает чанки в хеш-таблице плоскости
    UT_hash_handle hh;
} Chunk;

/// @brief Создаёт новый пустой чанк
/// @param cx X-координата чанка в сетке
/// @param cy Y-координата чанка в сетке
/// @return Указатель на созданный чанк или NULL при ошибке памяти
Chunk *ChunkNew(uint32_t cx, uint32_t cy);

/// @brief Освобождает память, занятую чанком и всеми сущностями внутри него
/// @param chunk Указатель на чанк (может быть NULL)
void ChunkFree(Chunk *chunk);

/// @brief Добавляет сущность в указанные клетки чанка (связывает через cell_next)
/// @param chunk  Указатель на чанк
/// @param entity Указатель на сущность
/// @param start_x Начальная локальная X-координата (внутри чанка)
/// @param start_y Начальная локальная Y-координата
/// @param end_x   Конечная локальная X-координата (включительно)
/// @param end_y   Конечная локальная Y-координата (включительно)
/// @return 0 при успехе, -1 если аргументы NULL
/// @note Эта функция только вставляет указатели в grid; она не проверяет выход за границы чанка и не изменяет координаты сущности
int ChunkAddEntity(Chunk *chunk, Entity *entity, uint32_t start_x, uint32_t start_y, uint32_t end_x, uint32_t end_y);

/// @brief Удаляет сущность из указанных клеток чанка (вынимает из связных списков grid)
/// @param chunk  Указатель на чанк
/// @param entity Указатель на сущность
/// @param start_x Начальная локальная X-координата
/// @param start_y Начальная локальная Y-координата
/// @param end_x   Конечная локальная X-координата (включительно)
/// @param end_y   Конечная локальная Y-координата (включительно)
/// @return 0 при успехе, -1 если аргументы NULL, -2 если сущность не найдена в указанном диапазоне
/// @note Эта функция не освобождает память сущности, а только убирает её из сетки чанка
int ChunkRemoveEntity(Chunk *chunk, Entity *entity, uint32_t start_x, uint32_t start_y, uint32_t end_x, uint32_t end_y);
