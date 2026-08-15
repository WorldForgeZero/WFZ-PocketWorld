#include "world.h"

Chunk* World::GetChunk(int32_t globalX, int32_t globalY)
{
    int32_t chunkX = TileToChunkCoord(globalX);
    int32_t chunkY = TileToChunkCoord(globalY);
    return GetChunkByChunkCoords(chunkX, chunkY);
}

Chunk& World::GetOrCreateChunk(int32_t globalX, int32_t globalY)
{
    int32_t chunkX = TileToChunkCoord(globalX);
    int32_t chunkY = TileToChunkCoord(globalY);
    return GetOrCreateChunkByChunkCoords(chunkX, chunkY);
}

void World::RemoveChunk(int32_t globalX, int32_t globalY)
{
    int32_t chunkX = TileToChunkCoord(globalX);
    int32_t chunkY = TileToChunkCoord(globalY);
    RemoveChunkByChunkCoords(chunkX, chunkY);
}

Chunk* World::GetChunkByChunkCoords(int32_t chunkX, int32_t chunkY)
{
    auto it = chunks_.find({chunkX, chunkY});
    return (it != chunks_.end()) ? it->second.get() : nullptr;
}

Chunk& World::GetOrCreateChunkByChunkCoords(int32_t chunkX, int32_t chunkY)
{
    auto it = chunks_.find({chunkX, chunkY});
    if (it != chunks_.end())
        return *it->second;

    auto chunk = std::make_unique<Chunk>(chunkX, chunkY);
    Chunk* ptr = chunk.get();
    chunks_[{chunkX, chunkY}] = std::move(chunk);
    return *ptr;
}

void World::RemoveChunkByChunkCoords(int32_t chunkX, int32_t chunkY)
{
    chunks_.erase({chunkX, chunkY});
}
