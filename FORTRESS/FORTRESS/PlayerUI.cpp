#include "stdafx.h"
#include "GameFramework.h"
#include "PlayerUI.h"


extern GameFramework gGameFramework;

// 생성자

PlayerUI::PlayerUI(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float scale, int id, Player* p)
    : CubeObject(device, deviceContext, scale),
    playerID(id), player(p)
{
    profileColor = ImVec4(player->GetColor().x, player->GetColor().y, player->GetColor().z, 1.0f);
}

// UI 표시 함수

inline void PlayerUI::ShowPlayerUI()
{
    ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX));

    ImGui::Begin(playerID == 1 ? "Player 1" : "Player 2", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImVec2 windowSize = ImGui::GetWindowSize();

    ImVec2 windowPos = (playerID == 1) ? ImVec2(10, 10) : ImVec2(ImGui::GetIO().DisplaySize.x - windowSize.x - 10, 10);
    ImGui::SetWindowPos(windowPos);

    // Player Profile (단색 사각형)
    ImGui::Text("Player %d", playerID);
    ImGui::ColorButton("Profile", profileColor, ImGuiColorEditFlags_NoTooltip, ImVec2(50, 50));

    // HP Bar
    ImGui::Text("HP:");
    ImGui::ProgressBar(player->GetHP() / 100, ImVec2(150, 20));

    // PowerUp Gage
    ImGui::Text("Power Up:");
    ImGui::ProgressBar(player->GetPowerUpGage() / 100, ImVec2(150, 20));

    ImGui::End();
}

inline void PlayerUI::ShowGameOverUI()
{
    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Always);

    ImGui::Begin("GameOver", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 windowPos = ImVec2(
        (ImGui::GetIO().DisplaySize.x - windowSize.x) / 2,
        (ImGui::GetIO().DisplaySize.y - windowSize.y) / 2
    );
    ImGui::SetWindowPos(windowPos);

    ImGui::Text("Winner is Player %d!!", (playerID == 1) ? 2 : 1);
    //ImGui::ColorButton("Profile", profileColor, ImGuiColorEditFlags_NoTooltip, ImVec2(50, 50));
    if (ImGui::Button("Restart", ImVec2(150, 20)))
    {
        Graphics* graphics = gGameFramework.GetGraphics();
        Scene* scene = new MenuScene(graphics->GetDevice(), graphics->GetDeviceContext());
        gGameFramework.GetSceneManager()->ChangeScene(scene);
    }
    ImGui::End();
}

inline void PlayerUI::Update(double deltaTime)
{
    ShowPlayerUI();

    if (player->IsDead())
        ShowGameOverUI();
}
