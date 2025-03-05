#include "Camera.h"

Camera::Camera()
{
	m_xmf4x4View = Matrix4x4::Identity();
	m_xmf4x4Projection = Matrix4x4::Identity();
	m_d3dViewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_d3dScissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	m_xmf3Offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fTimeLag = 0.0f;
	m_xmf3LookAtWorld = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_nMode = 0x00;

	

}
void Camera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}

void Camera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}

void Camera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float viewWidth, float viewHeight)
{
	XMMATRIX xmmtxProjection = XMMatrixOrthographicLH(
		viewWidth,  // 뷰포트 너비
		viewHeight, // 뷰포트 높이
		fNearPlaneDistance,
		fFarPlaneDistance
	);
	XMStoreFloat4x4(&m_xmf4x4Projection, xmmtxProjection);
}

void Camera::CreateShaderVariables(ID3D11Device* pd3dDevice, ID3D11DeviceContext* deviceContext)
{
	_constantBuffer = new ConstantBuffer<VS_CB_CAMERA_INFO>(pd3dDevice, deviceContext);
	_constantBuffer->Create();
}

void Camera::UpdateShaderVariables(ID3D11DeviceContext* deviceContext)
{
	
	if (!_constantBuffer) return;
	XMFLOAT4X4 xmf4x4View;
	XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));

	XMFLOAT4X4 xmf4x4Projection;
	XMStoreFloat4x4(&xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));


	VS_CB_CAMERA_INFO cbCameraInfo = {};
	cbCameraInfo.m_xmf4x4View = xmf4x4View;
	cbCameraInfo.m_xmf4x4Projection = xmf4x4Projection;
	cbCameraInfo.m_xmf3Position = m_xmf3Position;
	_constantBuffer->CopyData(cbCameraInfo);

	// 버퍼 바인딩
	ID3D11Buffer* constantBuffer = _constantBuffer->Get();
	deviceContext->VSSetConstantBuffers(1, 1, &constantBuffer);
}

void Camera::ReleaseShaderVariables(ID3D11DeviceContext* deviceContext)
{
	
}

void Camera::SetViewportsAndScissorRects(ID3D11DeviceContext* deviceContext)
{
	deviceContext->RSSetViewports(1, &m_d3dViewport);
	deviceContext->RSSetScissorRects(1, &m_d3dScissorRect);
}
