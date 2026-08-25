#include "bindings.h"

#include <pybind11/pybind11.h>

namespace py = pybind11;

void register_bindings(py::module_ &m)
{
    m.doc() = "WorldForgeZero PocketWorld Engine (embedded core)";

    m.def("hello", []()
          { return "Hello from C++!"; });
}
