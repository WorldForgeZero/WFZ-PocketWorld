from PIL import Image, ImageDraw, ImageFont

import wfz_pocketworld._core as core

FLOOR = core.ENTITY_FLOOR
SOLID = core.ENTITY_SOLID
EPHEMERAL = core.ENTITY_EPHEMERAL
FIXED_SCALE = 4096

WORLD_MIN_X, WORLD_MAX_X = -15, 10
WORLD_MIN_Y, WORLD_MAX_Y = -8, 8

CELL_SIZE = 20
WIDTH = (WORLD_MAX_X - WORLD_MIN_X + 1) * CELL_SIZE
HEIGHT = (WORLD_MAX_Y - WORLD_MIN_Y + 1) * CELL_SIZE

WALL_X = 0


def tile_to_pixel(tile_x, tile_y):
    px = (tile_x - WORLD_MIN_X) * CELL_SIZE
    py = (WORLD_MAX_Y - tile_y) * CELL_SIZE
    return px, py


def draw_frame(world, moving_entities, tick):
    img = Image.new("RGB", (WIDTH, HEIGHT), (30, 30, 30))
    draw = ImageDraw.Draw(img)

    for x in range(WORLD_MIN_X, WORLD_MAX_X + 1):
        for y in range(WORLD_MIN_Y, WORLD_MAX_Y + 1):
            px, py = tile_to_pixel(x, y)
            draw.rectangle(
                [px, py, px + CELL_SIZE - 1, py + CELL_SIZE - 1],
                fill=(70, 70, 70),
                outline=(50, 50, 50),
            )

    for y in range(WORLD_MIN_Y, WORLD_MAX_Y + 1):
        px, py = tile_to_pixel(WALL_X, y)
        draw.rectangle(
            [px, py, px + CELL_SIZE - 1, py + CELL_SIZE - 1],
            fill=(200, 50, 50),
            outline=(120, 20, 20),
        )

    for entity_id, color in moving_entities:
        entity = world.GetEntity(entity_id)
        if entity is None:
            continue
        tile_x = entity.anchor.x
        tile_y = entity.anchor.y
        px, py = tile_to_pixel(tile_x, tile_y)
        pad = 4
        draw.rectangle(
            [px + pad, py + pad, px + CELL_SIZE - 1 - pad, py + CELL_SIZE - 1 - pad],
            fill=color,
            outline=(255, 255, 255),
            width=2,
        )

    font = ImageFont.load_default()
    draw.text((4, 4), f"Tick: {tick}", fill=(255, 255, 255), font=font)

    return img


def main():
    world = core.World()

    for x in range(WORLD_MIN_X, WORLD_MAX_X + 1):
        for y in range(WORLD_MIN_Y, WORLD_MAX_Y + 1):
            world.SpawnEntity(100, FLOOR, x, y)

    for y in range(WORLD_MIN_Y, WORLD_MAX_Y + 1):
        world.SpawnEntity(200, SOLID, WALL_X, y)

    moving_entities = []

    speeds = [
        (1, (0, 200, 0)),  # зелёный 1 тайл/с
        (3, (0, 150, 255)),  # синий 3 тайла/с
        (8, (255, 100, 0)),  # оранжевый 8 тайлов/с
    ]
    start_y_positions = [-3, 0, 3]

    for i, (speed, color) in enumerate(speeds):
        id = world.SpawnEntity(300, 0, -12, start_y_positions[i])
        world.SetVelocity(id, speed, 0)
        moving_entities.append((id, color))

    ghost_id = world.SpawnEntity(400, EPHEMERAL, -12, 6)
    world.SetVelocity(ghost_id, 4, 0)
    moving_entities.append((ghost_id, (255, 255, 0)))

    frames = []
    dt = 0.05  # 20 тиков в секунду
    total_ticks = 240  # 12 секунд

    for tick in range(total_ticks):
        world.Tick(dt)
        frames.append(draw_frame(world, moving_entities, tick))

    if frames:
        frames[0].save(
            "simulation.gif",
            save_all=True,
            append_images=frames[1:],
            duration=50,  # 50 мс = 20 fps
            loop=0,
        )
        print("GIF сохранён: simulation.gif")
    else:
        print("Нет кадров")


if __name__ == "__main__":
    main()
