#include "entity.h"
#include <stdlib.h>

Entity *EntityNew(uint32_t type, uint32_t flags, uint8_t width, uint8_t height, uint8_t rotation)
{
    Entity *p = malloc(sizeof(Entity));
    if (!p)
        return NULL;

    p->id = 0;
    p->x = p->y = 0;
    p->cell_next = NULL;

    p->type = type;
    p->flags = flags;

    p->width = width;
    p->height = height;
    p->rotation = rotation;

    p->gas_isol = 100;
    p->rad_isol = 100;
    p->mana_isol = 100;

    return p;
}

void EntityFree(Entity *pointer)
{
    if (pointer == NULL)
        return;

    free(pointer);
}
