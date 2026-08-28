#include "game/zone/zone_manager.h"

#include <algorithm>
#include <cstdint>
#include <queue>
#include <unordered_set>

#include "game/entity/entity.h"
#include "game/entity/entity_chunk.h"
#include "game/entity/tile.h"
#include "game/world/world.h"
#include "utils/constants.h"

namespace
{
    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};
}

bool ZoneManager::IsFloor(World &world, Coordinate coord) const
{
    int32_t lx, ly;
    EntityChunk *chunk = world.GetEntityManager().GetChunkAndLocalCoords(coord.x, coord.y, lx, ly);
    if (!chunk)
        return false;

    return chunk->GetTile(lx, ly).floorType != 0;
}

bool ZoneManager::HasBlocker(World &world, Coordinate coord) const
{
    int32_t lx, ly;
    EntityChunk *chunk = world.GetEntityManager().GetChunkAndLocalCoords(coord.x, coord.y, lx, ly);
    if (!chunk)
        return false;

    Tile &tile = chunk->GetTile(lx, ly);
    for (Entity *e : tile.occupyingEntities)
        if (e->HasFlag(EntityFlags::ZONE_BLOCKER))
            return true;

    return false;
}

void ZoneManager::UpdateBorderForZone(World &world, uint32_t zoneId)
{
    auto it = zones_.find(zoneId);
    if (it == zones_.end())
        return;

    Zone &zone = it->second;
    zone.borderTiles.clear();

    for (const Coordinate &coord : zone.tiles)
    {
        if (HasBlocker(world, coord))
            continue;

        bool isBorder = false;
        for (int i = 0; i < 4; ++i)
        {
            Coordinate neigh{coord.x + dx[i], coord.y + dy[i]};
            int32_t nlX, nlY;
            EntityChunk *neighChunk = world.GetEntityManager().GetChunkAndLocalCoords(neigh.x, neigh.y, nlX, nlY);
            if (!neighChunk)
            {
                isBorder = true;
                break;
            }

            Tile &nt = neighChunk->GetTile(nlX, nlY);
            if (nt.floorType == 0)
            {
                isBorder = true;
                break;
            }
        }

        if (isBorder)
            zone.borderTiles.push_back(coord);
    }
}

void ZoneManager::OnFloorChanged(Coordinate coord, bool hasFloor)
{
    pendingEvents_.push({hasFloor ? EventType::FloorAdded : EventType::FloorRemoved, coord});
}

void ZoneManager::OnSolidChanged(Coordinate coord)
{
    pendingEvents_.push({EventType::BlockerChanged, coord});
}

void ZoneManager::ProcessPendingChanges(World &world)
{
    while (!pendingEvents_.empty())
    {
        Event e = pendingEvents_.front();
        pendingEvents_.pop();

        switch (e.type)
        {
        case EventType::FloorAdded:
            AddFloor(world, e.coord);
            break;

        case EventType::FloorRemoved:
            RemoveFloor(world, e.coord);
            break;

        case EventType::BlockerChanged:
        {
            auto it = tileToZone_.find(e.coord);
            if (it != tileToZone_.end())
                UpdateBorderForZone(world, it->second);

            break;
        }
        }
    }
}

void ZoneManager::AddFloor(World &world, Coordinate coord)
{
    if (tileToZone_.find(coord) != tileToZone_.end())
        return;

    std::unordered_set<uint32_t> neighborZones;
    for (int i = 0; i < 4; ++i)
    {
        Coordinate nb{coord.x + dx[i], coord.y + dy[i]};
        auto it = tileToZone_.find(nb);
        if (it != tileToZone_.end())
            neighborZones.insert(it->second);
    }

    if (neighborZones.empty())
    {
        uint32_t newId = nextZoneId_++;
        Zone zone;
        zone.id = newId;
        zone.tiles.push_back(coord);
        zones_[newId] = std::move(zone);
        tileToZone_[coord] = newId;
    }
    else if (neighborZones.size() == 1)
    {
        uint32_t zoneId = *neighborZones.begin();
        zones_[zoneId].tiles.push_back(coord);
        tileToZone_[coord] = zoneId;
    }
    else
    {
        uint32_t mainId = *neighborZones.begin();
        Zone &mainZone = zones_[mainId];

        for (uint32_t otherId : neighborZones)
        {
            if (otherId == mainId)
                continue;

            Zone &otherZone = zones_[otherId];
            for (const Coordinate &t : otherZone.tiles)
            {
                tileToZone_[t] = mainId;
                mainZone.tiles.push_back(t);
            }
            zones_.erase(otherId);
        }

        mainZone.tiles.push_back(coord);
        tileToZone_[coord] = mainId;
    }

    std::unordered_set<uint32_t> zonesToUpdate = neighborZones;
    if (!neighborZones.empty())
    {
        uint32_t mainId = *neighborZones.begin();
        zonesToUpdate.insert(mainId);
    }

    for (uint32_t id : zonesToUpdate)
    {
        if (zones_.find(id) != zones_.end())
            UpdateBorderForZone(world, id);
    }
}

void ZoneManager::RemoveFloor(World &world, Coordinate coord)
{
    auto it = tileToZone_.find(coord);
    if (it == tileToZone_.end())
        return;

    uint32_t oldZoneId = it->second;
    Zone &zone = zones_[oldZoneId];

    zone.tiles.erase(std::remove(zone.tiles.begin(), zone.tiles.end(), coord), zone.tiles.end());
    tileToZone_.erase(coord);

    if (zone.tiles.empty())
    {
        zones_.erase(oldZoneId);
        return;
    }

    std::vector<Coordinate> firstComponent;
    std::queue<Coordinate> q;
    std::unordered_set<Coordinate> visited;

    Coordinate start = zone.tiles.front();
    q.push(start);
    visited.insert(start);
    firstComponent.push_back(start);

    while (!q.empty())
    {
        Coordinate cur = q.front();
        q.pop();

        for (int i = 0; i < 4; ++i)
        {
            Coordinate nb{cur.x + dx[i], cur.y + dy[i]};
            auto nbIt = tileToZone_.find(nb);
            if (nbIt != tileToZone_.end() && nbIt->second == oldZoneId && visited.find(nb) == visited.end())
            {
                visited.insert(nb);
                q.push(nb);
                firstComponent.push_back(nb);
            }
        }
    }

    if (firstComponent.size() == zone.tiles.size())
    {
        UpdateBorderForZone(world, oldZoneId);
        return;
    }

    std::vector<Coordinate> oldTiles = zone.tiles;
    zone.tiles = firstComponent;

    std::unordered_set<Coordinate> remaining;
    for (const Coordinate &t : oldTiles)
        if (visited.find(t) == visited.end())
            remaining.insert(t);

    std::vector<uint32_t> newZoneIds;
    while (!remaining.empty())
    {
        Coordinate newStart = *remaining.begin();
        std::vector<Coordinate> component;
        std::queue<Coordinate> q2;
        q2.push(newStart);
        remaining.erase(newStart);
        component.push_back(newStart);

        while (!q2.empty())
        {
            Coordinate cur = q2.front();
            q2.pop();

            for (int i = 0; i < 4; ++i)
            {
                Coordinate nb{cur.x + dx[i], cur.y + dy[i]};
                if (remaining.find(nb) != remaining.end())
                {
                    remaining.erase(nb);
                    q2.push(nb);
                    component.push_back(nb);
                }
            }
        }

        uint32_t newId = nextZoneId_++;
        Zone newZone;
        newZone.id = newId;
        newZone.tiles = std::move(component);
        for (const Coordinate &t : newZone.tiles)
            tileToZone_[t] = newId;

        zones_[newId] = std::move(newZone);
        newZoneIds.push_back(newId);
    }

    UpdateBorderForZone(world, oldZoneId);
    for (uint32_t id : newZoneIds)
        UpdateBorderForZone(world, id);
}

const Zone *ZoneManager::GetZone(uint32_t zoneId) const
{
    auto it = zones_.find(zoneId);
    return it != zones_.end() ? &it->second : nullptr;
}

uint32_t ZoneManager::GetZoneIdAt(Coordinate coord) const
{
    auto it = tileToZone_.find(coord);
    return it != tileToZone_.end() ? it->second : 0;
}
