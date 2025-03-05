#pragma once
#include "GameObject.h"
#include "Player.h"
#include "imgui.h"
#include <iostream>


class PlayerUI : public CubeObject
{
public:
    Player* player;
    int playerID;
    float hp;
    float powerUp;
    ImVec4 profileColor;

    // ������
    PlayerUI(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float scale, 
        int id, Player* p)
        : CubeObject(device, deviceContext, scale), 
        playerID(id), player(p) 
    { 
        profileColor = ImVec4(player->GetColor().x, player->GetColor().y, player->GetColor().z, 1.0f);
    }

    // UI ǥ�� �Լ�
    void ShowPlayerUI()
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX));

        ImGui::Begin(playerID == 1 ? "Player 1" : "Player 2", nullptr,
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

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

    void Update(double deltaTime) 
    {
        ShowPlayerUI();
    }
};

//class GameUI : public CubeObject
//{
//public:
//    //PlayerUI* player1;
//    // 
//    //PlayerUI* player2;
//
//    // ������ (�÷��̾� UI �ʱ�ȭ)
//    GameUI(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float scale)
//        :CubeObject(device, deviceContext, scale) { }
//
//        //player1(1, 0.8f, 0.5f, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), // ������
//        //player2(2, 0.6f, 0.3f, ImVec4(0.0f, 0.0f, 1.0f, 1.0f))  // �Ķ���
//
//    // ��ü UI ǥ�� �Լ�
//    void ShowGameUI()
//    {
//        //player1.ShowPlayerUI();
//        //player2.ShowPlayerUI();
//    }
//};

