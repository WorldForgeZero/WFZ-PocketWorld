import pytest

import wfz_pocketworld._core as core


@pytest.fixture(autouse=True)
def manage_world():
    """Инициализируем мир перед тестом и выключаем после."""
    core.world_init()
    yield
    core.world_shutdown()


def test_create_plane():
    pid = core.plane_create(0)
    try:
        assert pid > 0

    finally:
        core.plane_destroy(pid)


def test_create_multiple():
    pid1 = core.plane_create(1)
    pid2 = core.plane_create(2)
    try:
        assert pid1 != pid2
    finally:
        core.plane_destroy(pid1)
        core.plane_destroy(pid2)


def test_destroy_existing():
    pid = core.plane_create(0)
    core.plane_destroy(pid)


def test_destroy_nonexistent():
    with pytest.raises(ValueError, match="not found"):
        core.plane_destroy(999)


def test_destroy_twice():
    pid = core.plane_create(0)
    core.plane_destroy(pid)
    with pytest.raises(ValueError, match="not found"):
        core.plane_destroy(pid)
