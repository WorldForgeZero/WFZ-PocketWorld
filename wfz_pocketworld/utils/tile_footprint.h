#pragma once

#include <cstdint>
#include <cstdlib>
#include <vector>

#include "coordinate.h"

template <typename T>
class TileFootprint
{
private:
    struct Cell
    {
        Coordinate coord;
        T value;
    };
    std::vector<Cell> cells_;

public:
    void AddCell(int32_t x, int32_t y, T value)
    {
        cells_.push_back({{x, y}, value});
    }

    template <typename Func>
    void Apply(Func &&func) const
    {
        for (const auto &c : cells_)
        {
            func(c.coord, c.value);
        }
    }

    void Clear() { cells_.clear(); }

    bool IsEmpty() const { return cells_.empty(); }
};
