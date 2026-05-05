#include "MainMenu.h"
#include "Dungeon/DungeonScene.h"
#include "Dungeon/DungeonGenerator.h"
#include "Rendering/Material.h"
#include "Core/AssetManager.h"

void MainMenu::OnMenuUI() {
    float buttonWidth = 200.0f;

    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - buttonWidth) * 0.5f);
    ImGui::Text("MINIENGINE");
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - buttonWidth) * 0.5f);
    if (ImGui::Button("Start Game", ImVec2(buttonWidth, 50)))
        RequestTransition(std::make_unique<DungeonScene>());

    ImGui::Spacing();

    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - buttonWidth) * 0.5f);
    if (ImGui::Button("Quit", ImVec2(buttonWidth, 50)))
        exit(0);
}