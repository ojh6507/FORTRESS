#pragma once
#include "GameObject.h"
class MainMenu: public GameObject {
	using Super = GameObject;
public:
	
	MainMenu(ID3D11Device* device, ID3D11DeviceContext* deviceContext) : Super(device, deviceContext) {};
	// GameObject을(를) 통해 상속됨
	void Update(double deltaTime) override;

private:
	int _playerCount;
};

