#pragma once
#include <stdlib.h>
#include <stdint.h>
#include "uthash.h"

/// @brief Флаги сущностей (битовые маски)
enum EntityFlags
{
    ENTITY_SOLID = (1 << 0), ///< Блокирует проход
    // ещё до 31
};

/// @brief Представление игрового объекта в C-ядре
typedef struct Entity
{
    /// @brief Уникальный идентификатор в пределах плоскости
    uint32_t id;

    /// @brief Идентификатор типа (префаба), назначается ядром при регистрации префаба
    uint32_t type;

    /// @brief Битовые флаги (@see EntityFlags)
    uint32_t flags;

    /// @brief Внутренние координаты X, Y (беззнаковые, с учётом origin плоскости)
    uint32_t x, y;

    /// @brief Размеры сущности в клетках (ширина, высота)
    uint8_t width, height;

    /// @brief Поворот сущности: 0-7 (N, NE, E, SE, S, SW, W, NW)
    uint8_t rotation;

    /// @brief Изоляция от газов (0 - полностью пропускает, 100 - полностью блокирует)
    uint8_t gas_isol;

    /// @brief Изоляция от радиации (0 - пропускает, 100 - блокирует)
    uint8_t rad_isol;

    /// @brief Изоляция от маны (0 - пропускает, 100 - блокирует)
    uint8_t mana_isol;

    /// @brief Внутренний указатель для глобальной хеш-таблицы плоскости (uthash)
    UT_hash_handle hh;

    /// @brief Указатель на следующую сущность в связном списке клетки (используется в grid чанка)
    struct Entity *cell_next;
} Entity;

/// @brief Создаёт новую сущность (не размещённую в мире)
/// @param type      Идентификатор типа (префаба)
/// @param flags     Флаги сущности (@see EntityFlags)
/// @param width     Ширина в клетках
/// @param height    Высота в клетках
/// @param rotation  Начальный поворот (0-7)
/// @return Указатель на созданную сущность, или NULL при ошибке памяти
/// @note После создания id == 0, координаты x,y == 0, cell_next == NULL.
///       Изоляция (gas/rad/mana) по умолчанию равна 100 (полная блокировка).
///       Для размещения в мире используйте PlaneAddEntity().
Entity *EntityNew(uint32_t type, uint32_t flags, uint8_t width, uint8_t height, uint8_t rotation);

/// @brief Освобождает память, занятую сущностью
/// @param pointer Указатель на сущность (может быть NULL)
/// @note Эта функция только освобождает память структуры Entity.
///       Она не удаляет сущность из чанков, хеш-таблиц или других индексов.
///       Висящие ссылки и ID - привет! Разруливать их должен вызывающий код (PlaneRemoveEntity).
void EntityFree(Entity *pointer);
