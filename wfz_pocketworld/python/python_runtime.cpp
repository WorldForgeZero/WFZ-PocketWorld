#include "python_runtime.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <pybind11/embed.h>

#include "bindings.h"

namespace py = pybind11;
namespace fs = std::filesystem;

namespace python_runtime
{

    static py::module_::module_def wfz_core_mod_def;
    static std::unique_ptr<py::scoped_interpreter> g_interpreter;

    PyMODINIT_FUNC PyInit__core_impl()
    {
        auto m = py::module_::create_extension_module("wfz._core", nullptr, &wfz_core_mod_def);
        register_bindings(m);
        return m.ptr();
    }

    bool Init(const std::string &scriptDir)
    {
        if (g_interpreter)
            return false;

        PyImport_AppendInittab("wfz._core", &PyInit__core_impl);
        setenv("PYTHONPATH", scriptDir.c_str(), 1);

        g_interpreter = std::make_unique<py::scoped_interpreter>();
        return true;
    }

    bool RunFile(const std::string &path)
    {
        if (!g_interpreter)
            return false;

        try
        {
            py::eval_file(path);
            return true;
        }
        catch (const py::error_already_set &e)
        {
            std::cerr << "Ошибка в " << path << ": " << e.what() << std::endl;
            return false;
        }
    }

    void Shutdown()
    {
        g_interpreter.reset();
    }
}
