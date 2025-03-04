#include "common.h"
#include "InputLayout.h"

void InputLayout::Create(const vector<D3D11_INPUT_ELEMENT_DESC>& descs, ID3DBlob* vsblob) {
	const UINT32 count = static_cast<UINT32>(descs.size());
	HRESULT hr = _device->CreateInputLayout(descs.data(), count, vsblob->GetBufferPointer(), vsblob->GetBufferSize(), &_inputLayout);
	assert(SUCCEEDED(hr));
}

