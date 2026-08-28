#include "chunk.h"

Chunk::Chunk(int32_t chunkX, int32_t chunkY)
    : chunkX_(chunkX), chunkY_(chunkY)
{
    for (uint32_t iy = 0; iy < CHUNK_SIZE; ++iy)
    {
        for (uint32_t ix = 0; ix < CHUNK_SIZE; ++ix)
        {
            tiles_[iy * CHUNK_SIZE + ix].localX = static_cast<uint8_t>(ix);
            tiles_[iy * CHUNK_SIZE + ix].localY = static_cast<uint8_t>(iy);
        }
    }
}

Chunk::~Chunk() = default;

Tile &Chunk::GetTile(uint32_t localX, uint32_t localY)
{
    return tiles_[localY * CHUNK_SIZE + localX];
}

void Chunk::AddEntity(std::unique_ptr<Entity> entity)
{
    entities_.push_back(std::move(entity));
}

std::unique_ptr<Entity> Chunk::RemoveEntity(Entity *entity)
{
    for (auto it = entities_.begin(); it != entities_.end(); ++it)
    {
        if (it->get() == entity)
        {
            auto ptr = std::move(*it);
            entities_.erase(it);
            return ptr;
        }
    }
    return nullptr;
}

const std::vector<std::unique_ptr<Entity>> &Chunk::GetEntities() const
{
    return entities_;
}
