#pragma once
#include "GameObject.h"
#include "FVector3.h"

class MovableObject :
    public GameObject
{
public:
    FVector3 position;
    FVector3 rotation;
    FVector3 scale;

private:
    FVector3 velocity;

    bool bCanMove;
    bool bIsDead;
    float hp;
    float powerUpGage;

public:
    MovableObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
        : GameObject(device, deviceContext), velocity(0.0f, 0.0f, 0.0f),
        bCanMove(true), bIsDead(false), hp(100.0f), powerUpGage(0.0f) {}
    ~MovableObject();

    // Getter
    FVector3 GetVelocity() const { return velocity; }
    bool CanMove() const { return bCanMove; }
    bool IsDead() const { return bIsDead; }
    float GetHP() const { return hp; }
    float GetPowerUpGage() const { return powerUpGage; }

    // Setter
    void SetVelocity(const FVector3& newVelocity) { velocity = newVelocity; }
    void SetCanMove(bool canMove) { bCanMove = canMove; }
    void SetHP(float newHP);
    void SetPowerUpGage(float newGage) { powerUpGage = newGage; }

    // Method
    void Move(FVector3 direction);

private:
    void SetIsDead(bool isDead) { bIsDead = isDead; }
};

