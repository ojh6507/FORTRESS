#include "common.h"
#include "States.h"

void RasterizerState::Create() {
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = false;

	HRESULT hr = _device->CreateRasterizerState(&desc, &_rasterizerState);
	assert(SUCCEEDED(hr));
}
