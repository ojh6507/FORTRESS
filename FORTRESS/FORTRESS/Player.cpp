#pragma once
#include "Player.h"

void Player::Render()
{
	GameObject::Render();
	if (_child) _child->Render();
}

void Player::Update(double deltaTime)
{
	bIsGround = false;
	ComputeIsGround();

	if (!bIsGround) {
		if (!_parent) {
			velocity.y += gravityAcceleration * deltaTime;
			Move(FVector3(0.0f, velocity.y, 0.0f) * deltaTime);

		}
	}
	else
		velocity.y = 0.0f;

	// **넉백 (공기 저항 포함)**
	if (knockbackVelocity.MagnitudeSquared() > 0.01f)
	{
		Move(knockbackVelocity * deltaTime);
		knockbackVelocity *= pow(knockbackDamping, deltaTime * 60.0f);;
	}
	else
	{
		knockbackVelocity = FVector3(0.0f, 0.0f, 0.0f);
	}

	// **Player input**
	if (Input::Instance()->IsKeyDown(DIK_A))
		Move(FVector3(-50.0f, 0.0f, 0.0f) * deltaTime);
	else if (Input::Instance()->IsKeyDown(DIK_D))
		Move(FVector3(50.0f, 0.0f, 0.0f) * deltaTime);


	if (Input::Instance()->IsMouseButtonPressed(0)) {
		RotateZ(deltaTime);
	}
	if (Input::Instance()->IsMouseButtonDown(1))
		RotateZ(deltaTime);
	if (Input::Instance()->IsMouseButtonPressed(0))
		Fire(0, _tf.GetRotation().z, 500);

	if (Input::Instance()->IsMouseButtonDown(1))
		TakeDamage(10, FVector3(-10.0f, 8.0f, 0.0f));


	if (_child) {
		_child->UpdateOffset();
		_child->Update(deltaTime);
	}
}
void Player::RotateZ( double deltaTime)
{
	if (_child) _child->RotateZ( deltaTime);
}

void PlayerHead::RotateZ(double deltaTime)
{
	
	if (_child) _child->RotateZ( deltaTime);
}

void PlayerBarrel::RotateZ(double deltaTime)
{
	float currentAngle = _tf.GetRotation().z;
	float angleChange = anglePerSecond * deltaTime;
	float newAngle = currentAngle + angleChange;

	float headAngle = _parent->GetRotation().z;
	bool isFacingRight = cos(XMConvertToRadians(headAngle)) > 0;

	float minAngle, maxAngle;
	if (isFacingRight) {
		minAngle = -2.0f;
		maxAngle = 45.0f;
	}
	else {
		minAngle = 135.0f;
		maxAngle = 225.0f;
	}

	// 현재 각도가 최대값 이상일 경우 자동으로 내려감
	if (newAngle > maxAngle) {
		newAngle = maxAngle;
		anglePerSecond = -fabs(anglePerSecond);
	}
	if (newAngle < minAngle) {
		newAngle = minAngle;
		anglePerSecond = fabs(anglePerSecond);
	}
	_tf.SetRotation(FVector3(0, 0, newAngle));
	UpdateOffset();
	if (_child) _child->RotateZ(deltaTime);
}


void PlayerBarrel::UpdateOffset()
{
	if (_parent) {
		FVector3 parentPos = _parent->GetPosition();
		float parentAngle = XMConvertToRadians(_parent->GetRotation().z);

		float cosA = cosf(parentAngle);
		float sinA = sinf(parentAngle);

		FVector3 rotatedOffset = {
			offset.x * cosA - offset.y * sinA,
			offset.x * sinA + offset.y * cosA,
			0
		};

		_tf.SetPosition(parentPos + rotatedOffset);
	}
}

void PlayerHead::UpdateOffset()
{
	if (_parent) // 부모는 PlayerBody
	{
		FVector3 parentPos = _parent->GetPosition();
		float parentAngle = XMConvertToRadians(_parent->GetRotation().z);

		float cosA = cosf(parentAngle);
		float sinA = sinf(parentAngle);

		// 차체 기준으로 회전 변환된 offset 적용
		FVector3 rotatedOffset = {
			offset.x * cosA - offset.y * sinA,
			offset.x * sinA + offset.y * cosA,
			0
		};

		_tf.SetPosition(parentPos + rotatedOffset);
	}
}
