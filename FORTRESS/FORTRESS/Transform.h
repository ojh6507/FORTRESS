#pragma once
#include "stdafx.h"
class Transform {
public:
	FVector3 GetPosition() const { return _position; }
	void SetPosition(FVector3 position) { _position = position; }

    FVector3 GetRotation() const { return _rotation; }
	void SetRotation(FVector3 rotation) { _rotation = rotation; }

	FVector3 GetScale() const { return _scale; }
	void SetScale(FVector3 scale) { _scale = scale; }
    XMFLOAT4X4 GetWorldMatrix() {
        XMMATRIX scaleMatrix = XMMatrixScaling(_scale.x, _scale.y, _scale.z);
        XMMATRIX rotationX = XMMatrixRotationX(XMConvertToRadians(_rotation.x));
        XMMATRIX rotationY = XMMatrixRotationY(XMConvertToRadians(_rotation.y));
        XMMATRIX rotationZ = XMMatrixRotationZ(XMConvertToRadians(_rotation.z));
        XMMATRIX translationMatrix = XMMatrixTranslation(_position.x, _position.y, _position.z);

        XMMATRIX localMatrix = scaleMatrix * rotationX * rotationY * rotationZ * translationMatrix;
        localMatrix = XMMatrixTranspose(localMatrix);

        XMFLOAT4X4 worldMatrixFloat4x4;
        XMStoreFloat4x4(&worldMatrixFloat4x4, localMatrix);
        return worldMatrixFloat4x4;
    }


private:
    FVector3 _position{1,1,0};
    FVector3 _rotation{0,0,0};
    FVector3 _scale{1,1,1};
};  

