#include "GameLock.h"
#include "Main.h"
#include "Config.h"
#include "imgui.h"
#include "WindowManager.h"

GameLock::State GameLock::lastState = GameLock::State::None;

bool didFreezeTime = false;
bool didPushBlur = false;
void GameLock::SetState(State currentState) {
    if (lastState == currentState) {
        return;
    }
    lastState = currentState;
    if (Config::FreezeTimeOnMenu) {
        const auto main = reinterpret_cast<CustomRE::Main*>(RE::Main::GetSingleton());
        if (currentState == State::Locked) {
            main->freezeTime = true;
            didFreezeTime = true;
        } else {
            didFreezeTime = false;
            main->freezeTime = false;
        }
    } else if (didFreezeTime) {
        didFreezeTime = false;
        const auto main = reinterpret_cast<CustomRE::Main*>(RE::Main::GetSingleton());
        main->freezeTime = false;
    }

    if (Config::BlurBackgroundOnMenu) {
        if (currentState == State::Locked) {
            RE::UIBlurManager::GetSingleton()->IncrementBlurCount();
            didPushBlur = true;
        } else {
            didPushBlur = false;
            RE::UIBlurManager::GetSingleton()->DecrementBlurCount();
        }
    } else if (didPushBlur) {
        RE::UIBlurManager::GetSingleton()->DecrementBlurCount();
        didPushBlur = false;
    }

    if (currentState == State::Unlocked) {
        WindowManager::MainInterface->PauseGame = true;
        auto& io = ImGui::GetIO();
        io.ClearInputKeys();
    }
}
