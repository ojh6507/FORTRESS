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
	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Always);
	ImGui::Begin("status", nullptr,
		ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 windowPos = ImVec2((ImGui::GetIO().DisplaySize.x- windowSize.x) / 2, 10);
	ImGui::SetWindowPos(windowPos);
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
	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Always);
	ImGui::Begin("status", nullptr,
		ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 windowPos = ImVec2((ImGui::GetIO().DisplaySize.x - windowSize.x) / 2, 10);
	ImGui::SetWindowPos(windowPos);

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
	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Always);
	ImGui::Begin("status", nullptr,
		ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 windowPos = ImVec2((ImGui::GetIO().DisplaySize.x - windowSize.x) / 2, 10);
	ImGui::SetWindowPos(windowPos);
	ImGui::Text("Waiting");
	ImGui::End();

	if (_context->GetTimerTime() > 3000) {
		_turnedPlayerIdx = (_turnedPlayerIdx + 1) % _context->players.size();
		_context->StopTimer();
		_context->ChangeState(new MoveAndShotState(_context, _turnedPlayerIdx));
	}
}

void GameOverState::Reserve() {
	_context->StartTimer();
}

void GameOverState::Update()
{
	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Always);
	ImGui::Begin("status", nullptr,
		ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 windowPos = ImVec2((ImGui::GetIO().DisplaySize.x - windowSize.x) / 2, 10);
	ImGui::SetWindowPos(windowPos);
	ImGui::Text("GameOver");
	
	char s[256];
	sprintf_s(s, 256, "player%d Win", 0);
	ImGui::Text(s);
	ImGui::End();
}
