#include "stdafx.h"
#include "IngameManager.h"

const double IngameManager::TURNTIME = 10.0;
extern double gGameElapsedTime;

IngameManager::IngameManager() : GameObject(nullptr, nullptr) {
	_state = new InitReadyState(this, nullptr);
	_state->Reserve();
}

void IngameManager::Update(double deltaTime) {
	
	_state->Update();
}

void IngameManager::ChangeState(IngameState* state) {
	_state = state;
	_state->Reserve();
}

void IngameManager::StartTimer() {
	_startTimerTime = gGameElapsedTime;
}

double IngameManager::GetTimerTime() {
	return gGameElapsedTime - _startTimerTime;
}




IngameState::IngameState(IngameManager* context, GameObject* turnedPlayer) {
	_context = context;
	_playerHasTurn = turnedPlayer;
}




void InitReadyState::Reserve() {
	_context->StartTimer();
	
}

void InitReadyState::Update()
{
	ImGui::Begin("test");
	ImGui::Text("Ready");
	ImGui::End();

	if (_context->GetTimerTime() > 5000) {
		_context->ChangeState(new MoveAndShotState(_context, _playerHasTurn));
	}
}




void MoveAndShotState::Reserve() {
	_context->StartTimer();
}

void MoveAndShotState::Update()
{
	ImGui::Begin("test");
	ImGui::Text("Move&Shot");
	ImGui::Text((std::to_string(_context->GetTimerTime() / 1000.0) + "s").c_str());
	ImGui::End();

	if (_context->GetTimerTime() > IngameManager::TURNTIME * 1000.0) {
		_context->ChangeState(new WaitingAfterShotState(_context, _playerHasTurn));
	}
}




void WaitingAfterShotState::Reserve() {
	_context->StartTimer();
}

void WaitingAfterShotState::Update()
{
	ImGui::Begin("test");
	ImGui::Text("Waiting");
	ImGui::End();

	if (_context->GetTimerTime() > 3000) {
		_context->ChangeState(new MoveAndShotState(_context, _playerHasTurn));
	}
}




void GameOverState::Reserve() {
	//
}

void GameOverState::Update()
{
	ImGui::Begin("test");
	ImGui::Text("GameOver");
	ImGui::End();
}
