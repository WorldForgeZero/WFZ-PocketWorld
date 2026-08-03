#include <stdlib.h>
#include "plane.h"

static uint32_t next_plane_id = 1;

// Проверка, свободна ли клетка для сущности self (есть ли SOLID кроме себя)
static int IsCellPassable(Plane *plane, Entity *self, uint32_t x, uint32_t y)
{
    Entity *head = PlaneGetEntitiesAt(plane, (int64_t)x, (int64_t)y);
    while (head)
    {
        if (head != self && (head->flags & ENTITY_SOLID))
            return 0;

        head = head->cell_next;
    }

    return 1;
}

// Получить или создать чанк по индексам
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

// Рассчитать локальные координаты внутри чанка cx/cy для сущности с координатами (x,y) и размерами (w,h)
static void GetChunkLocalRange(uint32_t cx, uint32_t cy, uint32_t x, uint32_t y, uint8_t w, uint8_t h, uint32_t *sx, uint32_t *sy, uint32_t *ex, uint32_t *ey)
{
    uint32_t chunk_mask = CHUNK_SIZE - 1;
    uint32_t cx1 = x >> CHUNK_SHIFT;
    uint32_t cy1 = y >> CHUNK_SHIFT;
    uint32_t cx2 = (x + w - 1) >> CHUNK_SHIFT;
    uint32_t cy2 = (y + h - 1) >> CHUNK_SHIFT;

    *sx = (cx == cx1) ? (x & chunk_mask) : 0;
    *sy = (cy == cy1) ? (y & chunk_mask) : 0;
    *ex = (cx == cx2) ? ((x + w - 1) & chunk_mask) : chunk_mask;
    *ey = (cy == cy2) ? ((y + h - 1) & chunk_mask) : chunk_mask;
}

// Добавить сущность во все чанки, которые она занимает
static void PlaneAddEntityToChunks(Plane *plane, Entity *entity)
{
    uint32_t cx1 = entity->x >> CHUNK_SHIFT;
    uint32_t cy1 = entity->y >> CHUNK_SHIFT;
    uint32_t cx2 = (entity->x + entity->width - 1) >> CHUNK_SHIFT;
    uint32_t cy2 = (entity->y + entity->height - 1) >> CHUNK_SHIFT;

    for (uint32_t cy = cy1; cy <= cy2; cy++)
    {
        for (uint32_t cx = cx1; cx <= cx2; cx++)
        {
            Chunk *chunk = PlaneGetChunk(plane, cx, cy);
            if (!chunk)
                continue;

            uint32_t sx, sy, ex, ey;
            GetChunkLocalRange(cx, cy, entity->x, entity->y, entity->width, entity->height, &sx, &sy, &ex, &ey);
            ChunkAddEntity(chunk, entity, sx, sy, ex, ey);
        }
    }
}

// Удалить сущность из всех чанков, которые она занимает
static void PlaneRemoveEntityFromChunks(Plane *plane, Entity *entity)
{
    uint32_t cx1 = entity->x >> CHUNK_SHIFT;
    uint32_t cy1 = entity->y >> CHUNK_SHIFT;
    uint32_t cx2 = (entity->x + entity->width - 1) >> CHUNK_SHIFT;
    uint32_t cy2 = (entity->y + entity->height - 1) >> CHUNK_SHIFT;

    for (uint32_t cy = cy1; cy <= cy2; cy++)
    {
        for (uint32_t cx = cx1; cx <= cx2; cx++)
        {
            Chunk *chunk = PlaneGetChunk(plane, cx, cy);
            if (!chunk)
                continue;

            uint32_t sx, sy, ex, ey;
            GetChunkLocalRange(cx, cy, entity->x, entity->y, entity->width, entity->height, &sx, &sy, &ex, &ey);
            ChunkRemoveEntity(chunk, entity, sx, sy, ex, ey);
        }
    }
}

// Удалить все опустевшие чанки из заданного диапазона
static void PlaneRemoveEmptyChunks(Plane *plane, uint32_t cx1, uint32_t cy1, uint32_t cx2, uint32_t cy2)
{
    for (uint32_t cy = cy1; cy <= cy2; cy++)
    {
        for (uint32_t cx = cx1; cx <= cx2; cx++)
        {
            ChunkKey key = {cx, cy};
            Chunk *chunk = NULL;
            HASH_FIND(hh, plane->chunks, &key, sizeof(ChunkKey), chunk);
            if (chunk && chunk->entity_count == 0)
            {
                HASH_DEL(plane->chunks, chunk);
                ChunkFree(chunk);
            }
        }
    }
}

// Обновить границы плоскости с учётом сущности
static void PlaneUpdateBounds(Plane *plane, Entity *entity)
{
    uint32_t ex = entity->x + entity->width - 1;
    uint32_t ey = entity->y + entity->height - 1;
    if (ex > plane->max_x)
        plane->max_x = ex;

    if (ey > plane->max_y)
        plane->max_y = ey;
}

// Преобразовать внешние координаты во внутренние с учётом origin
static int ToInternal(Plane *plane, int64_t ext_x, int64_t ext_y, uint32_t *ix, uint32_t *iy)
{
    int64_t x = ext_x + plane->origin_x;
    int64_t y = ext_y + plane->origin_y;
    if (x < 0)
    {
        plane->origin_x -= x;
        x = 0;
    }

    if (y < 0)
    {
        plane->origin_y -= y;
        y = 0;
    }

    *ix = (uint32_t)x;
    *iy = (uint32_t)y;
    return 0;
}

// Проверить, что вся область (x,y,w,h) проходима для self
static int CheckAreaPassable(Plane *plane, Entity *self, uint32_t x, uint32_t y, uint8_t w, uint8_t h)
{
    for (uint8_t dy = 0; dy < h; dy++)
        for (uint8_t dx = 0; dx < w; dx++)
            if (!IsCellPassable(plane, self, x + dx, y + dy))
                return 0;

    return 1;
}

Plane *PlaneNew(uint32_t flags)
{
    Plane *p = malloc(sizeof(Plane));
    if (!p)
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
    if (!plane)
        return;

    Entity *ent, *tmp_ent;
    HASH_ITER(hh, plane->entities_by_id, ent, tmp_ent)
    HASH_DEL(plane->entities_by_id, ent);

    Chunk *chunk, *tmp_chunk;
    HASH_ITER(hh, plane->chunks, chunk, tmp_chunk)
    {
        HASH_DEL(plane->chunks, chunk);
        ChunkFree(chunk);
    }
    free(plane);
}

uint32_t PlaneAddEntity(Plane *plane, Entity *entity, int64_t ext_x, int64_t ext_y)
{
    if (!plane || !entity)
        return 0;

    uint32_t ix, iy;
    ToInternal(plane, ext_x, ext_y, &ix, &iy);
    entity->x = ix;
    entity->y = iy;

    entity->id = plane->next_entity_id++;
    HASH_ADD(hh, plane->entities_by_id, id, sizeof(uint32_t), entity);

    PlaneAddEntityToChunks(plane, entity);
    PlaneUpdateBounds(plane, entity);
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

    uint32_t cx1 = entity->x >> CHUNK_SHIFT, cy1 = entity->y >> CHUNK_SHIFT;
    uint32_t cx2 = (entity->x + entity->width - 1) >> CHUNK_SHIFT;
    uint32_t cy2 = (entity->y + entity->height - 1) >> CHUNK_SHIFT;

    PlaneRemoveEntityFromChunks(plane, entity);
    PlaneRemoveEmptyChunks(plane, cx1, cy1, cx2, cy2);

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
    if (ix < 0 || iy < 0 || ix > (int64_t)plane->max_x || iy > (int64_t)plane->max_y)
        return NULL;

    uint32_t x = (uint32_t)ix, y = (uint32_t)iy;
    uint32_t cx = x >> CHUNK_SHIFT, cy = y >> CHUNK_SHIFT;
    Chunk *chunk = PlaneGetChunk(plane, cx, cy);
    if (!chunk)
        return NULL;

    return chunk->grid[x & (CHUNK_SIZE - 1)][y & (CHUNK_SIZE - 1)];
}

int PlaneSetEntityRotation(Plane *plane, uint32_t entity_id, uint8_t new_rotation)
{
    if (!plane)
        return -1;

    Entity *entity = PlaneFindEntity(plane, entity_id);
    if (!entity)
        return -2;

    PlaneRemoveEntityFromChunks(plane, entity);

    if ((entity->rotation & 1) != (new_rotation & 1))
    {
        uint8_t tmp = entity->width;
        entity->width = entity->height;
        entity->height = tmp;
    }
    entity->rotation = new_rotation;

    PlaneAddEntityToChunks(plane, entity);
    PlaneUpdateBounds(plane, entity);
    return 0;
}

int PlaneMoveEntity(Plane *plane, uint32_t entity_id, int64_t new_ext_x, int64_t new_ext_y)
{
    if (!plane)
        return -1;

    Entity *entity = PlaneFindEntity(plane, entity_id);
    if (!entity)
        return -2;

    uint32_t new_x, new_y;
    ToInternal(plane, new_ext_x, new_ext_y, &new_x, &new_y);

    if (!CheckAreaPassable(plane, entity, new_x, new_y, entity->width, entity->height))
        return -3;

    PlaneRemoveEntityFromChunks(plane, entity);
    entity->x = new_x;
    entity->y = new_y;
    PlaneAddEntityToChunks(plane, entity);
    PlaneUpdateBounds(plane, entity);
    return 0;
}

int PlaneTeleportEntity(Plane *plane, uint32_t entity_id, int64_t new_ext_x, int64_t new_ext_y, uint8_t force)
{
    if (!plane)
        return -1;

    Entity *entity = PlaneFindEntity(plane, entity_id);
    if (!entity)
        return -2;

    uint32_t new_x, new_y;
    ToInternal(plane, new_ext_x, new_ext_y, &new_x, &new_y);

    if (!force && !CheckAreaPassable(plane, entity, new_x, new_y, entity->width, entity->height))
        return -3;

    PlaneRemoveEntityFromChunks(plane, entity);
    entity->x = new_x;
    entity->y = new_y;
    PlaneAddEntityToChunks(plane, entity);
    PlaneUpdateBounds(plane, entity);
    return 0;
}
