import pytest

from wfz_pocketworld import PlaneManager


class TestPlaneManager:
    def test_create_plane(self):
        pid = PlaneManager.create(0)
        try:
            assert pid > 0

        finally:
            PlaneManager.destroy(pid)

    def test_create_multiple(self):
        pid1 = PlaneManager.create(1)
        pid2 = PlaneManager.create(2)
        try:
            assert pid1 != pid2

        finally:
            PlaneManager.destroy(pid1)
            PlaneManager.destroy(pid2)

    def test_destroy_existing(self):
        pid = PlaneManager.create(0)
        PlaneManager.destroy(pid)

    def test_destroy_nonexistent(self):
        with pytest.raises(ValueError, match="not found"):
            PlaneManager.destroy(999)

    def test_destroy_twice(self):
        pid = PlaneManager.create(0)
        PlaneManager.destroy(pid)
        with pytest.raises(ValueError, match="not found"):
            PlaneManager.destroy(pid)
