#pragma once

struct FVertexSimple
{
	float x, y, z;	 // Position
	float r, g, b, a; // Color
	static std::vector<D3D11_INPUT_ELEMENT_DESC> descs;
};