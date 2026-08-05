#include <cstdint>
#include <pybind11/pybind11.h>

namespace py = pybind11;

static bool world_initialized = false;

int InitWorld()
{
    if (world_initialized)
        return -1;

    world_initialized = true;
    return 0;
}

void ShutdownWorld()
{
    world_initialized = false;
}

PYBIND11_MODULE(_core, m)
{
    m.def("world_init", &InitWorld, "Initialize world (plane manager)");
    m.def("world_shutdown", &ShutdownWorld, "Shutdown world and free resources");
}
