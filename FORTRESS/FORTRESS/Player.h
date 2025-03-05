#pragma once
#include "GameObject.h"
#include "FVector3.h"

class Projectile;
class PlayerFirePoint;

class Player :
    public CubeObject
{
private:
    FVector3 velocity;

    FVector3 knockbackVelocity = FVector3(0.0f, 0.0f, 0.0f);
    float knockbackDamping = 0.9f;

    Projectile* projectile;
    PlayerFirePoint* firePoint;
    bool isMoveMode;
    bool bIsDead;
    bool bIsGround;
    const float gravityAcceleration = -80.0f;
    float hp;
    float powerUpGage;
    void SetIsDead(bool isDead) { bIsDead = isDead; }

public:
    Player(ID3D11Device* device, ID3D11DeviceContext* deviceContext, FVector3 scale, FVector3 color)
        : CubeObject(device, deviceContext, scale, color), velocity(0.0f, 0.0f, 0.0f),
        isMoveMode(true), bIsDead(false), hp(100), powerUpGage(0) {}

    virtual ~Player() {
        if (_child) delete _child;
    }

    // Getter
    FVector3 GetVelocity() const { return velocity; }
    bool IsMoveMode() const { return isMoveMode; }
    bool IsDead() const { return bIsDead; }
    float GetHP() const { return hp; }
    float GetPowerUpGage() const { return powerUpGage; }
    FVector3 GetFirePosition() const
    {
        return _tf.GetPosition() + FVector3(0, 20 * _tf.GetScale().y, 0);
    }

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
    virtual void Render() override;
    virtual void Update(double deltaTime);
    void ComputeIsGround();

    void Move(FVector3 velocity);

    virtual void RotateZ(double deltaTime = 1);
    void Fire(int projectileType, float angle, float power);
    void SuccessHitEnemy();
    void TakeDamage(float damage, FVector3 knockbackDirection);
    void SetChild(Player* child) {
        _child = child;
    }
    void SetFirePoint(PlayerFirePoint* pfire);
    void UpdateFirePoint();
  
    void SetParent(Player* parent) {
        _parent = parent;
    }
    virtual void UpdateOffset() {};
    FVector3 GetRotation() {
        return _tf.GetRotation();
    }
    void Reload(Projectile* newProjectTile) {
        projectile = newProjectTile;
    }
    void SetPosition(const FVector3& pos) {
    
        _tf.SetPosition(pos);
        if (_child) _child->SetPosition(pos);
    };
    virtual void SetDir(short d) {
        dir = d;
        if (_child) {
            _child->SetDir(d);
            _child->UpdateOffset();
        }
    }
    Player* _parent;
protected:
    FVector3 firePosition;
    float angle;
    Player* _child;
    float anglePerSecond = 10;
    short dir = 1;
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
            return;
        }
    }
}

inline void Player::Move(FVector3 velocity)
{
    if (isMoveMode)
        _tf.SetPosition(_tf.GetPosition() + velocity);

}

inline void Player::Fire(int projectileType,float angle, float power)
{
    // 발사체 생성
    if(projectile)
        projectile->FireProjectile(firePosition, angle, power);
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
    knockbackDirection = knockbackDirection.Normalized();
    knockbackVelocity = knockbackDirection * 50.0f * damage; // 넉백 초기 속도 (크기 조정 가능)
}



class PlayerBarrel : public Player {
private:
    FVector3 offset;
public:
    PlayerBarrel(ID3D11Device* device, ID3D11DeviceContext* deviceContext, FVector3 scale, FVector3 relativeOffset, FVector3 color)
        : Player(device, deviceContext, scale, color), offset{ relativeOffset } {
    }
    virtual ~PlayerBarrel() {
        if (_child) delete _child;
    }
    virtual void RotateZ(double deltaTime = 1);
    virtual void UpdateOffset();
};

class PlayerFirePoint : public Player {
private:
    FVector3 offset;
public:
    PlayerFirePoint(ID3D11Device* device, ID3D11DeviceContext* deviceContext, FVector3 scale, FVector3 relativeOffset, FVector3 color)
        : Player(device, deviceContext, scale, color), offset{ relativeOffset } {
    }
    virtual ~PlayerFirePoint() {
        if (_child) delete _child;
    }
    virtual void RotateZ(double deltaTime = 1);
    virtual void UpdateOffset();
};

class PlayerHead : public Player {
private:
    FVector3 offset;
public:
    PlayerHead(ID3D11Device* device, ID3D11DeviceContext* deviceContext, FVector3 scale, FVector3 relativeOffset, FVector3 color)
        : Player(device, deviceContext, scale, color), offset{ relativeOffset } {
    }
    virtual ~PlayerHead() {
        if (_child) delete _child;
    }
    virtual void UpdateOffset();
    virtual void RotateZ(double deltaTime = 1);
};
