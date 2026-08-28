#pragma once

#include <cstdint>
#include <vector>

#include "game/entity/entity.h"
#include "game/entity_manager/entity_manager.h"
#include "game/zone/zone_manager.h"

class World
{
public:
    World()
    {
        entityManager_.SetWorld(this);
    }

    ~World() = default;

    World(const World &) = delete;
    World &operator=(const World &) = delete;

    EntityManager &GetEntityManager() { return entityManager_; }
    const EntityManager &GetEntityManager() const { return entityManager_; }
    ZoneManager &GetZoneManager() { return zoneManager_; }
    const ZoneManager &GetZoneManager() const { return zoneManager_; }

    void Tick(double dt)
    {
        entityManager_.UpdateMovement(dt);

        zoneManager_.ProcessPendingChanges(*this);

        entityManager_.FlushEmptyChunks();
    }

    void NotifyFloorChanged(Coordinate coord, bool hasFloor)
    {
        zoneManager_.OnFloorChanged(coord, hasFloor);
    }

    void NotifyZoneBlockerChanged(Coordinate coord)
    {
        zoneManager_.OnSolidChanged(coord);
    }

    static int32_t TileToChunkCoord(int32_t tileCoord)
    {
        int32_t result = tileCoord / CHUNK_SIZE;
        if (tileCoord % CHUNK_SIZE != 0 && tileCoord < 0)
            --result;
        return result;
    }

private:
    EntityManager entityManager_;
    ZoneManager zoneManager_;
};
