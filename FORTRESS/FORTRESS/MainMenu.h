#pragma once
#include "GameObject.h"
class MainMenu: public GameObject {
	using Super = GameObject;
public:
	
	MainMenu(ID3D11Device* device, ID3D11DeviceContext* deviceContext) : Super(device, deviceContext) {};
	// GameObject��(��) ���� ��ӵ�
	void Update(double deltaTime) override;

private:
	int _playerCount;
};

