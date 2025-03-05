#pragma once
#include "Player.h"

void Player::Update(double deltaTime)
{
	bIsGround = false;
	ComputeIsGround();

	if (!bIsGround)
	{
		velocity.y += gravityAcceleration * deltaTime;
		Move(FVector3(0.0f, velocity.y, 0.0f), deltaTime);
	}
	else
		velocity.y = 0.0f;

	if (Input::Instance()->IsKeyDown(DIK_A))
		Move(FVector3(-50.0f, 0.0f, 0.0f), deltaTime);
	if (Input::Instance()->IsKeyDown(DIK_D))
		Move(FVector3(50.0f, 0.0f, 0.0f), deltaTime);

	if (Input::Instance()->IsMouseButtonPressed(0))
		//RotateZ(30, deltaTime);
		TakeDamage(10, FVector3(-10.0f, 0.0f, 0.0f));
	if (Input::Instance()->IsMouseButtonDown(1))
		RotateZ(-30, deltaTime);


}
