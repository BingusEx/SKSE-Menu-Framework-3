#include "Hooks.h"
#include "Renderer.h"
#include "FontManager.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "WindowManager.h"
#include "InputEventHandler.h"
#include "HudManager.h"
#include "GameLock.h"
#include "TextureLoader.h"


void Hooks::Install() {
    D3DInitHook::install();
    DXGIPresentHook::install();
    ProcessInputQueueHook::install();
}

void Hooks::D3DInitHook::install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(
        REL::RelocationID(75595, 77226, 75595).address() + REL::Relocate(0x9, 0x275, 0x9), thunk);
}

void Hooks::DXGIPresentHook::install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(
        REL::RelocationID(75461, 77246, 75461).address() + REL::Relocate(0x9, 0x9, 0x15), thunk);
}

void Hooks::ProcessInputQueueHook::install() {
    SKSE::AllocTrampoline(14);
    auto& trampoline = SKSE::GetTrampoline();
    originalFunction = trampoline.write_call<5>(
        REL::RelocationID(67315, 68617, 67315).address() + REL::Relocate(0x7B, 0x7B, 0x81), thunk);
}
void DisableImGuiInput() {
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
    io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;
}
void EnableImGuiInput() {
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    io.ConfigFlags &= ~ImGuiConfigFlags_NavNoCaptureKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
}
void Hooks::ProcessInputQueueHook::thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher,
                                      RE::InputEvent* const* a_event) {
    bool isInputCapturedByOpenClose = UI::Renderer::ProcessOpenClose(a_event);

    if (!ImGui::IsAnyItemActive()) {
        a_event = InputEventHandler::Process(const_cast<RE::InputEvent**>(a_event));
    }

    if (isInputCapturedByOpenClose) {
        constexpr RE::InputEvent* const dummy[] = {nullptr};
        originalFunction(a_dispatcher, dummy);
    } else {
        if (WindowManager::ShouldTheGameBePaused()) {
            constexpr RE::InputEvent* const dummy[] = {nullptr};
            originalFunction(a_dispatcher, dummy);
            EnableImGuiInput();
            UI::TranslateInputEvent(a_event);
        } else {
            DisableImGuiInput();
            originalFunction(a_dispatcher, a_event);
        }
    }
}

LRESULT Hooks::WndProcHook::thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_KILLFOCUS) {
        auto& io = ImGui::GetIO();
        io.ClearInputKeys();
    }
    return func(hWnd, uMsg, wParam, lParam);
}

void Hooks::D3DInitHook::thunk() {
    logger::debug("[D3DInitHook] START");

    originalFunction();

    const auto renderer = RE::BSGraphics::Renderer::GetSingleton();
    if (!renderer) {
        SKSE::log::error("couldn't find renderer");
        return;
    }
    auto data = renderer->GetRuntimeData();
    const auto swapChain = reinterpret_cast<IDXGISwapChain*>(data.renderWindows[0].swapChain);
    if (!swapChain) {
        SKSE::log::error("couldn't find swapChain");
        return;
    }

    DXGI_SWAP_CHAIN_DESC desc{};
    if (FAILED(swapChain->GetDesc(std::addressof(desc)))) {
        SKSE::log::error("IDXGISwapChain::GetDesc failed.");
        return;
    }
    const auto device = reinterpret_cast<ID3D11Device*>(data.forwarder);
    const auto context = reinterpret_cast<ID3D11DeviceContext*>(data.context);

    TextureLoader::Init(device, context);

    SKSE::log::info("Initializing ImGui...");

    ImGui::CreateContext();

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.IniFilename = nullptr;
    io.MouseDrawCursor = true;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    if (!ImGui_ImplWin32_Init(desc.OutputWindow)) {
        SKSE::log::error("ImGui initialization failed (Win32)");
        return;
    }

    if (!ImGui_ImplDX11_Init(device, context)) {
        SKSE::log::error("ImGui initialization failed (DX11)");
        return;
    }

    UI::Renderer::initialized.store(true);

    SKSE::log::info("ImGui initialized.");

    WndProcHook::func = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrA(desc.OutputWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook::thunk)));
    if (!WndProcHook::func) {
        SKSE::log::error("SetWindowLongPtrA failed!");
    }

    Config::LoadStyle();

    auto regular = FontManager::LoadFonts(io, Config::FontSizeMedium);
    io.FontDefault = regular.defaultFont;

    FontManager::fontSizes["Big"] = FontManager::LoadFonts(io, Config::FontSizeBig);
    FontManager::fontSizes["Small"] = FontManager::LoadFonts(io, Config::FontSizeSmall);
    FontManager::fontSizes["Default"] = regular;

    io.Fonts->Build();

    logger::debug("[D3DInitHook] FINISH");
}

void Hooks::DXGIPresentHook::thunk(std::uint32_t a_timer) {
    originalFunction(a_timer);

    if (!UI::Renderer::initialized.load()) {
        return;
    }



    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    {
        // trick imgui into rendering at game's real resolution (ie. if upscaled with Display Tweaks)
        static const auto screenSize = RE::BSGraphics::Renderer::GetScreenSize();

        auto& io = ImGui::GetIO();
        io.DisplaySize.x = static_cast<float>(screenSize.width);
        io.DisplaySize.y = static_cast<float>(screenSize.height);
    }
    ImGui::NewFrame();
    HudManager::Render();
    if (WindowManager::IsAnyWindowOpen()) {
        auto& io = ImGui::GetIO();
        if (!WindowManager::ShouldTheGameBePaused()) {
            io.MouseDrawCursor = false;
            GameLock::SetState(GameLock::State::Resume);
        } else {
            GameLock::SetState(GameLock::State::Locked);
            io.MouseDrawCursor = true;
        }

        UI::Renderer::RenderWindows();
    } else {
        auto& io = ImGui::GetIO();
        io.MouseDrawCursor = false;
        GameLock::SetState(GameLock::State::Unlocked);
    }
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    FontManager::CleanFont();
}