#pragma once
class Transform {
public:
	FVector3 GetPosition() { return _position; }
	void SetPosition(FVector3 position) { _position = position; }

private:
	FVector3 _position;
};

