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

PlaneManager plane_manager = {0};
static unsigned int next_plane_id = 1;

Plane *PlaneNew(unsigned int flags)
{
    Plane *p = malloc(sizeof(Plane));
    if (p == NULL)
        return NULL;

    p->id = next_plane_id++;
    p->flags = flags;

    return p;
}

void PlaneFree(Plane *plane)
{
    if (plane == NULL)
        return;

    free(plane);
}

/// @brief Инициализатор plane_manager
/// @return `0` - Успешно
///
/// `-1` - Объект уже инициализирован
///
/// `-2` - Нехватило памяти под выделение
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

/// @brief Освобождает память занятую менеджером
/// @return `0` - Успешкно
///
/// `-1` - Объект не инициализирован
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

/// @brief Создаёт поверхность в менеджере поверхностей
/// @param flags Флаги поверхности
/// @return 0 - если ошибка инициализации. Иначе id поверхности
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

/// @brief Удаляет созданную через AddPlane поверхность
/// @param plane_id индификатор поверхности
/// @return `0` - Успешно
///
/// `-1` - Менеджер не инициализирован
///
/// `-2` - Поверхность не найдена
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
