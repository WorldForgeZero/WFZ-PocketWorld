#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "entity.h"
#include "uthash.h"

#define CHUNK_SHIFT 4
#define CHUNK_SIZE (1 << CHUNK_SHIFT) // 16

typedef struct
{
    uint32_t cx;
    uint32_t cy;
} ChunkKey;

typedef struct Chunk
{
    /// @brief Координаты чанка в сетке плоскости
    ChunkKey key;

    Entity *grid[CHUNK_SIZE][CHUNK_SIZE];

    /// @brief Текущее количество сущностей в чанке
    uint32_t entity_count;

    /// @brief Внутренний указатель для хеш-таблицы (uthash)
    UT_hash_handle hh;
} Chunk; // TODO:B + 56B https://troydhanson.github.io/uthash/userguide.html#_a_word_about_memory

/// @brief Создаёт новый пустой чанк
/// @param cx Координата X чанка в сетке
/// @param cy Координата Y чанка в сетке
/// @return Указатель на созданный чанк или NULL при ошибке памяти
Chunk *ChunkNew(uint32_t cx, uint32_t cy);

/// @brief Освобождает чанк и все сущности внутри него
/// @param chunk Указатель на чанк (может быть NULL)
void ChunkFree(Chunk *chunk);

/// @brief Добавляет сущность в чанк
/// @param chunk Указатель на чанк
/// @param entity Указатель на сущность
/// @return 0 при успехе, -1 если аргументы NULL, -2 если не удалось расширить память
int ChunkAddEntity(Chunk *chunk, Entity *entity);

/// @brief Удаляет сущность из сетки чанка (не освобождает память)
/// @param chunk Указатель на чанк
/// @param entity Указатель на удаляемую сущность
/// @return 0 при успехе, -1 если аргументы NULL, -2 если сущность не найдена
int ChunkRemoveEntity(Chunk *chunk, Entity *entity);
