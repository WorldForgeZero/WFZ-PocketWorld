#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <vector>

#include "light_source.h"

class LightGrid
{
private:
    uint16_t *level_;
    uint16_t ambient_;

    uint8_t active_cells_;
    std::vector<LightSource> rad_sources_;

public:
    LightGrid(uint16_t ambient);
    ~LightGrid();

    uint16_t GetAmbient() const noexcept;
    void SetAmbient(uint16_t ambient) noexcept;
};
