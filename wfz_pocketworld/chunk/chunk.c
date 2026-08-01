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
    chunk->entity_capacity = 4;
    chunk->entities = malloc(sizeof(Entity *) * chunk->entity_capacity);
    if (chunk->entities == NULL)
    {
        free(chunk);
        return NULL;
    }

    return chunk;
}

void ChunkFree(Chunk *chunk)
{
    if (!chunk)
        return;

    for (uint32_t i = 0; i < chunk->entity_count; i++)
    {
        EntityFree(chunk->entities[i]);
    }

    free(chunk->entities);
    free(chunk);
}

int ChunkAddEntity(Chunk *chunk, Entity *entity)
{
    if (!chunk || !entity)
        return -1;

    if (chunk->entity_count >= chunk->entity_capacity)
    {
        uint32_t new_cap = chunk->entity_capacity == 0 ? 4 : chunk->entity_capacity * 2;
        Entity **tmp = realloc(chunk->entities, sizeof(Entity *) * new_cap);
        if (!tmp)
            return -2;

        chunk->entities = tmp;
        chunk->entity_capacity = new_cap;
    }

    chunk->entities[chunk->entity_count++] = entity;
    return 0;
}

int ChunkRemoveEntity(Chunk *chunk, uint32_t entity_id)
{
    if (!chunk)
        return -1;

    for (uint32_t i = 0; i < chunk->entity_count; i++)
    {
        if (chunk->entities[i]->id == entity_id)
        {
            chunk->entities[i] = chunk->entities[--chunk->entity_count];
            return 0;
        }
    }
    return -2;
}

Entity *ChunkFindEntity(Chunk *chunk, uint32_t entity_id)
{
    if (!chunk)
        return NULL;

    for (uint32_t i = 0; i < chunk->entity_count; i++)
    {
        if (chunk->entities[i]->id == entity_id)
            return chunk->entities[i];
    }
    return NULL;
}
