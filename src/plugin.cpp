#include "Hooks.h"
#include "Config.h"
#include "Logger.h"
#include "UI.h"
#include "SKSEMenuFramework.h"
#include "Licence.h"

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SetupLog();
    if (!Licence::Validate()) {
        return false;
    }
    logger::info("Plugin loaded");
    SKSE::Init(skse);
    Config::Init();
    WindowManager::MainInterface = AddWindow(UI::RenderMenuWindow);
    WindowManager::MainInterface->PauseGame = true;
    Hooks::Install();
    return true;
}