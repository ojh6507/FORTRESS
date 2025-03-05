#include "stdafx.h"
#include "IngameManager.h"

IngameManager::IngameManager() : GameObject(nullptr, nullptr) {
	_state = new InitReadyState(this, nullptr);
	_state->Reserve();
}

void IngameManager::Update(double deltaTime) {
	
}


void IngameManager::ChangeState(IngameState* state) {
	_state = state;
	_state->Reserve();
}


IngameState::IngameState(IngameManager* context, GameObject* turnedPlayer) {
	_context = context;
	_playerHasTurn = turnedPlayer;
}


InitReadyState::InitReadyState(IngameManager* context, GameObject* turnedPlayer): IngameState(context, turnedPlayer) {
}

void InitReadyState::Reserve() {
	setTimeout([this]() {
		_context->ChangeState(new MoveAndShotState(_context, _playerHasTurn));
	}, 5000);
}


void MoveAndShotState::Reserve() {
	// player who has turn moves and shots
	_context->ChangeState(new WaitingAfterShotState(_context, _playerHasTurn));
}


void WaitingAfterShotState::Reserve() {
	OutputDebugString(L"triggered\n");
	// 
	// _context->ChangeState(new Game::State::WaitingAfterShotState(_context, _nextPlayer));
}


void GameOverState::Reserve() {
	//
}
