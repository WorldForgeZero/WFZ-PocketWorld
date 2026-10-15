#pragma once

#include <cstdint>
#include <functional>

struct Coordinate
{
    int32_t x;
    int32_t y;

    bool operator==(const Coordinate &other) const
    {
        return x == other.x && y == other.y;
    }
};

namespace std
{
    template <>
    struct hash<Coordinate>
    {
        size_t operator()(const Coordinate &c) const noexcept
        {
            return std::hash<int32_t>()(c.x) ^ (std::hash<int32_t>()(c.y) << 1);
        }
    };
}
