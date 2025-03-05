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

    void Update(double deltaTime);
};

