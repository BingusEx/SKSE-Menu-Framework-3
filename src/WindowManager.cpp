#include "WindowManager.h"

Window::Window() {
	Interface = new WindowInterface();
    Render = nullptr;
}


bool WindowManager::IsAnyWindowOpen() {
    auto it = std::find_if(WindowManager::Windows.begin(), WindowManager::Windows.end(),
                           [](Window* x) { return x->Interface->IsOpen.load(); });
    return it != WindowManager::Windows.end();
}


bool WindowManager::ShouldTheGameBePaused() {
    auto it = std::find_if(WindowManager::Windows.begin(), WindowManager::Windows.end(),
                           [](Window* x) { return x->Interface->IsOpen.load() && x->Interface->PauseGame.load();  });
    return it != WindowManager::Windows.end();
}

void WindowManager::Close() {
    WindowManager::MainInterface->PauseGame = true;
    WindowManager::ConfigInterface->PauseGame = true;
    ConfigInterface->IsOpen = false;
    MainInterface->IsOpen = false;
}

void WindowManager::Open() {
    MainInterface->IsOpen = true;
}
