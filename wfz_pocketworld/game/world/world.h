#pragma once

#include <cstdint>
#include <limits>

#include "game/entity_manager/entity_manager.h"
#include "game/zone/zone_manager.h"
#include "utils/coordinate.h"

struct WorldBounds
{
    int32_t minX = std::numeric_limits<int32_t>::min();
    int32_t maxX = std::numeric_limits<int32_t>::max();
    int32_t minY = std::numeric_limits<int32_t>::min();
    int32_t maxY = std::numeric_limits<int32_t>::max();
};

namespace WorldFlags
{
    constexpr uint8_t WrapX = 1u << 0;
    constexpr uint8_t WrapY = 1u << 1;
    constexpr uint8_t SolidX = 1u << 2;
    constexpr uint8_t SolidY = 1u << 3;
}

class World
{
public:
    World();
    explicit World(WorldBounds bounds, uint8_t flags = 0);

    ~World() = default;

    World(const World &) = delete;
    World &operator=(const World &) = delete;

    EntityManager &GetEntityManager() { return entityManager_; }
    const EntityManager &GetEntityManager() const { return entityManager_; }

    ZoneManager &GetZoneManager() { return zoneManager_; }
    const ZoneManager &GetZoneManager() const { return zoneManager_; }

    const WorldBounds &GetBounds() const { return bounds_; }

    bool HasFlag(uint8_t flag) const
    {
        return (flags_ & flag) != 0;
    }

    bool IsInside(Coordinate coord) const;
    bool NormalizeCoordinate(Coordinate &coord) const;

    bool GetNeighbor(Coordinate origin, int32_t deltaX, int32_t deltaY, Coordinate &neighbor) const;

    void Tick(double dt);

    void NotifyFloorChanged(Coordinate coord, bool hasFloor);
    void NotifyZoneBlockerChanged(Coordinate coord);

    static int32_t TileToChunkCoord(int32_t tileCoord);

private:
    static bool NormalizeAxis(int64_t &value, int32_t min, int32_t max, bool wrap);

    WorldBounds bounds_;
    uint8_t flags_ = 0;

    EntityManager entityManager_;
    ZoneManager zoneManager_;
};
