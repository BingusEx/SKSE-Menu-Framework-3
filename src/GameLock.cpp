#include "GameLock.h"
#include "Config.h"

GameLock::State GameLock::lastState = GameLock::State::None;

bool didFreezeTime = false;
bool didPushBlur = false;
void GameLock::SetState(State currentState) {
    if (lastState == currentState) {
        return;
    }
    lastState = currentState;
    if (Config::FreezeTimeOnMenu) {
        const auto main = reinterpret_cast<RE::Main*>(RE::Main::GetSingleton());
        if (currentState == State::Locked) {
            main->freezeTime = true;
            didFreezeTime = true;
        } else {
            didFreezeTime = false;
            main->freezeTime = false;
        }
    } else if (didFreezeTime) {
        didFreezeTime = false;
        const auto main = reinterpret_cast<RE::Main*>(RE::Main::GetSingleton());
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
        auto& io = ImGui::GetIO();
        io.ClearInputKeys();
    }
}
