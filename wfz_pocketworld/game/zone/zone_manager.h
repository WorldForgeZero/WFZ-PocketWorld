#pragma once

#include <cstdint>
#include <queue>
#include <unordered_map>
#include <vector>

#include "coordinate.h"

class World;

struct Zone
{
    uint32_t id;
    std::vector<Coordinate> tiles;
    std::vector<Coordinate> borderTiles;
};

class ZoneManager
{
public:
    void OnFloorChanged(Coordinate coord, bool hasFloor);
    void OnSolidChanged(Coordinate coord);

    void ProcessPendingChanges(World &world);

    const Zone *GetZone(uint32_t zoneId) const;
    uint32_t GetZoneIdAt(Coordinate coord) const;
    const std::unordered_map<uint32_t, Zone> &GetAllZones() const { return zones_; }

private:
    enum class EventType
    {
        FloorAdded,
        FloorRemoved,
        BlockerChanged
    };

    struct Event
    {
        EventType type;
        Coordinate coord;
    };

    bool IsFloor(World &world, Coordinate coord) const;
    bool HasBlocker(World &world, Coordinate coord) const;
    void UpdateBorderForZone(World &world, uint32_t zoneId);
    void AddFloor(World &world, Coordinate coord);
    void RemoveFloor(World &world, Coordinate coord);

    std::queue<Event> pendingEvents_;
    std::unordered_map<uint32_t, Zone> zones_;
    std::unordered_map<Coordinate, uint32_t> tileToZone_;
    uint32_t nextZoneId_ = 1;
};
