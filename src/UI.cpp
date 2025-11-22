#include "UI.h"
#include "WindowManager.h"
#include <imgui.h>
#include "Renderer.h"
#include "StyleManager.h"

static ImGuiTextFilter filter;

UI::MenuTree* UI::RootMenu = new UI::MenuTree();


int frame = 0;

size_t item_current_idx = 0;
size_t node_id = 0;
UI::MenuTree* display_node;


static ImGuiTreeNodeFlags base_flags =
    ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
static int selection_mask = (1 << 2);

void DummyRenderer(std::pair<const std::string, UI::MenuTree*>& node) {
    ++node_id;
    for (auto& item : node.second->Children) {
        DummyRenderer(item);
    }
}

void RenderNode(std::pair<const std::string, UI::MenuTree*>& node) {
    ++node_id;
    ImGuiTreeNodeFlags node_flags = base_flags;
    // const bool is_selected = item_current_idx == i;
    if (item_current_idx == node_id) node_flags |= ImGuiTreeNodeFlags_Selected;

    if (node.second->Children.size() == 0) {
        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }
    bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)node_id, node_flags, node.first.c_str(), node_id);


    bool itemClicked = ImGui::IsItemClicked();
    bool itemToggledOpen = ImGui::IsItemToggledOpen();
    bool gamepadButtonPressed = ImGui::IsKeyPressed(ImGuiKey_GamepadFaceDown);  // Typically A button
    bool itemIsFocused = ImGui::IsItemFocused();  // Check if the item is focused/highlighted by gamepad navigation

    if ((itemClicked || (gamepadButtonPressed && itemIsFocused)) && !itemToggledOpen) {
        if (node.second->Render) {
            item_current_idx = node_id;
            display_node = node.second;
        }
    }
    if (node_open && node.second->Children.size() != 0) {
        for (auto& item : node.second->SortedChildren) {
            RenderNode(item);
        }
        ImGui::TreePop();
    } else {
        for (auto& item : node.second->Children) {
            DummyRenderer(item);
        }
    }
}

void __stdcall UI::RenderMenuWindow() {
    auto viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2{0.5f, 0.5f});
    ImGui::SetNextWindowSize(ImVec2{viewport->Size.x * 0.8f, viewport->Size.y * 0.8f}, ImGuiCond_Appearing);
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoTitleBar;

    ImGui::Begin("#MCPMainWindow", nullptr, window_flags);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Resume Game")) {
                WindowManager::MainInterface->PauseGame = false;
            }
            if (ImGui::MenuItem("Settings")) {
                WindowManager::ConfigInterface->IsOpen = true;
            }
            ImGui::EndMenu();
        }

        float barWidth = ImGui::GetWindowWidth();
        float barHeight = ImGui::GetFrameHeight();
        float textWidth = ImGui::CalcTextSize("Mod Control Panel").x;

        float closeButtonSize = barHeight;
        float padding = ImGui::GetStyle().ItemSpacing.x;

        float availableWidth = barWidth - closeButtonSize - padding;
        float pos = (availableWidth * 0.5f) - (textWidth * 0.5f);
        ImGui::SameLine(pos);
        ImGui::Text("Mod Control Panel");

        float closeButtonPos = barWidth - closeButtonSize - padding;
        ImGui::SameLine(closeButtonPos);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        if (ImGui::Button("X", ImVec2(closeButtonSize, closeButtonSize))) {
            WindowManager::MainInterface->IsOpen = false;
            WindowManager::ConfigInterface->IsOpen = false;
        }
        ImGui::PopStyleVar();

        ImGui::EndMenuBar();
    }

    float filterHeight = 20.0f;
    float headerHeight = 20.0f;
    float headerOffsetY = 0.0f;
    if (Config::MenuStyle != MenuStyle::Classic) {
        filterHeight = 50.0f;
        headerHeight = 41.0f;
        headerOffsetY = 5.0f;
    }

    // Filter section
    ImGui::BeginChild("TreeView2", ImVec2(ImGui::GetContentRegionAvail().x * 0.3f, filterHeight), ImGuiChildFlags_None);
    filter.Draw("##SKSEModControlPanelMenuFilter", -FLT_MIN);
    ImGui::EndChild();

    ImGui::SameLine();

    // Header section
    ImGui::BeginChild("SKSEModControlPanelModMenuHeader", ImVec2(0, headerHeight), ImGuiChildFlags_None);
    if (display_node) {
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth = ImGui::CalcTextSize(display_node->Title.c_str()).x;
        float offsetX = (windowWidth - textWidth) * 0.5f;
        ImGui::SetCursorPosX(offsetX);
        ImGui::SetCursorPosY(headerOffsetY);
        ImGui::Text("%s", display_node->Title.c_str());
    }
    ImGui::EndChild();

    // Tree view section
    ImGui::BeginChild("SKSEModControlPanelTreeView", ImVec2(ImGui::GetContentRegionAvail().x * 0.3f, -FLT_MIN),
                      ImGuiChildFlags_Border);
    node_id = 0;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 5.0f));
    for (const auto& item : RootMenu->Children) {
        if (filter.PassFilter(item.first.c_str()) &&
            (ImGui::CollapsingHeader(std::format("{}##{}", item.first, node_id).c_str()))) {
            for (auto node : item.second->SortedChildren) {
                RenderNode(node);
            }
        } else {
            for (auto node : item.second->Children) {
                DummyRenderer(node);
            }
        }
    }
    ImGui::PopStyleVar();
    ImGui::EndChild();

    ImGui::SameLine();

    // Content section
    ImGui::BeginChild("SKSEModControlPanelMenuNode", ImVec2(0, -FLT_MIN), ImGuiChildFlags_Border);
    if (display_node) {
        display_node->Render();
    }
    ImGui::EndChild();

    ImGui::End();
}

void UI::AddToTree(UI::MenuTree* node, std::vector<std::string>& path, RenderFunction render, std::string title) {
    if (!path.empty()) {
        auto currentName = path.front();
        path.erase(path.begin());

        auto foundItem = node->Children.find(currentName);
        if (foundItem != node->Children.end()) {
            AddToTree(foundItem->second, path, render, title);
        } else {
            auto newItem = new UI::MenuTree();
            node->Children[currentName] = newItem;
            node->SortedChildren.push_back(std::pair<const std::string, UI::MenuTree*>(currentName, newItem));
            AddToTree(newItem, path, render, title);
        }
    } else {
        node->Render = render;
        node->Title = title;
    }
}

bool ToggleButton(const char* label, bool* v) {
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight();
    float width = height * 1.8f;
    float radius = height * 0.5f;

    ImGui::InvisibleButton(label, ImVec2(width, height));
    bool clicked = ImGui::IsItemClicked();
    if (clicked) *v = !*v;

    float t = *v ? 1.0f : 0.0f;
    ImU32 col_bg = ImGui::GetColorU32(*v ? ImGuiCol_ButtonActive : ImGuiCol_ButtonActive);

    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
    draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f,
                               IM_COL32(255, 255, 255, 255));

    ImGui::SameLine();
    ImGui::Text("%s", label);

    return clicked;
}

void UI::RenderConfigWindow() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2{0.5f, 0.5f});
    ImGui::SetNextWindowSize(ImVec2{viewport->Size.x * 0.4f, viewport->Size.y * 0.4f}, ImGuiCond_Appearing);
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoTitleBar;

    if (ImGui::Begin("Settings##Settings", nullptr, window_flags)) {
        if (ImGui::BeginMenuBar()) {
            ImGui::Text("Settings");
            float barWidth = ImGui::GetWindowWidth();
            float barHeight = ImGui::GetFrameHeight();
            float textWidth = ImGui::CalcTextSize("Mod Control Panel").x;

            float closeButtonSize = barHeight;
            float padding = ImGui::GetStyle().ItemSpacing.x;

             float closeButtonPos = barWidth - closeButtonSize - padding;
             ImGui::SameLine(closeButtonPos);
             ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
             if (ImGui::Button("X", ImVec2(closeButtonSize, closeButtonSize))) {
                 WindowManager::ConfigInterface->IsOpen = false;
             }
            ImGui::PopStyleVar();
            ImGui::EndMenuBar();
        }
        const char* styleNames[] = {"Skyrim", "Modern", "Classic"};
        int currentStyle = static_cast<int>(Config::MenuStyle);

        ImGui::Text("Menu Style:");
        if (ImGui::Combo("##MenuStyleCombo", &currentStyle, styleNames, IM_ARRAYSIZE(styleNames))) {
            Config::MenuStyle = static_cast<MenuStyle>(currentStyle);
            StyleManager::LoadStyle();
            Config::Save();
        }

        if (ToggleButton("Freeze Time", &Config::FreezeTimeOnMenu)) {
            Config::Save();
        }
        
        if (ToggleButton("Blur Background", &Config::BlurBackgroundOnMenu)) {
            Config::Save();
        }

        ImGui::End();
    }
}
