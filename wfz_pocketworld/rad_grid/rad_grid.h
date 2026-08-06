#pragma once

#include <stdint.h>
#include <stdlib.h>

class RadGrid
{
private:
    uint16_t *level_;
    uint16_t ambient_;

    uint8_t active_cells_;

public:
    RadGrid(uint16_t ambient);
    ~RadGrid();

    uint16_t GetAmbient() const noexcept;
    void SetAmbient(uint16_t ambient) noexcept;
};
