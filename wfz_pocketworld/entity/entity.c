#include "entity.h"

#include <stdlib.h>
#include <stdint.h>

Entity *EntityNew(uint32_t type, uint32_t flags, uint8_t width, uint8_t height)
{
    Entity *p = malloc(sizeof(Entity));
    if (p == NULL)
        return NULL;

    // См. доку
    p->id = 0;
    p->x = p->y = 0;

    // -.-
    p->_unised1 = 0;

    //
    p->type = type;
    p->flags = flags;

    p->width = width;
    p->height = height;

    return p;
}

void EntityFree(Entity *pointer)
{
    if (pointer == NULL)
        return;

    free(pointer);
}
