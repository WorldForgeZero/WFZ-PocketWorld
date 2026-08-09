#include "entity.h"
#include "coordinate.h"
#include "resistance.h"
#include "vector_2d.h"

Entity::Entity(
    uint32_t type,
    uint32_t flags,
    uint8_t rotation,
    uint8_t height,
    uint8_t width,
    Resistance res)
    : id(0),
      type_(type),
      flags_(flags),
      coord_{0, 0},
      rotation(rotation),
      height(height),
      width(width),
      vel{0, 0},
      res(res)
{
}

uint32_t Entity::GetType() const { return type_; }
uint32_t Entity::GetFlags() const { return flags_; }
bool Entity::HasFlag(uint32_t flag) const { return (flags_ & flag) != 0; }
void Entity::AddFlag(uint32_t flag) { flags_ |= flag; }
void Entity::RemoveFlag(uint32_t flag) { flags_ &= ~flag; }

Entity *Entity::GetCellNext() const { return cell_next_; }
void Entity::SetCellNext(Entity *next) { cell_next_ = next; }
