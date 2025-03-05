#pragma once
#include "GameObject.h"
#include "Player.h"
#include "imgui.h"

class PlayerUI : public CubeObject
{
public:
    Player* player;
    int playerID;
    float hp;
    float powerUp;
    ImVec4 profileColor;

    // ������
    PlayerUI(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float scale, int id, Player* p);

    // UI ǥ�� �Լ�
    void ShowPlayerUI();

    void ShowGameOverUI();

        ImVec2 windowSize = ImGui::GetWindowSize();

        ImVec2 windowPos = (playerID == 1) ? ImVec2(10, 10) : ImVec2(ImGui::GetIO().DisplaySize.x - windowSize.x - 10, 10);
        ImGui::SetWindowPos(windowPos);

        // Player Profile (�ܻ� �簢��)
        ImGui::Text("Player %d", playerID);
        ImGui::ColorButton("Profile", profileColor, ImGuiColorEditFlags_NoTooltip, ImVec2(50, 50));

        // HP Bar
        ImGui::Text("HP:");
        ImGui::ProgressBar(player->GetHP() / 100 , ImVec2(150, 20));

        // PowerUp Gage
        ImGui::Text("Power Up:");
        ImGui::ProgressBar(player->GetPowerUpGage() / 100, ImVec2(150, 20));

        ImGui::End();
    }

    void ShowGameOverUI()
    {
        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Always);

        ImGui::Begin("GameOver", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 windowPos = ImVec2(
            (ImGui::GetIO().DisplaySize.x - windowSize.x) / 2,
            (ImGui::GetIO().DisplaySize.y - windowSize.y) / 2
        );
        ImGui::SetWindowPos(windowPos);

        auto CenterItem = [](float itemWidth, float offset = 8.0f) {
            float windowWidth = ImGui::GetContentRegionAvail().x;
            ImGui::SetCursorPosX((windowWidth - itemWidth) * 0.5f + offset);
            };

        std::string winnerText = "Winner is Player " + std::to_string((playerID == 1) ? 2 : 1) + "!!";
        CenterItem(ImGui::CalcTextSize(winnerText.c_str()).x);
        ImGui::Text("%s", winnerText.c_str());

        CenterItem(150);
        if (ImGui::Button("Restart", ImVec2(150, 20)))
        {
            // ����ŸƮ ���� �߰�
        }

        ImGui::End();
    }

    void Update(double deltaTime) 
    {
        ShowPlayerUI();

        if (player->IsDead())
            ShowGameOverUI();
    }
};

