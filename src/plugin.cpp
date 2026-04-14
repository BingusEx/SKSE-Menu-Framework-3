#include "Hooks.h"
#include "Config.h"
#include "Logger.h"
#include "UI.h"
#include "SKSEMenuFramework.h"
#include "Licence.h"
#include "Translations.h"
#include "Version.hpp"

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SetupLog();
    #if VALIDATE_LICENSE
    if (!Licence::Validate()) {
        return false;
    }
    #endif
    logger::info("Plugin loaded");
    SKSE::Init(skse);
    Config::Init();
    WindowManager::MainInterface = AddWindow(UI::RenderMenuWindow);
    WindowManager::ConfigInterface = AddWindow(UI::RenderConfigWindow);
    WindowManager::MainInterface->BlockUserInput = true;
    WindowManager::ConfigInterface->BlockUserInput = true;
    Translations::Install();
    Hooks::Install();
    return true;
}

SKSEPluginInfo(
    .Version = Plugin::ModVersion, 
    .Name = Plugin::ModName, 
    .Author = "SkyrimThiago",
    .StructCompatibility = SKSE::StructCompatibility::Independent,
    .RuntimeCompatibility = SKSE::VersionIndependence::AddressLibrary
);