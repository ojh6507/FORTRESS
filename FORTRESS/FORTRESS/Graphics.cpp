#include "Graphics.h"

Graphics::Graphics(HWND hWnd): _hWnd(hWnd) {
	CreateDeviceAndSwapChain();
	CreateRenderTargetView();
}

Graphics::~Graphics() {
	ReleaseRenderTargetView();
	ReleaseDeviceAndSwapChain();
}

void Graphics::RenderBegin() {
	_deviceContext->OMSetRenderTargets(1, &_renderTargetView, nullptr);
	_deviceContext->ClearRenderTargetView(_renderTargetView, _clearColor);
}

void Graphics::RenderEnd() {
	_swapChain->Present(1, 0);
}

void Graphics::CreateDeviceAndSwapChain() {
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

	DXGI_SWAP_CHAIN_DESC desc = {};
	{
		desc.BufferDesc.Width = FRAME_BUFFER_WIDTH;							// 버퍼 크기
		desc.BufferDesc.Height = FRAME_BUFFER_HEIGHT;
		desc.BufferDesc.RefreshRate.Numerator = 60;				// 주사율
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 컬러 포맷
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SampleDesc.Count = 1;								// 멀티샘플링. 안티애일리어싱이랑 관련
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// 최종 렌더 목적으로 사용하겠다.
		desc.BufferCount = 1;									// 후면 버퍼 개수
		desc.OutputWindow = _hWnd;								// 목표하는 윈도우 창
		desc.Windowed = true;									// 창모드 여부
		desc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
	}
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&desc,
		&_swapChain,
		&_device,
		nullptr,
		&_deviceContext
	);
	assert(SUCCEEDED(hr));
}

void Graphics::ReleaseDeviceAndSwapChain() {
	if (_deviceContext)
		_deviceContext->Flush();

	if (_renderTargetView) {
		_renderTargetView->Release();
		_renderTargetView = nullptr;
	}

	if (_deviceContext) {
		_deviceContext->Release();
		_deviceContext = nullptr;
	}

	if (_device) {
		_device->Release();
		_device = nullptr;
	}
}

void Graphics::CreateRenderTargetView() {
	HRESULT hr;
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&_backBuffer);
	assert(SUCCEEDED(hr));

	D3D11_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	hr = _device->CreateRenderTargetView(_backBuffer, nullptr, &_renderTargetView);
	assert(SUCCEEDED(hr));
}

void Graphics::ReleaseRenderTargetView() {
	if (_backBuffer) {
		_backBuffer->Release();
		_backBuffer = nullptr;
	}
	if (_renderTargetView) {
		_renderTargetView->Release();
		_renderTargetView = nullptr;
	}
}

