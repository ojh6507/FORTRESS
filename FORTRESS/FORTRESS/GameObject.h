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

protected:
	Transform _tf;
	VertexBuffer<FVertexSimple>* _vertexBuffer;
	IndexBuffer* _indexBuffer;

	std::vector<FVertexSimple> _vertices;
	std::vector<UINT32> _indices;
	
	ConstantBuffer<VS_CB_GAMEOBJECT_INFO>* _constantBuffer;
	GameObject* child;

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
class  Projectile : public GameObject 
{
public:
	Projectile(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Update(double deltaTime);

	FVector3 _velocity = { 0.0f, 0.0f, 0.0f };
	FVector3 _position = { 0.0f, 0.0f, 0.0f };
	FVector3 _acceleration = { -10.1f, -150.8f, 0.0f };
	bool _isFired = false;

	void OutOfScreen() { _tf.SetPosition(FVector3(FRAME_BUFFER_WIDTH * 2, FRAME_BUFFER_HEIGHT * 2, 0)); }

	void FireProjectile(FVector3 firePosition, float angle, float speed) {
		_isFired = true;

		// 초기 발사 위치 (예제: 현재 오브젝트 위치)
		_tf.SetPosition(firePosition);
		_position = _tf.GetPosition();

		// 초기 발사 속도와 각도 설정
		//float speed = 200.f;  // 발사 속도
		//float angle = 85.0f;  // 발사 각도 (도 단위)

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
	void DebugVerticesAndIndices()
	{
		std::wstring debugStr;
		debugStr += L"=== Vertex Data ===\n";

		for (size_t i = 0; i < _vertices.size(); i++) {
			debugStr += L"Vertex " + std::to_wstring(i) + L": (" +
				std::to_wstring(_vertices[i].x) + L", " +
				std::to_wstring(_vertices[i].y) + L", " +
				std::to_wstring(_vertices[i].z) + L")\n";
		}

		debugStr += L"=== Index Data ===\n";
		for (size_t i = 0; i < _indices.size(); i++) {
			debugStr += std::to_wstring(_indices[i]) + L" ";
			if ((i + 1) % 3 == 0) debugStr += L"\n"; // 삼각형 단위 출력
		}

		OutputDebugString(debugStr.c_str());
	}
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