#include "plane_manager.h"

PlaneManager plane_manager = {0};

int InitPlaneManager()
{
    if (plane_manager.planes != NULL)
        return -1;

    plane_manager.plane_capacity = 4;
    plane_manager.plane_count = 0;

    plane_manager.planes = malloc(sizeof(Plane *) * plane_manager.plane_capacity);
    if (plane_manager.planes == NULL)
        return -2;

    return 0;
}

int DestroyPlaneManager()
{
    if (plane_manager.planes == NULL)
        return -1;

    for (unsigned int i = 0; i < plane_manager.plane_count; i++)
        PlaneFree(plane_manager.planes[i]);

    plane_manager.plane_capacity = 0;
    plane_manager.plane_count = 0;

    free(plane_manager.planes);
    plane_manager.planes = NULL;
    return 0;
}

unsigned int AddPlane(unsigned int flags)
{
    if (plane_manager.planes == NULL)
        return 0;

    Plane *p = PlaneNew(flags);
    if (p == NULL)
        return 0;

    if (plane_manager.plane_count >= plane_manager.plane_capacity)
    {
        unsigned int new_cap = plane_manager.plane_capacity * 2;
        Plane **tmp = realloc(plane_manager.planes, sizeof(Plane *) * new_cap);
        if (tmp == NULL)
        {
            PlaneFree(p);
            return 0;
        }
        plane_manager.planes = tmp;
        plane_manager.plane_capacity = new_cap;
    }

    plane_manager.planes[plane_manager.plane_count++] = p;
    return p->id;
}

int DestroyPlane(unsigned int plane_id)
{
    if (plane_manager.planes == NULL)
        return -1;

    for (unsigned int i = 0; i < plane_manager.plane_count; i++)
    {
        if (plane_manager.planes[i]->id == plane_id)
        {
            PlaneFree(plane_manager.planes[i]);

            plane_manager.planes[i] = plane_manager.planes[--plane_manager.plane_count];
            return 0;
        }
    }

    return -2;
}
