#pragma once
#include "GameObject.h"
#include <DirectXMath.h>

class MovableObject :
    public GameObject
{
public:
    DirectX::XMFLOAT2 position;
    DirectX::XMFLOAT2 rotation;


    MovableObject() : position(0.0f, 0.0f);
};

