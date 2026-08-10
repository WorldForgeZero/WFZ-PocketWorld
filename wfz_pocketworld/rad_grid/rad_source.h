#pragma once

#include <cstdint>

#include "coordinate.h"
#include "footprint.h"
#include "plane.h"

class RadSource
{
public:
    Coordinate coord = {0, 0};
    uint16_t strength = 0;
    bool enabled = true;
    bool isDirty = true;

private:
    Footprint<uint16_t> foot_;

public:
    RadSource() = default;
    RadSource(Coordinate c, uint16_t s) : coord(c), strength(s) {}

    void BuildFootprint(Plane *plane);
    void ApplyToPlane(Plane *plane);
    void RemoveFromPlane(Plane *plane);

    void SetEnabled(bool on) { enabled = on; }
};
