import random

import wfz_pocketworld._core as core


def get_tile_char(world: core.World, x, y):
    chunk = world.GetChunk(x, y)
    if chunk is None:
        return " "

    local_x = x - (chunk.GetX() << 4)
    local_y = y - (chunk.GetY() << 4)

    tile = chunk.GetTile(local_x, local_y)
    entities = tile.GetOccupyingEntities()

    for entity in entities:
        if entity.HasFlag(core.ENTITY_SOLID):
            return "#"

        elif entity.HasFlag(core.ENTITY_FLOOR):
            return "."

        else:
            return "o"

    return " "


def visualize(world, min_x, max_x, min_y, max_y):
    print("-" * (max_x - min_x + 3))
    for y in range(max_y, min_y - 1, -1):
        row = "|"
        for x in range(min_x, max_x + 1):
            row += get_tile_char(world, x, y)

        row += "|"
        print(row)

    print("-" * (max_x - min_x + 3))


def main():
    world = core.World()

    min_x, max_x = -5, 5
    min_y, max_y = -5, 5

    print("Создаём случайное наполнение...")
    for x in range(min_x, max_x + 1):
        for y in range(min_y, max_y + 1):
            coord = core.Coordinate(x, y)
            roll = random.random()
            if roll < 0.7:
                world.SpawnEntity(100, core.ENTITY_FLOOR, coord)

            elif roll < 0.9:
                world.SpawnEntity(200, core.ENTITY_SOLID, coord)

    print("Создаём стены...")
    wall_flags = core.ENTITY_SOLID
    for x in range(min_x, max_x + 1):
        world.SpawnEntity(200, wall_flags, x, max_y)
        world.SpawnEntity(200, wall_flags, x, min_y)

    for y in range(min_y + 1, max_y):
        world.SpawnEntity(200, wall_flags, min_x, y)
        world.SpawnEntity(200, wall_flags, max_x, y)

    print("Карта:")
    visualize(world, min_x, max_x, min_y, max_y)

    print("Готово.")


if __name__ == "__main__":
    main()
