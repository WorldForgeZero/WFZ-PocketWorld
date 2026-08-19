import random
import statistics
import time

import wfz_pocketworld._core as core

FLOOR = core.ENTITY_FLOOR
SOLID = core.ENTITY_SOLID
EPHEMERAL = core.ENTITY_EPHEMERAL


WORLD_HALF_X = 500
WORLD_HALF_Y = 500
RANDOM_WALL_CHANCE = 0.05
MOVING_COUNT = 2000
ITEM_COUNT = 5000
SEARCH_PER_TICK = 10
RAYCAST_PER_TICK = 10
SPAWN_PER_TICK = 5
REMOVE_PER_TICK = 5
TICKS = 200
DT = 0.05


def build_world():
    """Создаёт плотный мир и возвращает (world, moving_ids, static_ids)."""
    world = core.World()

    for x in range(-WORLD_HALF_X, WORLD_HALF_X):
        for y in range(-WORLD_HALF_Y, WORLD_HALF_Y):
            world.SpawnEntity(100, FLOOR, x, y)

    for x in range(-WORLD_HALF_X, WORLD_HALF_X):
        world.SpawnEntity(200, SOLID, x, WORLD_HALF_Y - 1)
        world.SpawnEntity(200, SOLID, x, -WORLD_HALF_Y)
    for y in range(-WORLD_HALF_Y, WORLD_HALF_Y):
        world.SpawnEntity(200, SOLID, WORLD_HALF_X - 1, y)
        world.SpawnEntity(200, SOLID, -WORLD_HALF_X, y)

    for x in range(-WORLD_HALF_X + 1, WORLD_HALF_X - 1):
        for y in range(-WORLD_HALF_Y + 1, WORLD_HALF_Y - 1):
            if random.random() < RANDOM_WALL_CHANCE:
                world.SpawnEntity(200, SOLID, x, y)

    moving_ids = []
    for i in range(MOVING_COUNT):
        x = random.randint(-WORLD_HALF_X + 1, WORLD_HALF_X - 2)
        y = random.randint(-WORLD_HALF_Y + 1, WORLD_HALF_Y - 2)
        eid = world.SpawnEntity(300, 0, x, y)

        vx = random.uniform(-2.0, 2.0)
        vy = random.uniform(-2.0, 2.0)
        world.SetVelocity(eid, vx, vy)
        moving_ids.append(eid)

    static_ids = []
    for i in range(ITEM_COUNT):
        x = random.randint(-WORLD_HALF_X + 1, WORLD_HALF_X - 2)
        y = random.randint(-WORLD_HALF_Y + 1, WORLD_HALF_Y - 2)
        eid = world.SpawnEntity(500, 0, x, y)
        static_ids.append(eid)

    return world, moving_ids, static_ids


def run_combat_bench():
    random.seed(42)
    print("Создание мира...")
    t0 = time.perf_counter()
    world, moving_ids, static_ids = build_world()
    build_time = time.perf_counter() - t0

    all_entities = world.GetAllEntities()
    print(f"Мир создан за {build_time:.3f} с, всего сущностей: {len(all_entities)}")
    print(f"  движущихся: {len(moving_ids)}, предметов: {len(static_ids)}")
    print(f"Запуск {TICKS} тиков...")

    tick_times = []
    for tick in range(TICKS):
        t_start = time.perf_counter()

        for _ in range(SPAWN_PER_TICK):
            x = random.randint(-WORLD_HALF_X + 1, WORLD_HALF_X - 2)
            y = random.randint(-WORLD_HALF_Y + 1, WORLD_HALF_Y - 2)
            world.SpawnEntity(500, 0, x, y)

        for _ in range(REMOVE_PER_TICK):
            if static_ids:
                eid = random.choice(static_ids)
                world.RemoveEntity(eid)
                static_ids.remove(eid)

        world.Tick(DT)

        for _ in range(SEARCH_PER_TICK):
            cx = random.randint(-WORLD_HALF_X + 10, WORLD_HALF_X - 10)
            cy = random.randint(-WORLD_HALF_Y + 10, WORLD_HALF_Y - 10)
            found = world.GetEntitiesInRadius(cx, cy, 10, 0)  # noqa: F841

        for _ in range(RAYCAST_PER_TICK):
            x0 = random.randint(-WORLD_HALF_X, WORLD_HALF_X)
            y0 = random.randint(-WORLD_HALF_Y, WORLD_HALF_Y)
            x1 = random.randint(-WORLD_HALF_X, WORLD_HALF_X)
            y1 = random.randint(-WORLD_HALF_Y, WORLD_HALF_Y)
            hit = world.RaycastFirst(x0, y0, x1, y1, SOLID)  # noqa: F841

        t_end = time.perf_counter()
        tick_times.append(t_end - t_start)

    mean_tick = statistics.mean(tick_times)
    median_tick = statistics.median(tick_times)
    stdev_tick = statistics.stdev(tick_times) if len(tick_times) > 1 else 0.0
    min_tick = min(tick_times)
    max_tick = max(tick_times)
    fps = 1.0 / mean_tick if mean_tick > 0 else 0.0

    print("\n=== Результаты боевого бенчмарка ===")
    print(f"Тиков: {TICKS}")
    print(f"Время на тик: mean={mean_tick:.6f} s, median={median_tick:.6f} s")
    print(f"  min={min_tick:.6f} s, max={max_tick:.6f} s, stdev={stdev_tick:.6f} s")
    print(f"Тиков в секунду (TPS симуляции): {fps:.1f}")
    print(f"Общее время цикла: {sum(tick_times):.3f} с")


if __name__ == "__main__":
    run_combat_bench()
