#include "GameLock.h"
#include "Main.h"
#include "Config.h"
#include "imgui.h"

GameLock::State GameLock::lastState = GameLock::State::None;

void GameLock::SetState(State currentState) {
    if (lastState == currentState) {
        return;
    }
    lastState = currentState;
    if (Config::FreezeTimeOnMenu) {
        const auto main = reinterpret_cast<CustomRE::Main*>(RE::Main::GetSingleton());
        if (currentState == State::Locked) {
            main->freezeTime = true;
        } else {
            main->freezeTime = false;
        }
    }

    if (Config::BlurBackgroundOnMenu) {
        if (currentState == State::Locked) {
            RE::UIBlurManager::GetSingleton()->IncrementBlurCount();
        } else {
            RE::UIBlurManager::GetSingleton()->DecrementBlurCount();
        }
    }

    if (currentState == State::Unlocked) {
        auto& io = ImGui::GetIO();
        io.ClearInputKeys();
    }
}
