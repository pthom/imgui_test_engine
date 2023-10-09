#ifdef IMGUI_TEST_ENGINE_WITH_PYTHON_GIL

#include "imgui_te_python_gil.h"

#include <pybind11/pybind11.h>
namespace py = pybind11;

#include <memory>


namespace ImGuiTestEnginePythonGIL
{

    ReleaseGilOnMainThread_Scoped::ReleaseGilOnMainThread_Scoped()
    {
        if (!Py_IsInitialized())
            return;
        _impl = static_cast<void *>(new py::gil_scoped_release());
    }

    ReleaseGilOnMainThread_Scoped::~ReleaseGilOnMainThread_Scoped()
    {
        if (!Py_IsInitialized())
            return;
        if (_impl)
            delete static_cast<py::gil_scoped_release *>(_impl);
    }


    std::unique_ptr<py::gil_scoped_acquire> GGilScopedAcquire;

    void AcquireGilOnCoroThread()
    {
        if (!Py_IsInitialized())
            return;
        assert(GGilScopedAcquire.get() == nullptr);
        GGilScopedAcquire = std::make_unique<py::gil_scoped_acquire>();
    }

    void ReleaseGilOnCoroThread()
    {
        if (!Py_IsInitialized())
            return;
        assert(GGilScopedAcquire.get() != nullptr);
        GGilScopedAcquire.reset();
    }

}

#endif // #ifdef IMGUI_TEST_ENGINE_WITH_PYTHON_GIL
