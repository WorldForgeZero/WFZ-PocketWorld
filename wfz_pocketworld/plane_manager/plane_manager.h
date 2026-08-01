#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "plane.h"

typedef struct PlaneManager
{
    /// @brief Массив указателей на плоскости
    Plane **items;

    /// @brief Текущее количество указателей
    uint32_t count;

    /// @brief Максимальный резерв указателей
    uint32_t capacity;
} PlaneManager;

extern PlaneManager plane_manager;

/// @brief Инициализатор plane_manager
/// @return `0` - Успешно
///
/// `-1` - Объект уже инициализирован
///
/// `-2` - Нехватило памяти под выделение
int InitPlaneManager(void);

/// @brief Освобождает память занятую менеджером
/// @return `0` - Успешкно
///
/// `-1` - Объект не инициализирован
int DestroyPlaneManager(void);

/// @brief Создаёт поверхность в менеджере поверхностей
/// @param flags Флаги поверхности
/// @return 0 - если ошибка инициализации. Иначе id поверхности
unsigned int AddPlane(unsigned int flags);

/// @brief Удаляет созданную через AddPlane() поверхность
/// @param plane_id индификатор поверхности
/// @return `0` - Успешно
///
/// `-1` - Менеджер не инициализирован
///
/// `-2` - Поверхность не найдена
int DestroyPlane(unsigned int plane_id);
