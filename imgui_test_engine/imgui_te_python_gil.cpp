#ifdef IMGUI_TEST_ENGINE_WITH_PYTHON_GIL

#include "imgui_te_python_gil.h"

#include <pybind11/pybind11.h>
namespace py = pybind11;

#include <memory>

//#define LOG_GIL(x) printf(x)
#define LOG_GIL(x)


namespace ImGuiTestEnginePythonGIL
{

    ReleaseGilOnMainThread_Scoped::ReleaseGilOnMainThread_Scoped()
    {
        if (!Py_IsInitialized())
        {
            LOG_GIL("ReleaseGilOnMainThread_Scoped: Py_IsInitialized() == false\n");
            return;
        }
        LOG_GIL("ReleaseGilOnMainThread_Scoped: start...\n");
        _impl = static_cast<void *>(new py::gil_scoped_release());
        LOG_GIL("ReleaseGilOnMainThread_Scoped: done...\n");
    }

    ReleaseGilOnMainThread_Scoped::~ReleaseGilOnMainThread_Scoped()
    {
        if (!Py_IsInitialized())
        {
            LOG_GIL("~ReleaseGilOnMainThread_Scoped: Py_IsInitialized() == false\n");
            return;
        }
        if (_impl)
        {
            LOG_GIL("~ReleaseGilOnMainThread_Scoped: start...\n");
            delete static_cast<py::gil_scoped_release *>(_impl);
            LOG_GIL("~ReleaseGilOnMainThread_Scoped: done...\n");
        }
        else
        {
            LOG_GIL("~ReleaseGilOnMainThread_Scoped: _impl == nullptr\n");
        }
    }


    std::unique_ptr<py::gil_scoped_acquire> GGilScopedAcquire;

    void AcquireGilOnCoroThread()
    {
        if (!Py_IsInitialized())
        {
            LOG_GIL("AcquireGilOnCoroThread: Py_IsInitialized() == false\n");
            return;
        }
        assert(GGilScopedAcquire.get() == nullptr);
        LOG_GIL("AcquireGilOnCoroThread: start...\n");
        GGilScopedAcquire = std::make_unique<py::gil_scoped_acquire>();
        LOG_GIL("AcquireGilOnCoroThread: done...\n");
    }

    void ReleaseGilOnCoroThread()
    {
        if (!Py_IsInitialized())
        {
            LOG_GIL("ReleaseGilOnCoroThread: Py_IsInitialized() == false\n");
            return;
        }
        LOG_GIL("ReleaseGilOnCoroThread: start...\n");
        assert(GGilScopedAcquire.get() != nullptr);
        GGilScopedAcquire.reset();
        LOG_GIL("ReleaseGilOnCoroThread: done...\n");
    }

}

#endif // #ifdef IMGUI_TEST_ENGINE_WITH_PYTHON_GIL
