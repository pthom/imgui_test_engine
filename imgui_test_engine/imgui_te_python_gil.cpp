#ifdef IMGUI_TEST_ENGINE_WITH_PYTHON_GIL

#include "imgui_te_python_gil.h"

#include <pybind11/pybind11.h>
#include "Python.h"

namespace py = pybind11;


namespace PythonGIL
{
    ReleaseGilOnMainThread_Scoped::ReleaseGilOnMainThread_Scoped()
    {
        if (Py_IsInitialized())
            _impl = static_cast<void *>(new py::gil_scoped_release());
        else
            _impl = nullptr;
    }

    ReleaseGilOnMainThread_Scoped::~ReleaseGilOnMainThread_Scoped()
    {
        if (_impl)
            delete static_cast<py::gil_scoped_release *>(_impl);
    }


    std::optional<PyGILState_STATE> GGILState_Coro;

    void AcquireGilOnCoroThread()
    {
        if (!Py_IsInitialized())
            return;

        assert(! GGILState_Coro.has_value());
        GGILState_Coro = PyGILState_Ensure();
    }

    void ReleaseGilOnCoroThread()
    {
        if (!Py_IsInitialized())
            return;

        assert(GGILState_Coro.has_value());
        PyGILState_Release(GGILState_Coro.value());
        GGILState_Coro.reset();
    }

}

#endif // #ifdef IMGUI_TEST_ENGINE_WITH_PYTHON_GIL
