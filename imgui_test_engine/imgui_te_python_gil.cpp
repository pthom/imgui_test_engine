#ifdef IMGUI_TEST_ENGINE_WITH_PYTHON_GIL

#include "imgui_te_python_gil.h"

#include <Python.h>

//#define LOG_GIL(x) printf(x)
#define LOG_GIL(x)


//
// gil_scoped_release is a RAII class to release the GIL (used on the main thread)
//
struct gil_scoped_release
{
public:
    // non copyable
    gil_scoped_release(gil_scoped_release const&) = delete;
    gil_scoped_release& operator=(gil_scoped_release const&) = delete;

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

    // The coroutine thread owns one Python thread state for its whole lifetime.
    //
    // PyGILState_Ensure/Release pairs cannot be used around each yield: CPython destroys the
    // thread state when its GIL-state counter drops to zero, and that would happen in the middle
    // of the user's Python code (whose frames live on this thread state). Binding layers used to
    // hide this by nesting their own PyGILState_Ensure around the call into Python; nanobind 3
    // no longer does so on Python >= 3.12 when a thread state is already attached.
    //
    // So: PyGILState_Ensure once at thread start (this creates the thread state), then
    // PyEval_SaveThread / PyEval_RestoreThread around yields (the thread state survives),
    // and PyGILState_Release once at thread end (this destroys the thread state).
    static thread_local PyGILState_STATE GCoroThreadGilState;
    static thread_local PyThreadState*   GCoroThreadState = nullptr;
    static thread_local bool             GCoroThreadHoldsGil = false;

    void AcquireGilOnCoroThread_ThreadStart()
    {
        if (!Py_IsInitialized())
        {
            LOG_GIL("AcquireGilOnCoroThread_ThreadStart: Py_IsInitialized() == false\n");
            return;
        }
        assert(GCoroThreadState == nullptr);
        LOG_GIL("AcquireGilOnCoroThread_ThreadStart: start...\n");
        GCoroThreadGilState = PyGILState_Ensure();
        GCoroThreadState = PyThreadState_Get();
        GCoroThreadHoldsGil = true;
        LOG_GIL("AcquireGilOnCoroThread_ThreadStart: done...\n");
    }

    void AcquireGilOnCoroThread()
    {
        if (!Py_IsInitialized())
        {
            LOG_GIL("AcquireGilOnCoroThread: Py_IsInitialized() == false\n");
            return;
        }
        assert(GCoroThreadState != nullptr && !GCoroThreadHoldsGil);
        LOG_GIL("AcquireGilOnCoroThread: start...\n");
        PyEval_RestoreThread(GCoroThreadState);
        GCoroThreadHoldsGil = true;
        LOG_GIL("AcquireGilOnCoroThread: done...\n");
    }

    void ReleaseGilOnCoroThread()
    {
        if (!Py_IsInitialized())
        {
            LOG_GIL("ReleaseGilOnCoroThread: Py_IsInitialized() == false\n");
            return;
        }
        assert(GCoroThreadState != nullptr && GCoroThreadHoldsGil);
        LOG_GIL("ReleaseGilOnCoroThread: start...\n");
        PyThreadState* saved = PyEval_SaveThread();
        assert(saved == GCoroThreadState);
        (void)saved;
        GCoroThreadHoldsGil = false;
        LOG_GIL("ReleaseGilOnCoroThread: done...\n");
    }

    void ReleaseGilOnCoroThread_ThreadEnd()
    {
        if (!Py_IsInitialized())
        {
            LOG_GIL("ReleaseGilOnCoroThread_ThreadEnd: Py_IsInitialized() == false\n");
            return;
        }
        assert(GCoroThreadState != nullptr && GCoroThreadHoldsGil);
        LOG_GIL("ReleaseGilOnCoroThread_ThreadEnd: start...\n");
        PyGILState_Release(GCoroThreadGilState);
        GCoroThreadState = nullptr;
        GCoroThreadHoldsGil = false;
        LOG_GIL("ReleaseGilOnCoroThread_ThreadEnd: done...\n");
    }

}

#endif // #ifdef IMGUI_TEST_ENGINE_WITH_PYTHON_GIL
