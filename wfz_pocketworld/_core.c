#define PY_SSIZE_T_CLEAN
#include <Python.h>

#define UNUSED(x) (void)(x)

static PyMethodDef core_methods[] = {
    {NULL, NULL, 0, NULL},
};

static struct PyModuleDef core_module = {
    PyModuleDef_HEAD_INIT,
    "_core",
    NULL,
    -1,
    core_methods,
    NULL, // m_slots
    NULL, // m_traverse
    NULL, // m_clear
    NULL  // m_free
};

PyMODINIT_FUNC PyInit__core(void)
{
    return PyModule_Create(&core_module);
}
