#pragma once
#include "GameObject.h"

class IngameState;
class IngameManager : public GameObject {
	using Super = GameObject;
public:
	IngameManager();
	// GameObject을(를) 통해 상속됨
	void Update(double deltaTime) override;

	void ChangeState(IngameState* state);

	std::vector<GameObject> players;
private:
	IngameState* _state;
};

class IngameState {
public:
	IngameState(IngameManager* context, GameObject* turnedPlayer);
	virtual ~IngameState() {};
	virtual void Reserve() abstract;

protected:
	IngameManager* _context;
	GameObject* _playerHasTurn;
};

class InitReadyState: public IngameState {
	using Super = IngameState;
public:
	InitReadyState(IngameManager* context, GameObject* turnedPlayer);
	void Reserve() override;
};

class MoveAndShotState : public IngameState {
	using Super = IngameState;
public:
	MoveAndShotState(IngameManager* context, GameObject* turnedPlayer) : Super(context, turnedPlayer) {};
	void Reserve() override;
};

class WaitingAfterShotState : public IngameState {
	using Super = IngameState;
public:
	WaitingAfterShotState(IngameManager* context, GameObject* turnedPlayer) : Super(context, turnedPlayer) {};
	void Reserve() override;
};

class GameOverState : public IngameState {
	using Super = IngameState;
public:
	GameOverState(IngameManager* context, GameObject* turnedPlayer) : Super(context, turnedPlayer) {};
	void Reserve() override;
};

