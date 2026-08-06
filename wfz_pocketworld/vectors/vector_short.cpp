#include "vector_short.h"

Vector2DShort::Vector2DShort(int16_t x, int16_t y) noexcept : x_(x), y_(y) {}
Vector2DShort::Vector2DShort() noexcept : x_(0), y_(0) {}

//

int16_t Vector2DShort::GetX() const noexcept
{
    return x_;
}

int16_t Vector2DShort::GetY() const noexcept
{
    return y_;
}

//

Vector2DShort Vector2DShort::operator+(const Vector2DShort &other) const noexcept
{
    return Vector2DShort(x_ + other.x_, y_ + other.y_);
}

Vector2DShort Vector2DShort::operator+(int16_t value) const noexcept
{
    return Vector2DShort(x_ + value, y_ + value);
}

Vector2DShort Vector2DShort::operator-(const Vector2DShort &other) const noexcept
{
    return Vector2DShort(x_ - other.x_, y_ - other.y_);
}

Vector2DShort Vector2DShort::operator-(int16_t value) const noexcept
{
    return Vector2DShort(x_ - value, y_ - value);
}

Vector2DShort Vector2DShort::operator*(int16_t value) const noexcept
{
    return Vector2DShort((int16_t)(x_ * value), (int16_t)(y_ * value));
}

Vector2DShort Vector2DShort::operator/(int16_t value) const noexcept
{
    return Vector2DShort(x_ / value, y_ / value);
}
