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

	void Render();

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