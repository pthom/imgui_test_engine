#pragma once
#ifdef IMGUI_TEST_ENGINE_WITH_PYTHON_GIL

// When using imgui_test_engine via python bindings, the Global Interpreter Lock (GIL)
// must be transferred between the main thread and the coroutine thread, so that it remains
// possible to call python functions from each of these threads.
//
// See imgui_te_python_gil.jpg for more explanation about how the GIL is transferred between the main thread and the coroutine thread
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

    // Call AcquireGilOnCoroThread and ReleaseGilOnCoroThread on the coroutine thread
    void AcquireGilOnCoroThread();
    void ReleaseGilOnCoroThread();
};

#endif // #ifdef IMGUI_TEST_ENGINE_WITH_PYTHON_GIL
