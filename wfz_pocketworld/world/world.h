#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "chunk.h"
#include "constants.h"

struct ChunkKey
{
    int32_t x;
    int32_t y;

    bool operator==(const ChunkKey &other) const
    {
        return x == other.x && y == other.y;
    }
};

namespace std
{
    template <>
    struct hash<ChunkKey>
    {
        size_t operator()(const ChunkKey &k) const noexcept
        {
            return std::hash<int32_t>()(k.x) ^ (std::hash<int32_t>()(k.y) << 1);
        }
    };
}

class World
{

private:
    std::unordered_map<ChunkKey, std::unique_ptr<Chunk>> chunks_;

public:
    World() = default;
    ~World() = default;

    World(const World &) = delete;
    World &operator=(const World &) = delete;

    World(World &&) noexcept = default;
    World &operator=(World &&) noexcept = default;

    Chunk *GetChunk(int32_t globalX, int32_t globalY);
    Chunk &GetOrCreateChunk(int32_t globalX, int32_t globalY);
    void RemoveChunk(int32_t globalX, int32_t globalY);

private:
    Chunk *GetChunkByChunkCoords(int32_t chunkX, int32_t chunkY);
    Chunk &GetOrCreateChunkByChunkCoords(int32_t chunkX, int32_t chunkY);
    void RemoveChunkByChunkCoords(int32_t chunkX, int32_t chunkY);

    static int32_t TileToChunkCoord(int32_t tileCoord)
    {
        int32_t result = tileCoord / CHUNK_SIZE;
        if (tileCoord % CHUNK_SIZE != 0 && tileCoord < 0)
            --result;

        return result;
    }
};
