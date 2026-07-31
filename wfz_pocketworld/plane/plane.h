#ifndef PLANE_H
#define PLANE_H

#include <stdlib.h>

typedef struct Plane
{
    unsigned int id;
    unsigned int flags;
} Plane;

typedef struct PlaneManager
{
    Plane **items;
    unsigned int count;
    unsigned int capacity;
} PlaneManager;

extern PlaneManager plane_manager;

Plane *PlaneNew(unsigned int flags);
void PlaneFree(Plane *plane);

int InitPlaneManager(void);
int DestroyPlaneManager(void);

unsigned int AddPlane(unsigned int flags);
int DestroyPlane(unsigned int plane_id);

#endif
