#pragma once
#include "stdafx.h"
struct VS_CB_GAMEOBJECT_INFO {
	XMFLOAT4X4 m_xmf4x4World;
};

class GameObject {
public:
	GameObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~GameObject();

	virtual void Update(double deltaTime) abstract;
	virtual void Render();
	
	FVector3 GetPosition() {
		return _tf.GetPosition();
	}
	FVector3 GetScale() {
		return _tf.GetScale();
	}
	bool IntersectRay(const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDir, float& dist) {
		return _boundingBox.Intersects(XMLoadFloat3(&rayOrigin), XMLoadFloat3(&rayDir), dist);
	}
	void UpdateBoundingBox();

	XMFLOAT4X4 GetWorldMatrix(GameObject* parent = nullptr) {
		XMMATRIX scaleMatrix = XMMatrixScaling(_tf.GetScale().x, _tf.GetScale().y, _tf.GetScale().z);
		XMMATRIX rotationX = XMMatrixRotationX(XMConvertToRadians(_tf.GetRotation().x));
		XMMATRIX rotationY = XMMatrixRotationY(XMConvertToRadians(_tf.GetRotation().y));
		XMMATRIX rotationZ = XMMatrixRotationZ(XMConvertToRadians(_tf.GetRotation().z));
		XMMATRIX translationMatrix = XMMatrixTranslation(_tf.GetPosition().x, _tf.GetPosition().y, _tf.GetPosition().z);

		XMMATRIX localMatrix = scaleMatrix * rotationX * rotationY * rotationZ * translationMatrix;
		if (parent) {
			XMFLOAT4X4 parentMatrix = parent->GetWorldMatrix();
			XMMATRIX parentWorld = XMLoadFloat4x4(&parentMatrix);
			localMatrix = XMMatrixMultiply(localMatrix, parentWorld);
		}
		if (child) {

		}

		localMatrix = XMMatrixTranspose(localMatrix);

		XMFLOAT4X4 worldMatrixFloat4x4;
		XMStoreFloat4x4(&worldMatrixFloat4x4, localMatrix);
		return worldMatrixFloat4x4;
	}


protected:
	Transform _tf;
	VertexBuffer<FVertexSimple>* _vertexBuffer;
	IndexBuffer* _indexBuffer;

	std::vector<FVertexSimple> _vertices;
	std::vector<UINT32> _indices;
	
	ConstantBuffer<VS_CB_GAMEOBJECT_INFO>* _constantBuffer;
	GameObject* child;
	BoundingBox _boundingBox;
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
public:
	CubeObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Update(double deltaTime) {};
	
};
class Projectile : public GameObject 
{
public:
	Projectile(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Update(double deltaTime);

	FVector3 _velocity = { 0.0f, 0.0f, 0.0f };
	FVector3 _position = { 0.0f, 0.0f, 0.0f };
	FVector3 _acceleration = { -0.1f, -90.8f, 0.0f };
	bool _isFired = false;
	
	void FireProjectile() {
		_isFired = true;

		// 초기 발사 위치 (예제: 현재 오브젝트 위치)
		_position = _tf.GetPosition();

		// 초기 발사 속도와 각도 설정
		float speed = 1000.f;  // 발사 속도
		float angle = 85.0f;  // 발사 각도 (도 단위)

		_velocity.x = speed * cos(XMConvertToRadians(angle)); // X 방향 속도
		_velocity.y = speed * sin(XMConvertToRadians(angle)); // Y 방향 속도
	}

	void UpdateProjectile(double deltaTime) {
		_velocity.x +=  _acceleration.x * deltaTime;
		_velocity.y +=  _acceleration.y * deltaTime;

		_position.x += _velocity.x * deltaTime + 0.5f * _acceleration.x * deltaTime * deltaTime;
		_position.y += _velocity.y * deltaTime + 0.5f * _acceleration.y * deltaTime * deltaTime;

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