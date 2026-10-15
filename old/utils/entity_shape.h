#pragma once

#include <cstdint>
#include <cstdlib>
#include <vector>

#include "coordinate.h"

struct EntityShape
{
    std::vector<Coordinate> offsets[4]; // индексы: 0=0°, 1=90°, 2=180°, 3=270°
};
