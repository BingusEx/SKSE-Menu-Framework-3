# Updating

Mods using the old header should be fully compatible; however, if you want to use the new features, you need to update the header file

# New Features Added To The API

Video demonstration of the update

[![Video Demonstration of the update](https://img.youtube.com/vi/ZNCh1pje3qc/0.jpg)](https://www.youtube.com/watch?v=ZNCh1pje3qc)

## Is any window open API

If the result is true, the player is currently controlling the game; otherwise, it is false

```cpp
bool IsAnyBlockingWindowOpened()
```

## Input Capturing and blocking

Register function signature

```cpp
inline Model::InputEvent* AddInputEvent(Model::InputEventCallback callback) 
```

Example of registering a user input hook

```cpp
    SKSEMenuFramework::AddInputEvent(Example5::OnInput);
```

Example of defining a user input callback function

```cpp
bool __stdcall UI::Example5::OnInput(RE::InputEvent* event) { 
    bool blockUserInput = false;

    if (event->device == RE::INPUT_DEVICE::kKeyboard) {
        if (auto button = event->AsButtonEvent()) {
            if (button->GetIDCode() == RE::BSWin32KeyboardDevice::Key::kB && button->IsDown()) {
                NonPausingWindow->IsOpen = !NonPausingWindow->IsOpen;
                blockUserInput = true;
            }
        }
    }

    return blockUserInput;
}
```
## Foreground drawing

Register function signature

```cpp
Model::HudElement* AddHudElement(Model::HudElementCallback callback)
```

Example of registering a HUD drawing function

```cpp
SKSEMenuFramework::AddHudElement(Example5::RenderOverlay);
```
For example, the following code will render text on the top right corner of the screen if the user is not interacting with any menu
```cpp

void __stdcall UI::Example5::RenderOverlay() {
    if (SKSEMenuFramework::IsAnyBlockingWindowOpened()) {
        return;
    }
    auto drawList = ImGui::GetForegroundDrawList(); 

    const char* text = "Press B to toggle the info window";
    ImVec2 textSize;
    ImGui::CalcTextSize(&textSize, text,0, false, 0);
    ImVec2 textPos = ImVec2(ImGui::GetIO()->DisplaySize.x - textSize.x - 20, 20);  // 10px padding from edges
    ImGui::ImDrawListManager::AddText(drawList, textPos, IM_COL32(255, 255, 255, 255), text);
}
```
The following code will render a circle in the middle of the screen
```cpp
void __stdcall UI::Example5::RenderOverlay() {
    if (SKSEMenuFramework::IsAnyBlockingWindowOpened()) {
        return;
    }
    auto drawList = ImGui::GetForegroundDrawList(); 
    ImVec2 center = ImGui::GetIO()->DisplaySize;
    center.x *= 0.5;
    center.y *= 0.5;
    ImGui::ImDrawListManager::AddCircle(drawList, center, 100, IM_COL32(255, 0, 0, 255), 100, 10);
}
```
## Load Texture File

SVG, DDS, and most conventional image files are supported
```cpp
inline ImTextureID LoadTexture(std::string texturePath, ImVec2 size = {0, 0})
```
Example of how to render images on imgui
```cpp
auto texture = SKSEMenuFramework::LoadTexture("Data\\interface\\unlit-bomb.svg", {100, 100});
auto texture2 = SKSEMenuFramework::LoadTexture("Data\\interface\\screenshot.png");
ImGui::Text("Image Display: ");
ImGui::SameLine();
ImGui::Image(texture, ImVec2(100, 100));
ImGui::Image(texture2, ImVec2(640, 360));
```

# Compiling and building

## Environment variables

[How to set up envioriment variables](https://gist.github.com/Thiago099/b45ec7832fb754325b29a61006bcd10c)

- COMMONLIB_SSE_FOLDER

  Clone [this](https://github.com/alandtse/CommonLibVR) Repository, to somewhere safe and adds its path to this environment variable on Windows.

```bash
git clone --recursive https://github.com/alandtse/CommonLibVR
cd CommonLibVR
git checkout sse
```
  
## Optional ouput folder optional variables

- SKYRIM_FOLDER
- WILDLANDER_OWRT_FOLDER
- SKYRIM_OWRT_FOLDER
- SKYRIM_MODS_FOLDER
- SKYRIM_MODS_FOLDER2