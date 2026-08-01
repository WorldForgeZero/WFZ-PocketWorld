#include "plane_manager.h"

PlaneManager plane_manager = {0};

int InitPlaneManager()
{
    if (plane_manager.items != NULL)
        return -1;

    plane_manager.capacity = 4;
    plane_manager.count = 0;

    plane_manager.items = malloc(sizeof(Plane *) * plane_manager.capacity);
    if (plane_manager.items == NULL)
        return -2;

    return 0;
}

int DestroyPlaneManager()
{
    if (plane_manager.items == NULL)
        return -1;

    for (unsigned int i = 0; i < plane_manager.count; i++)
        PlaneFree(plane_manager.items[i]);

    plane_manager.capacity = 0;
    plane_manager.count = 0;

    free(plane_manager.items);
    plane_manager.items = NULL;
    return 0;
}

unsigned int AddPlane(unsigned int flags)
{
    if (plane_manager.items == NULL)
        return 0;

    Plane *p = PlaneNew(flags);
    if (p == NULL)
        return 0;

    if (plane_manager.count >= plane_manager.capacity)
    {
        unsigned int new_cap = plane_manager.capacity * 2;
        Plane **tmp = realloc(plane_manager.items, sizeof(Plane *) * new_cap);
        if (tmp == NULL)
        {
            PlaneFree(p);
            return 0;
        }
        plane_manager.items = tmp;
        plane_manager.capacity = new_cap;
    }

    plane_manager.items[plane_manager.count++] = p;
    return p->id;
}

int DestroyPlane(unsigned int plane_id)
{
    if (plane_manager.items == NULL)
        return -1;

    for (unsigned int i = 0; i < plane_manager.count; i++)
    {
        if (plane_manager.items[i]->id == plane_id)
        {
            PlaneFree(plane_manager.items[i]);

            plane_manager.items[i] = plane_manager.items[--plane_manager.count];
            return 0;
        }
    }

    return -2;
}
