#pragma once
namespace Hooks {

    void Install();

    struct WndProcHook {
        static LRESULT thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        static inline WNDPROC func;
    };

    struct D3DInitHook {
        static void thunk();
        static void install();
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

    struct RenderUIHook {
        static int64_t thunk1(int64_t gMenuManager);
        static int64_t thunk2(int64_t gMenuManager);
        static inline REL::Relocation<decltype(thunk1)> originalFunction1;
        static inline REL::Relocation<decltype(thunk2)> originalFunction2;
        static void install();
    };

    struct ProcessInputQueueHook {
        static void thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent* const* a_event);
        static void install();
        static inline REL::Relocation<decltype(thunk)> originalFunction;
    };

}