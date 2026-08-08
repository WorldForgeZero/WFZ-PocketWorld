#pragma once

#include <stdint.h>
#include <stdlib.h>

class Vector2D
{
private:
    int32_t x_, y_;

public:
    Vector2D(int32_t x, int32_t y) noexcept;
    Vector2D() noexcept;

    ~Vector2D() = default;

    int32_t GetX() const noexcept;
    int32_t GetY() const noexcept;

    Vector2D operator+(const Vector2D &other) const noexcept;
    Vector2D operator+(int32_t value) const noexcept;

    Vector2D operator-(const Vector2D &other) const noexcept;
    Vector2D operator-(int32_t value) const noexcept;

    Vector2D operator*(int32_t value) const noexcept;
    Vector2D operator/(int32_t value) const noexcept;
};
