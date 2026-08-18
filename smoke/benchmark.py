import time

import wfz_pocketworld._core as core

RANGE_NUM = 1_000_000


def benchmark():
    world = core.World()

    start = time.perf_counter()
    for i in range(RANGE_NUM):
        x = i % 200 - 100
        y = i // 200 - 100
        world.SpawnEntity(100, core.ENTITY_FLOOR, x, y)

    spawn_time = time.perf_counter() - start
    print(
        f"Spawn {RANGE_NUM} entities: {spawn_time:.4f} s ({RANGE_NUM / spawn_time:.0f} ops/s)"
    )

    # Перемещение
    entity = world.GetEntity(1)
    if entity:
        start = time.perf_counter()
        for _ in range(RANGE_NUM):
            world.MoveEntity(1, 0, 0)

        move_time = time.perf_counter() - start
        print(
            f"Move {RANGE_NUM} times: {move_time:.4f} s ({RANGE_NUM / move_time:.0f} ops/s)"
        )

    # Удаление
    start = time.perf_counter()
    for i in range(1, RANGE_NUM + 1):
        world.RemoveEntity(i)

    remove_time = time.perf_counter() - start
    print(
        f"Remove {RANGE_NUM} entities: {remove_time:.4f} s ({RANGE_NUM / remove_time:.0f} ops/s)"
    )


if __name__ == "__main__":
    benchmark()
