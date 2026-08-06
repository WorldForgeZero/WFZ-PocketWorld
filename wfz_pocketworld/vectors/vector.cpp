#include "vector.h"

Vector2D::Vector2D(int32_t x, int32_t y) noexcept : x_(x), y_(y) {}
Vector2D::Vector2D() noexcept : x_(0), y_(0) {}

//

int32_t Vector2D::GetX() const noexcept
{
    return x_;
}

int32_t Vector2D::GetY() const noexcept
{
    return y_;
}

//

Vector2D Vector2D::operator+(const Vector2D &other) const noexcept
{
    return Vector2D(x_ + other.x_, y_ + other.y_);
}

Vector2D Vector2D::operator+(int32_t value) const noexcept
{
    return Vector2D(x_ + value, y_ + value);
}

Vector2D Vector2D::operator-(const Vector2D &other) const noexcept
{
    return Vector2D(x_ - other.x_, y_ - other.y_);
}

Vector2D Vector2D::operator-(int32_t value) const noexcept
{
    return Vector2D(x_ - value, y_ - value);
}

Vector2D Vector2D::operator*(int32_t value) const noexcept
{
    return Vector2D((int32_t)(x_ * value), (int32_t)(y_ * value));
}

Vector2D Vector2D::operator/(int32_t value) const noexcept
{
    return Vector2D(x_ / value, y_ / value);
}
