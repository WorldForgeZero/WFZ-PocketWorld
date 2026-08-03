#include "chunk.h"
#include <stdlib.h>

Chunk *ChunkNew(uint32_t cx, uint32_t cy)
{
    Chunk *chunk = malloc(sizeof(Chunk));
    if (!chunk)
        return NULL;

    chunk->key.cx = cx;
    chunk->key.cy = cy;
    chunk->entity_count = 0;
    memset(chunk->grid, 0, sizeof(chunk->grid));
    return chunk;
}

void ChunkFree(Chunk *chunk)
{
    if (!chunk)
        return;

    for (uint32_t y = 0; y < CHUNK_SIZE; y++)
    {
        for (uint32_t x = 0; x < CHUNK_SIZE; x++)
        {
            Entity *ent = chunk->grid[x][y];
            while (ent)
            {
                Entity *next = ent->cell_next;
                EntityFree(ent);
                ent = next;
            }
        }
    }

    free(chunk);
}

int ChunkAddEntity(Chunk *chunk, Entity *entity, uint32_t start_x, uint32_t start_y, uint32_t end_x, uint32_t end_y)
{
    if (!chunk || !entity)
        return -1;

    for (uint32_t y = start_y; y <= end_y; y++)
    {
        for (uint32_t x = start_x; x <= end_x; x++)
        {
            entity->cell_next = chunk->grid[x][y];
            chunk->grid[x][y] = entity;
        }
    }

    chunk->entity_count++;
    return 0;
}

int ChunkRemoveEntity(Chunk *chunk, Entity *entity, uint32_t start_x, uint32_t start_y, uint32_t end_x, uint32_t end_y)
{
    if (!chunk || !entity)
        return -1;

    uint8_t found = 0;
    for (uint32_t y = start_y; y <= end_y; y++)
    {
        for (uint32_t x = start_x; x <= end_x; x++)
        {
            Entity **prev = &chunk->grid[x][y];
            while (*prev)
            {
                if (*prev == entity)
                {
                    *prev = entity->cell_next;
                    found = 1;
                    break;
                }
                prev = &(*prev)->cell_next;
            }
        }
    }

    if (!found)
        return -2;

    entity->cell_next = NULL;
    chunk->entity_count--;
    return 0;
}
