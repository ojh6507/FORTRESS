#pragma once
class Transform {
public:
	FVector3 GetPosition() const { return _position; }
	void SetPosition(FVector3 position) { _position = position; }

	float GetRotation() const { return _rotation; }
	void SetRotation(float rotation) { _rotation = rotation; }

	FVector3 GetScale() const { return _scale; }
	void SetScale(FVector3 scale) { _scale = scale; }

	

private:
	FVector3 _position;
	float _rotation;
	FVector3 _scale;
};

