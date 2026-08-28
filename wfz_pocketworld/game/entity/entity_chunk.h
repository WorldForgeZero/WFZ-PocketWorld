#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#include "game/entity/entity.h"
#include "game/entity/tile.h"
#include "utils/constants.h"

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

class EntityChunk
{
public:
    EntityChunk(int32_t chunkX, int32_t chunkY);
    ~EntityChunk();

    EntityChunk(const EntityChunk &) = delete;
    EntityChunk &operator=(const EntityChunk &) = delete;

    EntityChunk(EntityChunk &&) noexcept = default;
    EntityChunk &operator=(EntityChunk &&) noexcept = default;

    Tile &GetTile(uint32_t localX, uint32_t localY);
    const Tile &GetTile(uint32_t localX, uint32_t localY) const;

    void AddEntity(std::unique_ptr<Entity> entity);
    std::unique_ptr<Entity> RemoveEntity(Entity *entity);
    const std::vector<std::unique_ptr<Entity>> &GetEntities() const;

    int32_t GetX() const { return chunkX_; }
    int32_t GetY() const { return chunkY_; }

    bool IsEmpty() const;

private:
    int32_t chunkX_, chunkY_;
    std::array<Tile, CHUNK_SIZE * CHUNK_SIZE> tiles_;
    std::vector<std::unique_ptr<Entity>> entities_;
};
