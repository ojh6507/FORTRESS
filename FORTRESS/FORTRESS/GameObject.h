#pragma once
#include "stdafx.h"

class GameObject
{
public:
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3D11Buffer* ConstantBuffer;
	//
	//
};