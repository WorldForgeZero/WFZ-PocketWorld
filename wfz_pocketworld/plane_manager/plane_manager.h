#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "plane.h"
#include "dynarray.h"

DEFINE_DYNARRAY(Plane *, Plane)

typedef struct PlaneManager
{
    PlaneArray planes;
} PlaneManager;

extern PlaneManager plane_manager;

/// @brief Инициализатор plane_manager
/// @return 0 - успешно, -1 - уже инициализирован, -2 - не хватило памяти
int InitPlaneManager(void);

/// @brief Освобождает память занятую менеджером
/// @return 0 - успешно, -1 - объект не инициализирован
int DestroyPlaneManager(void);

/// @brief Создаёт поверхность в менеджере поверхностей
/// @param flags Флаги поверхности
/// @return 0 - если ошибка, иначе id поверхности
unsigned int AddPlane(unsigned int flags);

/// @brief Удаляет созданную через AddPlane() поверхность
/// @param plane_id идентификатор поверхности
/// @return 0 - успешно, -1 - менеджер не инициализирован, -2 - поверхность не найдена
int DestroyPlane(unsigned int plane_id);
