#pragma once
class GameObject {
public:
	GameObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~GameObject();

	void Update();
	void Render();

private:
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
	ConstantBuffer<Transform>* _constantBuffer;
};