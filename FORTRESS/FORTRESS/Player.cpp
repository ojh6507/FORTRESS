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
	UpdateBoundingBox();
	if (!bIsGround) {
		if (!_parent) {
			velocity.y += gravityAcceleration * deltaTime;
			Move(FVector3(0.0f, velocity.y, 0.0f) * deltaTime);

		}
	}
	else
		velocity.y = 0.0f;

	// **�˹� (���� ���� ����)**
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


	
	if (Input::Instance()->IsMouseButtonDown(1))
		RotateZ(deltaTime);
	if (Input::Instance()->IsMouseButtonPressed(0)) {
		UpdateFirePoint();
		Fire(0, angle,dir, 500);
	}

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
	
	float minAngle, maxAngle;
	if (dir != -1) {
		minAngle = -2.0f;
		maxAngle = 45.0f;
	}
	else {
		minAngle = 900.f;
		maxAngle = 270.0f;
	}

	// ���� ������ �ִ밪 �̻��� ��� �ڵ����� ������
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

void Player::SetFirePoint(PlayerFirePoint* pfire)
{
	firePoint = pfire;
}

void Player::UpdateFirePoint()
{
	if (firePoint) {
		angle = firePoint->_parent->GetRotation().z;
		firePosition = firePoint->GetPosition();
	}
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
	if (_child) _child->UpdateOffset();
}

void PlayerHead::UpdateOffset()
{
	if (_parent) // �θ�� PlayerBody
	{
		FVector3 parentPos = _parent->GetPosition();
		float parentAngle = XMConvertToRadians(_parent->GetRotation().z);

		float cosA = cosf(parentAngle);
		float sinA = sinf(parentAngle);

		// ��ü �������� ȸ�� ��ȯ�� offset ����
		FVector3 rotatedOffset = {
			offset.x * cosA - offset.y * sinA,
			offset.x * sinA + offset.y * cosA,
			0
		};

		_tf.SetPosition(parentPos + rotatedOffset);
	}
}

void PlayerFirePoint::RotateZ(double deltaTime)
{
	if (_parent) {
		float parentAngle = _parent->GetRotation().z; // Barrel�� ���� ����
		_tf.SetRotation(FVector3(0, 0, parentAngle)); // FirePoint�� ȸ���� Barrel�� ����ȭ
	}
}

void PlayerFirePoint::UpdateOffset()
{
    if (_parent) {
        FVector3 parentPos = _parent->GetPosition();
        float parentAngle = XMConvertToRadians(_parent->GetRotation().z);

        float cosA = cosf(parentAngle);
        float sinA = sinf(parentAngle);

        // ���� offset ��ȯ
        FVector3 rotatedOffset = {
            offset.x * cosA - offset.y * sinA,
            offset.x * sinA + offset.y * cosA,
            0
        };

        // �Ǻ� ��ġ ���� (��: �߾ӿ��� ������ ������ �̵�)
        FVector3 pivotOffset = {10.0f, 0.0f, 0.0f};  // ���ϴ� �Ǻ� �̵� ��ġ
        FVector3 rotatedPivotOffset = {
            pivotOffset.x * cosA - pivotOffset.y * sinA,
            pivotOffset.x * sinA + pivotOffset.y * cosA,
            0
        };

        _tf.SetPosition(parentPos + rotatedOffset + rotatedPivotOffset);
    }
}
