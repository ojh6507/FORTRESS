#include "stdafx.h"
#include "MainMenu.h"

extern double gGameElapsedTime;

void MainMenu::Update(double deltaTime)
{
	ImGui::Begin("asdf");
	ImGui::Text(std::to_string(gGameElapsedTime).c_str());
	ImGui::InputInt("Player Numbers", &_playerCount, 1, 1);
	if (ImGui::Button("Start")) {
		// start game
	}
	if (_playerCount < 2) _playerCount = 2;
	if (_playerCount > 4) _playerCount = 4;
	ImGui::End();
}
