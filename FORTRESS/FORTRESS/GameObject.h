#pragma once
#include "stdafx.h"
#include <DirectXCollision.h>

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
		float distance = FLT_MAX;
		XMVECTOR rayOriginVec = XMLoadFloat3(&rayOrigin);
		XMVECTOR rayDirVec = XMVector3Normalize(XMLoadFloat3(&rayDir));

		// **Z 좌표를 무시하고 2D 좌표만 사용**
		XMFLOAT2 rayOrigin2D = XMFLOAT2(rayOrigin.x, rayOrigin.y);
		XMFLOAT2 obbCenter2D = XMFLOAT2(_boundingBox.Center.x, _boundingBox.Center.y);
		XMFLOAT2 obbExtents2D = XMFLOAT2(_boundingBox.Extents.x, _boundingBox.Extents.y);

		
		if ((abs(rayOrigin2D.x - obbCenter2D.x) <= obbExtents2D.x) &&
			(abs(rayOrigin2D.y - obbCenter2D.y) <= obbExtents2D.y)) {
			outDist = 0;
			return true;
		}

		// **OBB와의 충돌 검사**
		BoundingOrientedBox transformedBox;
		_boundingBox.Transform(transformedBox, XMMatrixIdentity()); // 변환 적용

		// **2D에서 X, Y만 비교하여 충돌 검사**
		if ((abs(rayOrigin2D.x - transformedBox.Center.x) <= transformedBox.Extents.x) &&
			(abs(rayOrigin2D.y - transformedBox.Center.y) <= transformedBox.Extents.y)) {
			outDist = 0;
			return true;
		}

		return false;
	}
	void UpdateBoundingBox();


protected:
	Transform _tf;
	VertexBuffer<FVertexSimple>* _vertexBuffer;
	IndexBuffer* _indexBuffer;

	std::vector<FVertexSimple> _vertices;
	std::vector<UINT32> _indices;
	
	ConstantBuffer<VS_CB_GAMEOBJECT_INFO>* _constantBuffer;

	BoundingOrientedBox _boundingBox;
	BoundingOrientedBox _originBoundingBox;
	
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

	void OutOfScreen() { _tf.SetPosition(FVector3(FRAME_BUFFER_WIDTH * 2, FRAME_BUFFER_HEIGHT * 2, 0)); }

	void FireProjectile(FVector3& firePosition, float angle, float speed) {
		_isFired = true;

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