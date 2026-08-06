#include <stdint.h>
#include <stdlib.h>

#include "makro.h"
#include "rad_grid.h"

RadGrid::RadGrid(uint16_t ambient)
    : level_(nullptr),
      ambient_(ambient),
      active_cells_(0)
{
    level_ = (uint16_t *)CALLOC_GRID(sizeof(uint16_t));
    // FIXME: По хорошему тут бы сказать если у нас level_ умер, но как-то похуй.
}

RadGrid::~RadGrid()
{
    free(level_);
}

uint16_t RadGrid::GetAmbient() const noexcept
{
    return ambient_;
}

void RadGrid::SetAmbient(uint16_t ambient) noexcept
{
    ambient_ = ambient;
}
