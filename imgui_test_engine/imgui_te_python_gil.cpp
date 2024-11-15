#ifdef IMGUI_TEST_ENGINE_WITH_PYTHON_GIL

#include "imgui_te_python_gil.h"

#include <Python.h>
#include <memory>

//#define LOG_GIL(x) printf(x)
#define LOG_GIL(x)


//
// gil_scoped_acquire and gil_scoped_release are RAII classes to acquire and release the GIL
//
struct gil_scoped_acquire
{
public:
    // non copyable
    gil_scoped_acquire(gil_scoped_acquire const&) = delete;
    gil_scoped_acquire& operator=(gil_scoped_acquire const&) = delete;

    gil_scoped_acquire() noexcept : state(PyGILState_Ensure()) { }
    ~gil_scoped_acquire() { PyGILState_Release(state); }

private:
    const PyGILState_STATE state;
};

struct gil_scoped_release
{
public:
    // non copyable
    gil_scoped_release(gil_scoped_acquire const&) = delete;
    gil_scoped_release& operator=(gil_scoped_acquire const&) = delete;

    gil_scoped_release() noexcept : state(PyEval_SaveThread()) { }
    ~gil_scoped_release() { PyEval_RestoreThread(state); }

private:
    PyThreadState *state;
};


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
        _impl = static_cast<void *>(new gil_scoped_release());
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
            delete static_cast<gil_scoped_release *>(_impl);
            LOG_GIL("~ReleaseGilOnMainThread_Scoped: done...\n");
        }
        else
        {
            LOG_GIL("~ReleaseGilOnMainThread_Scoped: _impl == nullptr\n");
        }
    }

    std::unique_ptr<gil_scoped_acquire> GGilScopedAcquire;

    void AcquireGilOnCoroThread()
    {
        if (!Py_IsInitialized())
        {
            LOG_GIL("AcquireGilOnCoroThread: Py_IsInitialized() == false\n");
            return;
        }
        assert(GGilScopedAcquire.get() == nullptr);
        LOG_GIL("AcquireGilOnCoroThread: start...\n");
        GGilScopedAcquire = std::make_unique<gil_scoped_acquire>();
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
