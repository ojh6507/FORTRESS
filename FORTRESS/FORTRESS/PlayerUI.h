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

    // 생성자
    PlayerUI(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float scale, 
        int id, Player* p, ImVec4 color)
        : CubeObject(device, deviceContext, scale), 
        playerID(id), player(p), profileColor(color) { }

    // UI 표시 함수
    void ShowPlayerUI()
    {
        ImGui::Begin(playerID == 1 ? "Player 1" : "Player 2");

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
//    // 생성자 (플레이어 UI 초기화)
//    GameUI(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float scale)
//        :CubeObject(device, deviceContext, scale) { }
//
//        //player1(1, 0.8f, 0.5f, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)), // 빨간색
//        //player2(2, 0.6f, 0.3f, ImVec4(0.0f, 0.0f, 1.0f, 1.0f))  // 파란색
//
//    // 전체 UI 표시 함수
//    void ShowGameUI()
//    {
//        //player1.ShowPlayerUI();
//        //player2.ShowPlayerUI();
//    }
//};

