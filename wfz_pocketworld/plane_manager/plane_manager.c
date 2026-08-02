#include "plane_manager.h"
#include <stdlib.h>

PlaneManager plane_manager = {0};

int InitPlaneManager(void)
{
    if (plane_manager.planes.items != NULL)
        return -1;

    return PlaneArray_Init(&plane_manager.planes);
}

int DestroyPlaneManager(void)
{
    if (plane_manager.planes.items == NULL)
        return -1;

    for (uint32_t i = 0; i < plane_manager.planes.count; i++)
    {
        PlaneFree(plane_manager.planes.items[i]);
    }

    PlaneArray_Free(&plane_manager.planes);
    return 0;
}

unsigned int AddPlane(unsigned int flags)
{
    if (plane_manager.planes.items == NULL)
        return 0;

    Plane *p = PlaneNew(flags);
    if (!p)
        return 0;

    if (PlaneArray_Add(&plane_manager.planes, p) != 0)
    {
        PlaneFree(p);
        return 0;
    }

    return p->id;
}

int DestroyPlane(unsigned int plane_id)
{
    if (plane_manager.planes.items == NULL)
        return -1;

    for (uint32_t i = 0; i < plane_manager.planes.count; i++)
    {
        if (plane_manager.planes.items[i]->id == plane_id)
        {
            Plane *p = plane_manager.planes.items[i];
            PlaneArray_Remove(&plane_manager.planes, i);
            PlaneFree(p);
            return 0;
        }
    }

    return -2;
}
