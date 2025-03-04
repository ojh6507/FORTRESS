#include "stdafx.h"
#include "GameObject.h"

GameObject::GameObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {

	_vertices.resize(4);

	_vertices[0].x = -0.1f;
	_vertices[0].y = 0.1f;
	_vertices[0].z = 0.f;
	_vertices[0].r = 1.f;
	_vertices[0].g = 1.f;
	_vertices[0].b = 1.f;
	_vertices[0].a = 1.f;

	_vertices[1].x = 0.1f;
	_vertices[1].y = 0.1f;
	_vertices[1].z = 0.f;
	_vertices[1].r = 1.f;
	_vertices[1].g = 1.f;
	_vertices[1].b = 1.f;
	_vertices[1].a = 1.f;

	_vertices[2].x = 0.1f;
	_vertices[2].y = -0.1f;
	_vertices[2].z = 0.f;
	_vertices[2].r = 1.f;
	_vertices[2].g = 1.f;
	_vertices[2].b = 1.f;
	_vertices[2].a = 1.f;

	_vertices[3].x = -0.1f;
	_vertices[3].y = -0.1f;
	_vertices[3].z = 0.f;
	_vertices[3].r = 1.f;
	_vertices[3].g = 1.f;
	_vertices[3].b = 1.f;
	_vertices[3].a = 1.f;


	_indices.resize(6);

	_indices[0] = 0;
	_indices[1] = 1;
	_indices[2] = 2;
	_indices[3] = 2;
	_indices[4] = 1;
	_indices[5] = 3;

	_vertexBuffer = new VertexBuffer<FVertexSimple>(device);
	_vertexBuffer->Create(_vertices);

	_indexBuffer = new IndexBuffer(device);
	_indexBuffer->Create(_indices);

	_vertexShader = new VertexShader(device);
	_vertexShader->Create(L"Shader.hlsl", "VS", "vs_5_0");

	_pixelShader = new PixelShader(device);
	_pixelShader->Create(L"Shader.hlsl", "PS", "ps_5_0");

	_inputLayout = new InputLayout(device);
	_inputLayout->Create(FVertexSimple::descs, _vertexShader->GetBlob());

	_constantBuffer = new ConstantBuffer<Transform>(device, deviceContext);
	_constantBuffer->Create();

	_rasterizerState = new RasterizerState(device);
	_rasterizerState->Create();
}

GameObject::~GameObject() {
	delete _vertexBuffer;
	delete _indexBuffer;
	delete _vertexShader;
	delete _pixelShader;
	delete _inputLayout;
	delete _constantBuffer;
	delete _rasterizerState;
}

void GameObject::Update() {
}

void GameObject::Render() {
	_deviceContext->IASetInputLayout(_inputLayout->Get());
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->VSSetShader(_vertexShader->Get(), nullptr, 0);
	_deviceContext->RSSetState(_rasterizerState->Get());
	_deviceContext->PSSetShader(_pixelShader->Get(), nullptr, 0);

	UINT32 stride, offset;
	ID3D11Buffer* vertexBuffer = _vertexBuffer->Get();
	stride = _vertexBuffer->GetStride();
	stride = _vertexBuffer->GetOffset();
	_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	_deviceContext->IASetIndexBuffer(_indexBuffer->Get(), DXGI_FORMAT_R32_UINT, 0);

	ID3D11Buffer* constantBuffer = _constantBuffer->Get();
	_deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

	_deviceContext->DrawIndexed(_indexBuffer->GetCount(), 0, 0);
}
