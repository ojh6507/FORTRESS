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

    // 생성자
    PlayerUI(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float scale, 
        int id, Player* p)
        : CubeObject(device, deviceContext, scale), 
        playerID(id), player(p) 
    { 
        profileColor = ImVec4(player->GetColor().x, player->GetColor().y, player->GetColor().z, 1.0f);
    }

    // UI 표시 함수
    void ShowPlayerUI()
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
        ImGui::ProgressBar(player->GetHP() / 100 , ImVec2(150, 20));

        // PowerUp Gage
        ImGui::Text("Power Up:");
        ImGui::ProgressBar(player->GetPowerUpGage() / 100, ImVec2(150, 20));

        ImGui::End();
    }

    void ShowGameOverUI()
    {
        //ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Always);

        //ImGui::Begin("GameOver", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        //ImVec2 windowSize = ImGui::GetWindowSize();
        //ImVec2 windowPos = ImVec2(
        //    (ImGui::GetIO().DisplaySize.x - windowSize.x) / 2, 
        //    (ImGui::GetIO().DisplaySize.y - windowSize.y) / 2
        //);
        //ImGui::SetWindowPos(windowPos);

        //ImGui::Text("Winner is Player %d!!", (playerID == 1) ? 2 : 1);
        ////ImGui::ColorButton("Profile", profileColor, ImGuiColorEditFlags_NoTooltip, ImVec2(50, 50));
        //if (ImGui::Button("Restart", ImVec2(150, 20)))
        //{
        //    
        //}
        //ImGui::End();

        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Always);

        ImGui::Begin("GameOver", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 windowPos = ImVec2(
            (ImGui::GetIO().DisplaySize.x - windowSize.x) / 2,
            (ImGui::GetIO().DisplaySize.y - windowSize.y) / 2
        );
        ImGui::SetWindowPos(windowPos);

        // 가운데 정렬 함수 (오른쪽으로 조금 더 이동)
        auto CenterItem = [](float itemWidth, float offset = 10.0f) { // 오른쪽 보정값 추가
            float windowWidth = ImGui::GetContentRegionAvail().x;
            ImGui::SetCursorPosX((windowWidth - itemWidth) * 0.5f + offset);
            };

        // Winner 텍스트 중앙 정렬 (우측 보정 추가)
        std::string winnerText = "Winner is Player " + std::to_string((playerID == 1) ? 2 : 1) + "!!";
        CenterItem(ImGui::CalcTextSize(winnerText.c_str()).x, 8.0f); // 10px 우측 이동
        ImGui::Text("%s", winnerText.c_str());

        // Restart 버튼 중앙 정렬 (우측 보정 추가)
        CenterItem(150, 8.0f); // 10px 우측 이동
        if (ImGui::Button("Restart", ImVec2(150, 20)))
        {
            // 리스타트 로직 추가
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

