#pragma once

#include <stdlib.h>
#include <stdint.h>
#include "chunk.h"
#include "entity.h"

/**
 * @brief Флаги плоскости (битовые маски)
 */
enum PlaneFlags
{
    PLANE_HAS_RAD_SIM = (1 << 0),  ///< Включена симуляция радиации
    PLANE_HAS_GAS_SIM = (1 << 1),  ///< Включена симуляция газов (атмосферы)
    PLANE_HAS_MANA_SIM = (1 << 2), ///< Включена симуляция маны

    PLANE_SOLID_X = (1 << 3), ///< Граница по X непроходима (SOLID)
    PLANE_SOLID_Y = (1 << 4), ///< Граница по Y непроходима (SOLID)
    PLANE_WRAP_X = (1 << 5),  ///< Координаты зациклены по X (WRAP)
    PLANE_WRAP_Y = (1 << 6),  ///< Координаты зациклены по Y (WRAP)

    PLANE_FIX_X = (1 << 7), ///< Координаты по X фиксированы (не уверен что нужно)
    PLANE_FIX_Y = (1 << 8), ///< Координаты по Y фиксированы (не уверен что нужно)

    // ещё до 31
};

/**
 * @brief Описывает одну игровую плоскость (мир, уровень, арену)
 */
typedef struct Plane
{
    /// @brief Уникальный идентификатор плоскости
    uint32_t id;

    /// @brief Флаги плоскости (см. PlaneFlags)
    uint32_t flags;

    /// @brief Максимальные занятые координаты (внутренние, беззнаковые)
    uint32_t max_x, max_y;

    /// @brief Смещение начала координат для внешнего (человеческого) представления
    int64_t origin_x, origin_y;

    /// @brief Хеш-таблица чанков
    Chunk *chunks;

    /// @brief Глобальная хеш-таблица всех сущностей плоскости (по ID)
    Entity *entities_by_id;

    /// @brief Следующий свободный идентификатор для сущности
    uint32_t next_entity_id;
} Plane;

/// @brief Создаёт новую пустую плоскость
/// @param flags Флаги плоскости (PlaneFlags)
/// @return Указатель на плоскость или NULL при ошибке памяти
Plane *PlaneNew(uint32_t flags);

/// @brief Уничтожает плоскость и освобождает всю связанную память
/// @param pointer Указатель на плоскость (может быть NULL)
void PlaneFree(Plane *pointer);

/// @brief Размещает сущность в плоскости (назначает ID, записывает в чанки и глобальный индекс)
/// @param plane Указатель на плоскость
/// @param entity Указатель на сущность (поля type, flags, width, height, rotation должны быть заполнены)
/// @param ext_x Внешняя X-координата (может быть отрицательной)
/// @param ext_y Внешняя Y-координата
/// @return ID созданной сущности ( > 0 ), или 0 при ошибке
uint32_t PlaneAddEntity(Plane *plane, Entity *entity, int64_t ext_x, int64_t ext_y);

/// @brief Находит сущность по её уникальному ID
/// @param plane Указатель на плоскость
/// @param entity_id ID сущности
/// @return Указатель на сущность или NULL
Entity *PlaneFindEntity(Plane *plane, uint32_t entity_id);

/// @brief Удаляет сущность из плоскости (из чанков, глобального индекса) и освобождает память
/// @param plane Указатель на плоскость
/// @param entity_id ID сущности
/// @return 0 если успешно, -1 если плоскость NULL, -2 если сущность не найдена
int PlaneRemoveEntity(Plane *plane, uint32_t entity_id);

/// @brief Возвращает голову связного списка сущностей в заданной клетке (внешние координаты)
/// @param plane Указатель на плоскость
/// @param ext_x Внешняя X-координата
/// @param ext_y Внешняя Y-координата
/// @return Указатель на первую Entity в клетке, или NULL если клетка пуста
Entity *PlaneGetEntitiesAt(Plane *plane, int64_t ext_x, int64_t ext_y);

/// @brief Устанавливает новый поворот сущности и полностью перестраивает её в чанках
/// @param plane Указатель на плоскость
/// @param entity_id ID сущности
/// @param rotation Новый поворот (0-7)
/// @return 0 если успешно, -1 если плоскость NULL, -2 если сущность не найдена
int PlaneSetEntityRotation(Plane *plane, uint32_t entity_id, uint8_t rotation);

/// @brief Перемещает сущность с проверкой проходимости (флаг ENTITY_SOLID)
/// @param plane Указатель на плоскость
/// @param entity_id ID сущности
/// @param new_ext_x Новая внешняя X-координата
/// @param new_ext_y Новая внешняя Y-координата
/// @return 0 если успешно, -1 если плоскость NULL, -2 если сущность не найдена, -3 если путь заблокирован
int PlaneMoveEntity(Plane *plane, uint32_t entity_id, int64_t new_ext_x, int64_t new_ext_y);

/// @brief Телепортирует сущность (с опциональной проверкой коллизий)
/// @param plane Указатель на плоскость
/// @param entity_id ID сущности
/// @param new_ext_x Новая внешняя X-координата
/// @param new_ext_y Новая внешняя Y-координата
/// @param force 1 - переместить без проверки коллизий, 0 - проверить проходимость
/// @return 0 если успешно, -1 если плоскость NULL, -2 если сущность не найдена, -3 если путь заблокирован (только при force=0)
int PlaneTeleportEntity(Plane *plane, uint32_t entity_id, int64_t new_ext_x, int64_t new_ext_y, uint8_t force);
