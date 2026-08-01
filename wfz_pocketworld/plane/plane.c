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

    p->max_x = 0;
    p->max_y = 0;
    p->origin_x = 0;
    p->origin_y = 0;

    return p;
}

void PlaneFree(Plane *pointer)
{
    if (pointer == NULL)
        return;

    free(pointer);
}
