#pragma once

#include <cstdint>

#include "coordinate.h"
#include "footprint.h"
#include "plane.h"

struct LightCell
{
    uint16_t r = 0;
    uint16_t g = 0;
    uint16_t b = 0;
    uint16_t intensity = 0;
};

class LightSource
{
public:
    Coordinate coord = {0, 0};
    LightCell strength;

    bool enabled = true;
    bool isDirty = true;

private:
    Footprint<LightCell> foot_;

public:
    LightSource() = default;
    LightSource(Coordinate c, LightCell s) : coord(c), strength(s) {}

    void BuildFootprint(Plane *plane);
    void ApplyToPlane(Plane *plane);
    void RemoveFromPlane(Plane *plane);
    void SetEnabled(bool on) { enabled = on; }
};
