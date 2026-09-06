#include "game/world/world.h"

#include <cassert>
#include <stdexcept>

#include "utils/constants.h"

World::World()
    : World(WorldBounds{}, 0)
{
}

World::World(WorldBounds bounds, uint8_t flags)
    : bounds_(bounds),
      flags_(flags)
{
    if (bounds_.minX > bounds_.maxX ||
        bounds_.minY > bounds_.maxY)
    {
        throw std::invalid_argument("World bounds must not be empty");
    }

    if ((HasFlag(WorldFlags::WrapX) && HasFlag(WorldFlags::SolidX)) ||
        (HasFlag(WorldFlags::WrapY) && HasFlag(WorldFlags::SolidY)))
    {
        throw std::invalid_argument(
            "Wrap and solid flags are mutually exclusive on the same axis");
    }

    entityManager_.SetWorld(this);
}

bool World::IsInside(Coordinate coord) const
{
    return coord.x >= bounds_.minX && coord.x <= bounds_.maxX &&
           coord.y >= bounds_.minY && coord.y <= bounds_.maxY;
}

bool World::NormalizeCoordinate(Coordinate &coord) const
{
    int64_t x = coord.x;
    int64_t y = coord.y;

    if (!NormalizeAxis(x, bounds_.minX, bounds_.maxX, HasFlag(WorldFlags::WrapX)))
        return false;

    if (!NormalizeAxis(y, bounds_.minY, bounds_.maxY, HasFlag(WorldFlags::WrapY)))
        return false;

    coord.x = static_cast<int32_t>(x);
    coord.y = static_cast<int32_t>(y);

    return true;
}

bool World::GetNeighbor(Coordinate origin, int32_t deltaX, int32_t deltaY, Coordinate &neighbor) const
{
    assert((deltaX == 0 && (deltaY == -1 || deltaY == 1)) ||
           (deltaY == 0 && (deltaX == -1 || deltaX == 1)));

    if (!NormalizeCoordinate(origin))
        return false;

    neighbor = origin;

    if (deltaX != 0)
    {
        if (deltaX < 0 && origin.x == bounds_.minX)
        {
            if (!HasFlag(WorldFlags::WrapX))
                return false;

            neighbor.x = bounds_.maxX;
        }
        else if (deltaX > 0 && origin.x == bounds_.maxX)
        {
            if (!HasFlag(WorldFlags::WrapX))
                return false;

            neighbor.x = bounds_.minX;
        }
        else
        {
            neighbor.x += deltaX;
        }

        return true;
    }

    if (deltaY < 0 && origin.y == bounds_.minY)
    {
        if (!HasFlag(WorldFlags::WrapY))
            return false;

        neighbor.y = bounds_.maxY;
    }
    else if (deltaY > 0 && origin.y == bounds_.maxY)
    {
        if (!HasFlag(WorldFlags::WrapY))
            return false;

        neighbor.y = bounds_.minY;
    }
    else
    {
        neighbor.y += deltaY;
    }

    return true;
}

bool World::NormalizeAxis(int64_t &value, int32_t min, int32_t max, bool wrap)
{
    if (value >= min && value <= max)
        return true;

    if (!wrap)
        return false;

    const int64_t width = static_cast<int64_t>(max) - static_cast<int64_t>(min) + 1;

    int64_t offset = (value - static_cast<int64_t>(min)) % width;

    if (offset < 0)
        offset += width;

    value = static_cast<int64_t>(min) + offset;

    return true;
}

void World::Tick(double dt)
{
    entityManager_.UpdateMovement(dt);

    zoneManager_.ProcessPendingChanges(*this);

    entityManager_.FlushEmptyChunks();
}

void World::NotifyFloorChanged(Coordinate coord, bool hasFloor)
{
    zoneManager_.OnFloorChanged(coord, hasFloor);
}

void World::NotifyZoneBlockerChanged(Coordinate coord)
{
    zoneManager_.OnSolidChanged(coord);
}

int32_t World::TileToChunkCoord(int32_t tileCoord)
{
    int32_t result = tileCoord / CHUNK_SIZE;

    if (tileCoord % CHUNK_SIZE != 0 && tileCoord < 0)
        --result;

    return result;
}
