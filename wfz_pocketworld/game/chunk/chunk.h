#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#include "constants.h"
#include "entity.h"
#include "tile.h"

struct ChunkKey
{
    int32_t x, y;
    bool operator==(const ChunkKey &o) const { return x == o.x && y == o.y; }
};

namespace std
{
    template <>
    struct hash<ChunkKey>
    {
        size_t operator()(const ChunkKey &k) const noexcept
        {
            return std::hash<uint64_t>{}((static_cast<uint64_t>(static_cast<uint32_t>(k.x)) << 32) | static_cast<uint32_t>(k.y));
        }
    };
}

class Chunk
{
private:
    int32_t chunkX_, chunkY_;
    std::array<Tile, CHUNK_SIZE * CHUNK_SIZE> tiles_;
    std::vector<std::unique_ptr<Entity>> entities_;

public:
    Chunk(int32_t chunkX, int32_t chunkY);
    ~Chunk();

    Chunk(const Chunk &) = delete;
    Chunk &operator=(const Chunk &) = delete;

    Chunk(Chunk &&) noexcept = default;
    Chunk &operator=(Chunk &&) noexcept = default;

    Tile &GetTile(uint32_t localX, uint32_t localY);

    void AddEntity(std::unique_ptr<Entity> entity);
    std::unique_ptr<Entity> RemoveEntity(Entity *entity);
    const std::vector<std::unique_ptr<Entity>> &GetEntities() const;

    int32_t GetX() const { return chunkX_; }
    int32_t GetY() const { return chunkY_; }
};
