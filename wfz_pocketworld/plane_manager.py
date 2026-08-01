import wfz_pocketworld._core


class PlaneManager:
    @staticmethod
    def create(flags: int) -> int:
        return wfz_pocketworld._core.plane_create(flags)

    @staticmethod
    def destroy(plane_id: int) -> None:
        return wfz_pocketworld._core.plane_destroy(plane_id)
