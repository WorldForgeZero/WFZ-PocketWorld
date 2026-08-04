"""WorldForgeZero PocketWorld Engine (C extension)."""

# region core control

def world_init() -> None:
    """Инициализировать мир (менеджер плоскостей).

    Вызывается один раз перед началом работы с плоскостями.
    Если менеджер уже инициализирован - генерирует RuntimeError.
    """
    ...

def world_shutdown() -> None:
    """Завершить работу мира и освободить все ресурсы.

    Уничтожает все плоскости и сбрасывает состояние менеджера.
    """
    ...

# endregion

# region plane manager

def plane_create(flags: int) -> int:
    """Создать новую плоскость.

    :param flags: Флаги плоскости (см. PlaneFlags).
    :return:      Уникальный идентификатор плоскости (> 0).
    :raises RuntimeError: если менеджер не инициализирован или память не может быть выделена.
    """
    ...

def plane_destroy(plane_id: int) -> None:
    """Удалить плоскость по идентификатору.

    :param plane_id: ID удаляемой плоскости.
    :raises ValueError: если плоскость не найдена.
    :raises RuntimeError: если менеджер не инициализирован.
    """
    ...

# endregion
