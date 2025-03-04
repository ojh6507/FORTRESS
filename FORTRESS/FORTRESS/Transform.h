#pragma once
class Transform {
public:
	FVector3 GetPosition() { return _position; }
	void SetPosition(FVector3 position) { _position = position; }

	float GetRotation() { return _rotation; }
	void SetRotation(float rotation) { _rotation = rotation; }

	FVector3 GetScale() { return _scale; }
	void SetScale(FVector3 scale) { _scale = scale; }

private:
	FVector3 _position;
	float _rotation;
	FVector3 _scale;
};

