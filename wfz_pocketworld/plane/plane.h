#pragma once

#include <stdlib.h>
#include <stdint.h>

enum PlaneFlags
{
    /// @brief Указывает что текущая поверхность редактируется
    PLANE_EDITING = (1 << 0),

    // Следующий блок включения выключения симуляци

    /// @brief Указывает что текущая поверхность симулирует атмосферу
    PLANE_HAS_GAS_SIM = (1 << 1),

    /// @brief Указывает что текущая поверхность симулирует ману
    PLANE_HAS_MANA_SIM = (1 << 2),

    // Немного управления координатами

    /// @brief Указывает что за x координату нельзя пройти
    PLANE_SOLID_X = (1 << 3),

    /// @brief Указывает что за y координату пройти нельзя
    PLANE_SOLID_Y = (1 << 4),

    /// @brief Указывает что координаты зациклены по x
    PLANE_WRAP_X = (1 << 5),

    /// @brief Указывает что координаты зациклены по y
    PLANE_WRAP_Y = (1 << 6),

    /// @brief Указывает что координаты по x зафиксированы (не уверен что нужно)
    PLANE_FIX_X = (1 << 7),

    /// @brief Указывает что координаты по x зафиксированы (не уверен что нужно)
    PLANE_FIX_Y = (1 << 8),

    // ещё до 31
};

typedef struct Plane
{
    /// @brief Уникальный индификатор плоскости
    uint32_t id;

    /// @brief Флаги плоскости @see PlaneFlags
    uint32_t flags;

    /// @brief Максимальные координаты
    uint32_t max_x, max_y;

    /// @brief Координаты отчёта (0,0) плоскости
    int64_t origin_x, origin_y;
} Plane; // 32B

/// @brief Создаёт и инициализирует поверхность
/// @param flags Флаги поверности. @see PlaneFlags
/// @return Указатель на структуру если таковая есть
Plane *PlaneNew(uint32_t flags);

/// @brief Освобождает память от поверхности
/// @param pointer Указатель на поверхность
void PlaneFree(Plane *pointer);
