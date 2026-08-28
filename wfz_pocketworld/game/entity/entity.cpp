#include "entity.h"
#include "constants.h"

Entity::Entity(EntityUid uid, uint32_t type, uint32_t flags, Coordinate anchor, uint8_t rotation, const EntityShape *footprint)
    : uid(uid), id(0), movingIndex(0),

      type(type), flags(flags),

      res(),
      rotation(rotation),

      velX(0.0), velY(0.0),

      anchor(anchor),
      pos(anchor.x * VECTOR2D_FIXED_SCALE, anchor.y * VECTOR2D_FIXED_SCALE),

      footprint(footprint),

      chunk(nullptr)
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
        result.push_back({anchor.x, anchor.y});
    }
    return result;
}

bool Entity::HasFlag(uint32_t flag) const { return (flags & flag) != 0; }
void Entity::AddFlag(uint32_t flag) { flags |= flag; }
void Entity::RemoveFlag(uint32_t flag) { flags &= ~flag; }
