#pragma once
#include "Player.h"

class IngameState;
class IngameManager : public GameObject {
	using Super = GameObject;
	static IngameManager* _instance;
	IngameManager();
	IngameManager(IngameManager& other): GameObject(nullptr, nullptr) {};
	IngameManager& operator=(IngameManager& other) {};
	~IngameManager();
public:
	static const double TURNTIME;
	static IngameManager* Instance() {
		if (!_instance)
			_instance = new IngameManager();
		return _instance;
	}
	
	void Update(double deltaTime) override;
	void ChangeState(IngameState* state);

	std::vector<Player*> players;

	void StartTimer();
	void StopTimer();
	double GetTimerTime();
private:
	IngameState* _state;
	double _startTimerTime;
	bool _timerState;
	
};

class IngameState {
public:
	IngameState(IngameManager* context, Player* turnedPlayer);
	virtual ~IngameState() {};
	virtual void Reserve() abstract;
	virtual void Update() abstract;

protected:
	IngameManager* _context;
	Player* _playerHasTurn;
};


class InitReadyState: public IngameState {
	using Super = IngameState;
public:
	InitReadyState(IngameManager* context, Player* turnedPlayer) : Super(context, turnedPlayer) {};
	void Reserve() override;
	void Update() override;
};

class MoveAndShotState : public IngameState {
	using Super = IngameState;
public:
	MoveAndShotState(IngameManager* context, Player* turnedPlayer) : Super(context, turnedPlayer) {};
	void Reserve() override;
	void Update() override;
};

class WaitingAfterShotState : public IngameState {
	using Super = IngameState;
public:
	WaitingAfterShotState(IngameManager* context, Player* turnedPlayer) : Super(context, turnedPlayer) {};
	void Reserve() override;
	void Update() override;
};

class GameOverState : public IngameState {
	using Super = IngameState;
public:
	GameOverState(IngameManager* context, Player* turnedPlayer) : Super(context, turnedPlayer) {};
	void Reserve() override;
	void Update() override;
};

