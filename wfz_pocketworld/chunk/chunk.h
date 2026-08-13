#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#include "constants.h"
#include "coordinate.h"
#include "entity.h"
#include "tile.h"

class Chunk
{
private:
    Coordinate key_;

    std::array<Tile, CHUNK_SIZE * CHUNK_SIZE> tiles_;

    std::vector<std::unique_ptr<Entity>> entities_;

public:
    Chunk(uint32_t chunkX, uint32_t chunkY);
    ~Chunk();

    Chunk(const Chunk &) = delete;
    Chunk &operator=(const Chunk &) = delete;

    Chunk(Chunk &&) noexcept = default;
    Chunk &operator=(Chunk &&) noexcept = default;

    Tile &GetTile(uint32_t localX, uint32_t localY);

    void AddEntity(std::unique_ptr<Entity> entity);
    std::unique_ptr<Entity> RemoveEntity(Entity *entity);
    const std::vector<std::unique_ptr<Entity>> &GetEntities() const;

    uint32_t GetX() const { return key_.x; }
    uint32_t GetY() const { return key_.y; }
};
