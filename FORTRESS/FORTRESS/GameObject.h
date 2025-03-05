#pragma once

struct VS_CB_GAMEOBJECT_INFO {
	XMFLOAT4X4 m_xmf4x4World;
};

class GameObject {
public:
	GameObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~GameObject();

	void Update(float deltaTime);
	void Render();

private:
	GameObject* child;
	ID3D11Device* _device;
	ID3D11DeviceContext* _deviceContext;

	std::vector<FVertexSimple> _vertices;
	std::vector<UINT32> _indices;
	VertexBuffer<FVertexSimple>* _vertexBuffer;
	IndexBuffer* _indexBuffer;
	InputLayout* _inputLayout;

	VertexShader* _vertexShader;
	PixelShader* _pixelShader;

	RasterizerState* _rasterizerState;
	SamplerState* _samplerState;
	
	Transform _tf;
	ConstantBuffer<VS_CB_GAMEOBJECT_INFO>* _constantBuffer;
};