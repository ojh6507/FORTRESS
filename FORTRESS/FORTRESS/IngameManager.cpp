#include "stdafx.h"
#include "IngameManager.h"

const double IngameManager::TURNTIME = 10.0;
IngameManager* IngameManager::_instance = nullptr;
extern double gGameElapsedTime;

IngameManager::IngameManager() : GameObject(nullptr, nullptr), _startTimerTime(-1), _timerState(false) {
	_state = new InitReadyState(this, 0);
	_state->Reserve();
}

IngameManager::~IngameManager() {
	_instance = nullptr;
	delete _state;
}

void IngameManager::Update(double deltaTime) {
	_state->Update();
}

void IngameManager::ChangeState(IngameState* state) {
	delete _state;
	_state = state;
	_state->Reserve();
}

void IngameManager::StartTimer() {
	_startTimerTime = gGameElapsedTime;
	_timerState = true;
}

void IngameManager::StopTimer() {
	_startTimerTime = -1;
	_timerState = false;
}

double IngameManager::GetTimerTime() {
	if (_timerState)
		return gGameElapsedTime - _startTimerTime;
	else
		return -1;
}




IngameState::IngameState(IngameManager* context, int turnedPlayer) {
	_context = context;
	_turnedPlayerIdx = turnedPlayer;
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
		_context->StopTimer();
		_context->ChangeState(new MoveAndShotState(_context, _turnedPlayerIdx));
	}
}




void MoveAndShotState::Reserve() {
	_context->StartTimer();
	_context->players[_turnedPlayerIdx]->SetMoveMode(true);
}

void MoveAndShotState::Update()
{
	ImGui::Begin("test");
	ImGui::Text("Move&Shot");
	ImGui::Text((std::to_string(_context->GetTimerTime() / 1000.0) + "s").c_str());
	ImGui::End();

	if (_context->GetTimerTime() > IngameManager::TURNTIME * 1000.0 ||
		!_context->players[_turnedPlayerIdx]->IsMoveMode()
		) {
		_context->StopTimer();
		_context->ChangeState(new WaitingAfterShotState(_context, _turnedPlayerIdx));
	}
}




void WaitingAfterShotState::Reserve() {
	_context->StartTimer();
	_context->players[_turnedPlayerIdx]->SetMoveMode(false);
}

void WaitingAfterShotState::Update()
{
	ImGui::Begin("test");
	ImGui::Text("Waiting");
	ImGui::End();

	if (_context->GetTimerTime() > 3000) {
		_turnedPlayerIdx = (_turnedPlayerIdx + 1) % _context->players.size();
		_context->StopTimer();
		_context->ChangeState(new MoveAndShotState(_context, _turnedPlayerIdx));
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
