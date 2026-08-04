#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "plane.h"
#include "plane_manager.h"

#define UNUSED(x) (void)(x)

static PyObject *py_world_init(PyObject *self, PyObject *args)
{
    UNUSED(self);
    UNUSED(args);

    if (InitPlaneManager() != 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to initialize world");
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *py_world_shutdown(PyObject *self, PyObject *args)
{
    UNUSED(self);
    UNUSED(args);

    DestroyPlaneManager();
    Py_RETURN_NONE;
}

static PyObject *py_plane_create(PyObject *self, PyObject *args)
{
    UNUSED(self);

    unsigned int flags;
    if (!PyArg_ParseTuple(args, "I", &flags))
        return NULL;

    unsigned int id = AddPlane(flags);
    if (id == 0)
    {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create plane");
        return NULL;
    }

    return PyLong_FromUnsignedLong(id);
}

static PyObject *py_plane_destroy(PyObject *self, PyObject *args)
{
    UNUSED(self);

    unsigned int plane_id;
    if (!PyArg_ParseTuple(args, "I", &plane_id))
        return NULL;

    int rc = DestroyPlane(plane_id);
    if (rc == -1)
    {
        PyErr_SetString(PyExc_RuntimeError, "Plane manager not initialized");
        return NULL;
    }

    if (rc == -2)
    {
        PyErr_Format(PyExc_ValueError, "Plane with id %u not found", plane_id);
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyMethodDef core_methods[] = {
    {"world_init", py_world_init, METH_VARARGS, "Initialize world (plane manager)"},
    {"world_shutdown", py_world_shutdown, METH_VARARGS, "Shutdown world and free resources"},
    {"plane_create", py_plane_create, METH_VARARGS, "Create plane (flags) -> id"},
    {"plane_destroy", py_plane_destroy, METH_VARARGS, "Destroy plane by id"},
    {NULL, NULL, 0, NULL}};

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
