#pragma once

#include <stdlib.h>
#include <stdint.h>

enum EntityFlags
{
    /// @brief Заглушка пока не сделаю нормальные флаги энтитям
    ENTITY_STUB = (1 << 0),
};

typedef struct Entity
{
    /// @brief Уникальный (в рамках одной плоскости) индификатор энтити
    uint32_t id;

    /// @brief Айди типа префаба
    uint32_t type;

    /// @brief Флаги @see EntityFlags
    uint32_t flags;

    /// @brief Координаты сущности
    uint32_t x, y;

    /// @brief Размеры сущности
    uint8_t width, height;

    /// @brief Я не буду это описывать
    uint16_t _padding;
} Entity; // 24B

/// @brief Выделает поинтер на новый энтити
/// @param type Тип энтити
/// @param flags Флаги @see EntityFlags
/// @param width Ширина энтити
/// @param height Высота энтити
/// @return Указатель на энтити если создан успешно
/// @note Данная функция инициализирует `id`, `x`, `y` = 0 в связи с тем что данный энтити не установлен в мире при выделении памяти.
Entity *EntityNew(uint32_t type, uint32_t flags, uint8_t width, uint8_t height);

/// @brief Освобождает память из под энтити
/// @param pointer Указатель на энтити
/// @note Данная функция не трогает всё что связано не с освобождением памяти извне. Висящие ID и прочее привет!
void EntityFree(Entity *pointer);
