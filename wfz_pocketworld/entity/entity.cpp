#include "entity.h"

Entity::Entity(uint32_t id, uint32_t type, uint32_t flags,
               Coordinate anchor, uint8_t rotation,
               uint8_t width, uint8_t height,
               const EntityShape *footprint)
    : id(id), type(type), flags(flags),
      anchor(anchor), rotation(rotation),
      vel(0, 0), width(width), height(height),
      footprint(footprint)
{
}

std::vector<Coordinate> Entity::GetOccupiedTiles() const
{
    std::vector<Coordinate> result;
    if (footprint)
    {
        const auto &offsets = footprint->offsets[rotation];
        result.reserve(offsets.size());
        for (const auto &off : offsets)
        {
            result.push_back({anchor.x + off.x, anchor.y + off.y});
        }
    }
    else
    {
        int w = width;
        int h = height;
        if (rotation == 1 || rotation == 3)
            std::swap(w, h);

        result.reserve(w * h);
        for (int dx = 0; dx < w; ++dx)
        {
            for (int dy = 0; dy < h; ++dy)
            {
                int ox = dx, oy = dy;
                switch (rotation)
                {
                case 0:
                    ox = dx;
                    oy = dy;
                    break;

                case 1:
                    ox = dy;
                    oy = -dx;
                    break;

                case 2:
                    ox = -dx;
                    oy = -dy;
                    break;

                case 3:
                    ox = -dy;
                    oy = dx;
                    break;
                }
                result.push_back({anchor.x + ox, anchor.y + oy});
            }
        }
    }
    return result;
}

bool Entity::HasFlag(uint32_t flag) const { return (flags & flag) != 0; }
void Entity::AddFlag(uint32_t flag) { flags |= flag; }
void Entity::RemoveFlag(uint32_t flag) { flags &= ~flag; }
