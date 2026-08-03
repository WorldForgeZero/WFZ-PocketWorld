#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "chunk.h"
#include "entity.h"

enum PlaneFlags
{
    // Следующий блок включения выключения симуляци

    /// @brief Указывает что текущая поверхность симулирует радиацию
    PLANE_HAS_RAD_SIM = (1 << 0),

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

    /// @brief Хеш таблица чанков
    Chunk *chunks;

    /// @brief Глобальная хеш-таблица всех сущностей (по ID)
    Entity *entities_by_id;

    /// @brief Следующий айдишник энтити
    uint32_t next_entity_id;
} Plane; // TODO: B

/// @brief Создаёт и инициализирует поверхность
/// @param flags Флаги поверности. @see PlaneFlags
/// @return Указатель на структуру если таковая есть
Plane *PlaneNew(uint32_t flags);

/// @brief Освобождает память от поверхности
/// @param pointer Указатель на поверхность
void PlaneFree(Plane *pointer);

/// @brief Добавляет сущность в плоскость
/// @param plane Указатель на плоскость
/// @param entity Указатель на сущность
/// @param ext_x X-координата
/// @param ext_y Y-координата
/// @return ID созданной сущности, или 0 при ошибке
uint32_t PlaneAddEntity(Plane *plane, Entity *entity, int64_t ext_x, int64_t ext_y);

/// @brief Ищет энтити по ID
/// @param plane Указатель на плоскость
/// @param entity_id ID энтити
/// @return Указатель на энтити или NULL
Entity *PlaneFindEntity(Plane *plane, uint32_t entity_id);

/// @brief Удаляет энтити из плоскости и при необходимости чистит пустой чанк
/// @param plane Указатель на плоскость
/// @param entity_id ID энтити
/// @return 0 если успешно, -1 если плоскость NULL, -2 если энтити не найдена
int PlaneRemoveEntity(Plane *plane, uint32_t entity_id);

/// @brief Получить список сущностей в заданной клетке
/// @param plane Указатель на плоскость
/// @param ext_x X-координата
/// @param ext_y Y-координата
/// @return Указатель на первую Entity в клетке, или NULL если клетка пуста
Entity *PlaneGetEntitiesAt(Plane *plane, int64_t ext_x, int64_t ext_y);
