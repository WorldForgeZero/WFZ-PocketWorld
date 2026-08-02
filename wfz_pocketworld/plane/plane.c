#include <stdlib.h>
#include "plane.h"

static uint32_t next_plane_id = 1;

Plane *PlaneNew(uint32_t flags)
{
    Plane *p = malloc(sizeof(Plane));
    if (p == NULL)
        return NULL;

    p->id = next_plane_id++;
    p->flags = flags;

    p->chunks = NULL;
    p->entities_by_id = NULL;

    p->max_x = p->max_y = 0;
    p->origin_x = p->origin_y = 0;
    p->next_entity_id = 1;

    return p;
}

void PlaneFree(Plane *plane)
{
    if (plane == NULL)
        return;

    Entity *ent, *tmp_ent;
    HASH_ITER(hh, plane->entities_by_id, ent, tmp_ent)
    {
        HASH_DEL(plane->entities_by_id, ent);
    }

    Chunk *chunk, *tmp_chunk;
    HASH_ITER(hh, plane->chunks, chunk, tmp_chunk)
    {
        HASH_DEL(plane->chunks, chunk);
        ChunkFree(chunk);
    }

    free(plane);
}

static Chunk *PlaneGetChunk(Plane *plane, uint32_t cx, uint32_t cy)
{
    if (!plane)
        return NULL;

    ChunkKey key = {cx, cy};
    Chunk *chunk = NULL;
    HASH_FIND(hh, plane->chunks, &key, sizeof(ChunkKey), chunk);
    if (chunk)
        return chunk;

    Chunk *new_chunk = ChunkNew(cx, cy);
    if (!new_chunk)
        return NULL;

    HASH_ADD(hh, plane->chunks, key, sizeof(ChunkKey), new_chunk);
    return new_chunk;
}

uint32_t PlaneAddEntity(Plane *plane, Entity *entity, int64_t ext_x, int64_t ext_y)
{
    if (!plane || !entity)
        return 0;

    int64_t ix = ext_x + plane->origin_x;
    int64_t iy = ext_y + plane->origin_y;

    if (ix < 0)
    {
        plane->origin_x -= ix;
        ix = 0;
    }
    if (iy < 0)
    {
        plane->origin_y -= iy;
        iy = 0;
    }

    entity->x = (uint32_t)ix;
    entity->y = (uint32_t)iy;

    uint32_t cx = entity->x >> CHUNK_SHIFT;
    uint32_t cy = entity->y >> CHUNK_SHIFT;

    Chunk *chunk = PlaneGetChunk(plane, cx, cy);
    if (!chunk)
        return 0;

    entity->id = plane->next_entity_id++;

    HASH_ADD(hh, plane->entities_by_id, id, sizeof(uint32_t), entity);

    if (ChunkAddEntity(chunk, entity) != 0)
    {
        HASH_DEL(plane->entities_by_id, entity);
        return 0;
    }

    uint32_t ex = entity->x + entity->width - 1;
    uint32_t ey = entity->y + entity->height - 1;
    if (ex > plane->max_x)
        plane->max_x = ex;

    if (ey > plane->max_y)
        plane->max_y = ey;

    return entity->id;
}

Entity *PlaneFindEntity(Plane *plane, uint32_t entity_id)
{
    if (!plane)
        return NULL;

    Entity *entity = NULL;
    HASH_FIND(hh, plane->entities_by_id, &entity_id, sizeof(uint32_t), entity);
    return entity;
}

int PlaneRemoveEntity(Plane *plane, uint32_t entity_id)
{
    if (!plane)
        return -1;

    Entity *entity = PlaneFindEntity(plane, entity_id);
    if (!entity)
        return -2;

    uint32_t cx = entity->x >> CHUNK_SHIFT;
    uint32_t cy = entity->y >> CHUNK_SHIFT;

    Chunk *chunk = PlaneGetChunk(plane, cx, cy);
    if (!chunk)
        return -2;

    ChunkRemoveEntity(chunk, entity);

    if (chunk->entity_count == 0)
    {
        HASH_DEL(plane->chunks, chunk);
        ChunkFree(chunk);
    }

    HASH_DEL(plane->entities_by_id, entity);

    EntityFree(entity);

    return 0;
}

Entity *PlaneGetEntitiesAt(Plane *plane, int64_t ext_x, int64_t ext_y)
{
    if (!plane)
        return NULL;

    int64_t ix = ext_x + plane->origin_x;
    int64_t iy = ext_y + plane->origin_y;

    if (ix < 0 || iy < 0)
        return NULL;

    if (ix > plane->max_x || iy > plane->max_y)
        return NULL;

    uint32_t x = (uint32_t)ix;
    uint32_t y = (uint32_t)iy;

    uint32_t cx = x >> CHUNK_SHIFT;
    uint32_t cy = y >> CHUNK_SHIFT;

    Chunk *chunk = PlaneGetChunk(plane, cx, cy);
    if (!chunk)
        return NULL;

    uint32_t local_x = x & (CHUNK_SIZE - 1);
    uint32_t local_y = y & (CHUNK_SIZE - 1);

    return chunk->grid[local_x][local_y];
}
