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

int ChunkAddEntity(Chunk *chunk, Entity *entity)
{
    if (!chunk || !entity)
        return -1;

    uint32_t local_x = entity->x & (CHUNK_SIZE - 1);
    uint32_t local_y = entity->y & (CHUNK_SIZE - 1);

    for (uint8_t dy = 0; dy < entity->height; dy++)
    {
        for (uint8_t dx = 0; dx < entity->width; dx++)
        {
            uint32_t lx = (local_x + dx) & (CHUNK_SIZE - 1);
            uint32_t ly = (local_y + dy) & (CHUNK_SIZE - 1);
            entity->cell_next = chunk->grid[lx][ly];
            chunk->grid[lx][ly] = entity;
        }
    }

    chunk->entity_count++;
    return 0;
}

int ChunkRemoveEntity(Chunk *chunk, Entity *entity)
{
    if (!chunk || !entity)
        return -1;

    uint32_t local_x = entity->x & (CHUNK_SIZE - 1);
    uint32_t local_y = entity->y & (CHUNK_SIZE - 1);
    uint8_t found = 0;

    for (uint8_t dy = 0; dy < entity->height; dy++)
    {
        for (uint8_t dx = 0; dx < entity->width; dx++)
        {
            uint32_t lx = (local_x + dx) & (CHUNK_SIZE - 1);
            uint32_t ly = (local_y + dy) & (CHUNK_SIZE - 1);

            Entity **prev = &chunk->grid[lx][ly];
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
