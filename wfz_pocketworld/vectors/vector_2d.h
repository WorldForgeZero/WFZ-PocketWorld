#pragma once

#include <cstdint>
#include <cstdlib>

class Vector2D
{
private:
    int32_t x_, y_;

public:
    Vector2D(int32_t x, int32_t y) noexcept;
    Vector2D() noexcept;

    ~Vector2D() = default;

    int32_t GetX() const noexcept { return x_; };
    int32_t GetY() const noexcept { return y_; };

    void SetX(int32_t x) noexcept { x_ = x; };
    void SetY(int32_t y) noexcept { y_ = y; };

    Vector2D operator+(const Vector2D &other) const noexcept;
    Vector2D operator+(int32_t value) const noexcept;

    Vector2D operator-(const Vector2D &other) const noexcept;
    Vector2D operator-(int32_t value) const noexcept;

    Vector2D operator*(int32_t value) const noexcept;
    Vector2D operator/(int32_t value) const noexcept;
};
