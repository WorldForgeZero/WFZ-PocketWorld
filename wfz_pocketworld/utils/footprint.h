#pragma once

#include <cstdint>
#include <cstdlib>
#include <vector>

#include "coordinate.h"

struct FootprintTemplate
{
    std::vector<Coordinate> offsets[4]; // индексы: 0=0°, 1=90°, 2=180°, 3=270°
};

template <typename T>
class Footprint
{
private:
    struct Cell
    {
        Coordinate coord;
        T value;
    };
    std::vector<Cell> cells_;

public:
    void AddCell(uint32_t x, uint32_t y, T value)
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
