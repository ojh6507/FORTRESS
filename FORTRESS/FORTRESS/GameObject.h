#pragma once
#include "stdafx.h"

class Player;

struct VS_CB_GAMEOBJECT_INFO {
	XMFLOAT4X4 m_xmf4x4World;
};

class GameObject {
public:
	GameObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~GameObject();

	virtual void Update(double deltaTime) abstract;
	virtual void Render();
	
	FVector3 GetPosition() {
		return _tf.GetPosition();
	}
	FVector3 GetScale() {
		return _tf.GetScale();
	}
	bool IntersectRay2D(const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDir, float& outDist)
	{
		UpdateBoundingBox();
		XMFLOAT3 modifiedRayOrigin = XMFLOAT3(rayOrigin.x, rayOrigin.y, 0.0f);
		XMFLOAT3 modifiedRayDir = XMFLOAT3(rayDir.x, rayDir.y, 0.0f);

		return _boundingBox.Intersects(XMLoadFloat3(&modifiedRayOrigin), XMLoadFloat3(&modifiedRayDir), outDist);
	}

	void UpdateBoundingBox();

	BoundingBox _boundingBox;
	BoundingBox _originBoundingBox;

protected:
	Transform _tf;
	VertexBuffer<FVertexSimple>* _vertexBuffer;
	IndexBuffer* _indexBuffer;

	std::vector<FVertexSimple> _vertices;
	std::vector<UINT32> _indices;
	
	ConstantBuffer<VS_CB_GAMEOBJECT_INFO>* _constantBuffer;

	
private:
	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceContext;

	InputLayout* _inputLayout;

	VertexShader* _vertexShader;
	PixelShader* _pixelShader;

	RasterizerState* _rasterizerState;
	SamplerState* _samplerState;
	

};

class _test_concrete_GameObject : public GameObject {
	using Super = GameObject;
public:
	_test_concrete_GameObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext) : Super(device, deviceContext) {};
	void Update(double deltaTime);
};
class CubeObject : public GameObject
{
private:
	FVector3 myColor;
public:
	CubeObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext, FVector3 scale, FVector3 color = FVector3(0,0,0));
	void Update(double deltaTime) {};

	FVector3 GetColor() const { return myColor; }
	void Setcolor(FVector3 newColor)
	{
		myColor = newColor;

		for (auto& vertex : _vertices)
		{
			vertex.r = myColor.x;
			vertex.g = myColor.y;
			vertex.b = myColor.z;
			vertex.a = 1.f;
		}
	}
	
};
class Projectile : public GameObject 
{
public:
	Projectile(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Update(double deltaTime);

	FVector3 _velocity = { 0.0f, 0.0f, 0.0f };
	FVector3 _position = { 0.0f, 0.0f, 0.0f };
	FVector3 _acceleration = { -10.1f, -150.8f, 0.0f };
	bool _isFired = false;
	bool _isOutOfScreen = false;

	BoundingSphere collisionBound;
	float basicRadius;

	Player* motherPlayer;

	void OutOfScreen() { _tf.SetPosition(FVector3(999999, 999999, 0)); }


	void FireProjectile(Player& player, FVector3& firePosition, float angle, float speed) {
		_isFired = true;
		motherPlayer = &player;

		// 초기 발사 위치 (예제: 현재 오브젝트 위치)
		if (!&_tf) return;
		_tf.SetPosition(firePosition);
		_position = _tf.GetPosition();

		_velocity.x = speed * cos(XMConvertToRadians(angle)); // X 방향 속도
		_velocity.y = speed * sin(XMConvertToRadians(angle)); // Y 방향 속도
	}

	void UpdateProjectile(double deltaTime) {
		_velocity.x +=  _acceleration.x * deltaTime;
		_velocity.y +=  _acceleration.y * deltaTime;

		_position.x += _velocity.x * deltaTime + 0.5f * _acceleration.x * deltaTime;
		_position.y += _velocity.y * deltaTime + 0.5f * _acceleration.y * deltaTime;

		_tf.SetPosition(_position);
	}
};

class ObjObject : public GameObject
{
public:
	ObjObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::wstring& filepath);

	void Update(double deltaTime);

	void SetPostion(FVector3 posistion) {
		_tf.SetPosition(posistion);
	}
	void SetRotation(FVector3 rotation) {
		_tf.SetRotation(rotation);
	}
	void SetScale(FVector3 scale) {
		_tf.SetScale(scale);
	}
};