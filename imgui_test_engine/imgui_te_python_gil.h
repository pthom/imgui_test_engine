#pragma once
#ifdef IMGUI_TEST_ENGINE_WITH_PYTHON_GIL

// When using imgui_test_engine via python bindings, the Global Interpreter Lock (GIL)
// must be transferred between the main thread and the coroutine thread, so that it remains
// possible to call python functions from each of these threads.
//
// See imgui_te_python_gil.md (and the original sketch imgui_te_python_gil.jpg) for how the GIL is transferred
// between the main thread and the coroutine thread, and why the coroutine thread keeps one Python thread state.
namespace ImGuiTestEnginePythonGIL
{
    // Instantiate ReleaseGilOnMainThread_Scoped in a scope to release the GIL on the main thread
    struct ReleaseGilOnMainThread_Scoped
    {
        ReleaseGilOnMainThread_Scoped();
        ~ReleaseGilOnMainThread_Scoped();
    private:
        void *_impl;
    };

    // On the coroutine thread, call:
    //   - AcquireGilOnCoroThread_ThreadStart() once, when the coroutine thread starts running user code
    //   - ReleaseGilOnCoroThread() / AcquireGilOnCoroThread() around each yield to the main thread
    //   - ReleaseGilOnCoroThread_ThreadEnd() once, when the user code has returned
    // The thread keeps a single Python thread state for its whole lifetime: it must survive yields,
    // since Python frames of the user code are still live on it.
    void AcquireGilOnCoroThread_ThreadStart();
    void AcquireGilOnCoroThread();
    void ReleaseGilOnCoroThread();
    void ReleaseGilOnCoroThread_ThreadEnd();
};

#endif // #ifdef IMGUI_TEST_ENGINE_WITH_PYTHON_GIL
