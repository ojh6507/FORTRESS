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

	// **넉백 물리 적용 (공기 저항 포함)**
	if (knockbackVelocity.MagnitudeSquared() > 0.01f) // 특정 속도 이하일 때 멈추기
	{
		Move(knockbackVelocity * deltaTime);
		knockbackVelocity *= pow(knockbackDamping, deltaTime * 60.0f);; // 감속 적용
	}
	else
	{
		knockbackVelocity = FVector3(0.0f, 0.0f, 0.0f); // 속도가 거의 0이면 멈춤
	}

	if (Input::Instance()->IsKeyDown(DIK_A))
		Move(FVector3(-50.0f, 0.0f, 0.0f) * deltaTime);
	else if (Input::Instance()->IsKeyDown(DIK_D))
		Move(FVector3(50.0f, 0.0f, 0.0f) * deltaTime);

	if (Input::Instance()->IsMouseButtonPressed(0))
		//RotateZ(30, deltaTime);
		TakeDamage(10, FVector3(-10.0f, 8.0f, 0.0f));
	if (Input::Instance()->IsMouseButtonDown(1))
		RotateZ(30, deltaTime);


}
