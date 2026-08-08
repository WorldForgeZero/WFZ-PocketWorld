#pragma once

#include <stdint.h>
#include <stdlib.h>

class Vector2DShort
{
private:
    int16_t x_, y_;

public:
    Vector2DShort(int16_t x, int16_t y) noexcept;
    Vector2DShort() noexcept;

    ~Vector2DShort() = default;

    int16_t GetX() const noexcept;
    int16_t GetY() const noexcept;

    Vector2DShort operator+(const Vector2DShort &other) const noexcept;
    Vector2DShort operator+(int16_t value) const noexcept;

    Vector2DShort operator-(const Vector2DShort &other) const noexcept;
    Vector2DShort operator-(int16_t value) const noexcept;

    Vector2DShort operator*(int16_t value) const noexcept;
    Vector2DShort operator/(int16_t value) const noexcept;
};
