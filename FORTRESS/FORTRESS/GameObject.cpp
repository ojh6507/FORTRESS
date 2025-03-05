#include "stdafx.h"
#include "GameObject.h"

GameObject::GameObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext): _device(device), _deviceContext(deviceContext) {

	_vertexShader = new VertexShader(device);
	_vertexShader->Create(L"Shader.hlsl", "VS", "vs_5_0");

	_pixelShader = new PixelShader(device);
	_pixelShader->Create(L"Shader.hlsl", "PS", "ps_5_0");

	_inputLayout = new InputLayout(device);
	_inputLayout->Create(FVertexSimple::descs, _vertexShader->GetBlob());

	_constantBuffer = new ConstantBuffer<VS_CB_GAMEOBJECT_INFO>(device, deviceContext);
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

void GameObject::Render() {
	_deviceContext->IASetInputLayout(_inputLayout->Get());
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->VSSetShader(_vertexShader->Get(), nullptr, 0);
	_deviceContext->RSSetState(_rasterizerState->Get());
	_deviceContext->PSSetShader(_pixelShader->Get(), nullptr, 0);

	UINT32 stride, offset;
	ID3D11Buffer* vertexBuffer = _vertexBuffer->Get();
	stride = _vertexBuffer->GetStride();
	offset = _vertexBuffer->GetOffset();
	_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	_deviceContext->IASetIndexBuffer(_indexBuffer->Get(), DXGI_FORMAT_R32_UINT, 0);

	_constantBuffer->CopyData(VS_CB_GAMEOBJECT_INFO{_tf.GetWorldMatrix()});
	ID3D11Buffer* constantBuffer = _constantBuffer->Get();
	_deviceContext->VSSetConstantBuffers(2, 1, &constantBuffer);

	_deviceContext->DrawIndexed(_indexBuffer->GetCount(), 0, 0);
}

void _test_concrete_GameObject::Update(double deltaTime) {
	if (Input::Instance()->IsKeyDown(DIK_W)) {
		_tf.SetPosition(_tf.GetPosition() + FVector3(0.f, 30.0f, 0.f) * deltaTime);
	}
	if (Input::Instance()->IsKeyPressed(DIK_A)) {
		_tf.SetPosition(_tf.GetPosition() + FVector3(-30.05f, 0.f, 0.f));
	}
	if (Input::Instance()->IsKeyDown(DIK_S)) {
		_tf.SetPosition(_tf.GetPosition() + FVector3(0.f, -30.0f, 0.f) * deltaTime);
	}
	if (Input::Instance()->IsKeyPressed(DIK_D)) {
		_tf.SetPosition(_tf.GetPosition() + FVector3(30.05f, 0.f, 0.f));
	}

	if (Input::Instance()->IsMouseButtonPressed(0)) {
		_tf.SetPosition(_tf.GetPosition() + FVector3(-10.05f, 0.f, 0.f));
	}
	if (Input::Instance()->IsMouseButtonReleased(1)) {
		_tf.SetPosition(_tf.GetPosition() + FVector3(10.05f, 0.f, 0.f));
	}
}

Projectile::Projectile(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	:GameObject(device, deviceContext)
{

	constexpr int segments = 360;  // 원을 구성하는 세그먼트 개수
	constexpr float radius = 20.0f; // 원 반지름
	constexpr float PI = 3.1415926f;

	_vertices.resize(segments + 1);
	_vertices[0] = { 0.0f, 0.0f, 0.0f, 1.f, 1.f, 1.f, 1.f }; // 중심점

	for (int i = 0; i < segments; ++i) {
		float theta = (2.0f * PI * i) / segments;
		_vertices[i + 1] = { radius * cosf(theta), radius * sinf(theta), 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	}

	_indices.reserve(segments * 3);
	for (int i = 0; i < segments; ++i) {
		_indices.push_back(0);  // 중심점
		_indices.push_back((i + 1) % segments + 1);  // 다음 정점
		_indices.push_back(i + 1);  // 현재 정점 (순서 변경)
	}

	_vertexBuffer = new VertexBuffer<FVertexSimple>(device);
	_vertexBuffer->Create(_vertices);

	_indexBuffer = new IndexBuffer(device);
	_indexBuffer->Create(_indices);
	_tf.SetPosition({-300, -100, 0});
	
}

void Projectile::Update(double deltaTime)
{
	if (Input::Instance()->IsMouseButtonDown(0)) {

		FireProjectile();
	}
	if(_isFired)
		UpdateProjectile(deltaTime);
}

CubeObject::CubeObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext) : GameObject(device, deviceContext)
{
	_vertices.resize(4);

	_vertices[0].x = -40.1f;
	_vertices[0].y = 40.1f;
	_vertices[0].z = 40.f;
	_vertices[0].r = 1.f;
	_vertices[0].g = 1.f;
	_vertices[0].b = 1.f;
	_vertices[0].a = 1.f;

	_vertices[1].x = 40.1f;
	_vertices[1].y = 40.1f;
	_vertices[1].z = 40.f;
	_vertices[1].r = 1.f;
	_vertices[1].g = 1.f;
	_vertices[1].b = 1.f;
	_vertices[1].a = 1.f;

	_vertices[2].x = 40.1f;
	_vertices[2].y = -40.1f;
	_vertices[2].z = 40.f;
	_vertices[2].r = 1.f;
	_vertices[2].g = 1.f;
	_vertices[2].b = 1.f;
	_vertices[2].a = 1.f;

	_vertices[3].x = -40.1f;
	_vertices[3].y = -40.1f;
	_vertices[3].z = 40.f;
	_vertices[3].r = 1.f;
	_vertices[3].g = 1.f;
	_vertices[3].b = 1.f;
	_vertices[3].a = 1.f;


	_indices.resize(6);

	_indices[0] = 0;
	_indices[1] = 1;
	_indices[2] = 2;
	_indices[3] = 2;
	_indices[4] = 3;
	_indices[5] = 0;

	_vertexBuffer = new VertexBuffer<FVertexSimple>(device);
	_vertexBuffer->Create(_vertices);

	_indexBuffer = new IndexBuffer(device);
	_indexBuffer->Create(_indices);

}



ObjObject::ObjObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::wstring& filepath)
	:GameObject(device, deviceContext)
{
	std::ifstream objFile(filepath.c_str());

	if (objFile) {

		std::string line;
		while (std::getline(objFile, line))
		{
			std::istringstream lineStream(line);
			std::string type;
			lineStream >> type;

			if (type == "v") // Vertex position
			{
				XMFLOAT3 vertex;
				lineStream >> vertex.x >> vertex.y >> vertex.z;
				FVertexSimple vertexSimple{ vertex.x, vertex.y, vertex.z, 1, 0, 0, 1 };
				_vertices.push_back(vertexSimple);
			}
			else if (type == "f") // Face
			{
				std::vector<uint32_t> faceIndices;
				uint32_t index;

				while (lineStream >> index) { // 정점 인덱스만 읽음
					faceIndices.push_back(index - 1); // 1-based -> 0-based 인덱스 변환
				}

				
				for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
					_indices.push_back(faceIndices[0]);    // 첫 번째 정점
					_indices.push_back(faceIndices[i]);    // 두 번째 정점
					_indices.push_back(faceIndices[i + 1]); // 세 번째 정점
				}
			}

			DebugVerticesAndIndices();
		}


		_vertexBuffer = new VertexBuffer<FVertexSimple>(device);
		_vertexBuffer->Create(_vertices);

		_indexBuffer = new IndexBuffer(device);
		_indexBuffer->Create(_indices);
	}
}

void ObjObject::Update(double deltaTime)
{

}
