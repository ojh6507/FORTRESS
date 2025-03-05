#pragma once
#include "Player.h"

void Player::Update(double deltaTime)
{
	bIsGround = false;
	ComputeIsGround();

	if (!bIsGround)
	{
		velocity.y += gravityAcceleration * deltaTime;
		Move(FVector3(0.0f, velocity.y, 0.0f) * deltaTime);
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

	if (Input::Instance()->IsKeyDown(DIK_W))
		RotateZ(30, deltaTime);
	else if (Input::Instance()->IsKeyDown(DIK_S))
		RotateZ(-30, deltaTime);

	if (Input::Instance()->IsMouseButtonPressed(0))
		Fire(0, _tf.GetRotation().z, 500);

	if (Input::Instance()->IsMouseButtonDown(1))
		TakeDamage(10, FVector3(-10.0f, 8.0f, 0.0f));


}
