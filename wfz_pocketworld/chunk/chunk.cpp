#include "chunk.h"
#include "entity.h"

Chunk::Chunk(uint32_t chunkX, uint32_t chunkY)
    : key_{chunkX, chunkY}
{
    uint32_t baseX = chunkX * CHUNK_SIZE;
    uint32_t baseY = chunkY * CHUNK_SIZE;

    for (uint32_t iy = 0; iy < CHUNK_SIZE; ++iy)
    {
        for (uint32_t ix = 0; ix < CHUNK_SIZE; ++ix)
        {
            tiles_[iy * CHUNK_SIZE + ix].coord = {baseX + ix, baseY + iy};
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
