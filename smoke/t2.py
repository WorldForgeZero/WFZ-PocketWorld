import wfz_pocketworld._core as core

print([attr for attr in dir(core) if not attr.startswith("_")])
