#pragma once
#include "stdafx.h"

class Graphics {
public:
	Graphics(HWND hWnd);
	~Graphics();

	void RenderBegin();
	void RenderEnd();

	ID3D11Device* GetDevice() { return _device; }
	ID3D11DeviceContext* GetDeviceContext() { return _deviceContext; }

private:

	HWND _hWnd;
	UINT32 _width = FRAME_BUFFER_WIDTH;
	UINT32 _height = FRAME_BUFFER_HEIGHT;
	
	// Device & swapChain
	ID3D11Device* _device = nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;
	IDXGISwapChain* _swapChain = nullptr;

	// RTV
	ID3D11Texture2D* _backBuffer = nullptr;
	ID3D11RenderTargetView* _renderTargetView = nullptr;

	// Rasterizer
	ID3D11RasterizerState* _rasterizerState = nullptr;

	D3D11_VIEWPORT _viewport;

	float _clearColor[4] = { 0.f, 0.16f, 0.16f, 0.16f };

private:
	void CreateDeviceAndSwapChain();
	void ReleaseDeviceAndSwapChain();
	void CreateRenderTargetView();
	void ReleaseRenderTargetView();
};

