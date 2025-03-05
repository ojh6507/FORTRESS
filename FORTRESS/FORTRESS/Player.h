#pragma once
#include "GameObject.h"
#include "FVector3.h"

class Player :
    public GameObject
{
private:
    FVector3 velocity;

    bool isMoveMode;
    bool bIsDead;
    bool bIsGround;
    const float gravityAcceleration = -40.0f;
    float hp;
    float powerUpGage;

    void SetIsDead(bool isDead) { bIsDead = isDead; }

public:
    Player(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
        : GameObject(device, deviceContext), velocity(0.0f, 0.0f, 0.0f),
        isMoveMode(true), bIsDead(false), hp(100), powerUpGage(0) {}
    ~Player() {}

    // Getter
    FVector3 GetVelocity() const { return velocity; }
    bool IsMoveMode() const { return isMoveMode; }
    bool IsDead() const { return bIsDead; }
    float GetHP() const { return hp; }
    float GetPowerUpGage() const { return powerUpGage; }

    // Setter
    void SetVelocity(const FVector3& newVelocity) { velocity = newVelocity; }
    void SetMoveMode(bool canMove) { isMoveMode = canMove; }
    void SetHP(float newHP)
    {
        hp = (newHP <= 0) ? 0 : newHP;
        if (hp == 0)
            SetIsDead(true);
    }
    void SetPowerUpGage(float addGage) 
    { 
        powerUpGage += addGage;
        if (powerUpGage > 100.0f)
            powerUpGage = 100.0f;
    }

    // Method
    void Update(double deltaTime);

    void ComputeIsGround();

    void Move(FVector3 velocity, double deltaTime = 1);
    void RotateZ(float anglePerSecond, double deltaTime = 1);
    void Fire(int projectileType, FVector3 direction, float power);
    void SuccessHitEnemy();
    void TakeDamage(float damage, FVector3 knockbackDirection);
};

inline void Player::ComputeIsGround()
{
    for (const auto& vertex : _vertices) 
    {
        FVector3 vertexLocalPosition = FVector3(vertex.x, vertex.y, vertex.z);
        //float magnitude = vertexLocalPosition.Magnitude();
        //if (magnitude * sin(_tf.GetRotation().z) + _tf.GetPosition().y < -FRAME_BUFFER_HEIGHT / 2)
        if ((vertexLocalPosition.y * _tf.GetScale().y) + _tf.GetPosition().y < - FRAME_BUFFER_HEIGHT / 2)
        {
            bIsGround = true;
            break;
        }
    }
}

inline void Player::Move(FVector3 velocity, double deltaTime)
{
    if (isMoveMode)
        _tf.SetPosition(_tf.GetPosition() + velocity * deltaTime);
}

inline void Player::RotateZ(float anglePerSecond, double deltaTime)
{
    _tf.SetRotation(_tf.GetRotation() + FVector3(0, 0, anglePerSecond * deltaTime));
}

inline void Player::Fire(int projectileType, FVector3 direction, float power)
{
    // 발사체 생성
    // 발사체 한테 자기자신 전달, 발사체가 적을 맞췄는지 확인후 자신을 발사한 Player에게 결과 전달
}

inline void Player::SuccessHitEnemy()
{
    SetPowerUpGage(20.0f);
}

inline void Player::TakeDamage(float damage, FVector3 knockbackDirection)
{
    if (bIsDead)
        return;

    SetHP(GetHP() - damage);

    // 넉백
    knockbackDirection += FVector3(0, 10, 0);
    knockbackDirection.Normalized();
    Move(knockbackDirection.Normalized() * 10.0f);
}

