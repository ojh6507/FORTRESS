#include "Terrain.h"
#include <DirectXMath.h>

Terrain::Terrain(unsigned int width, unsigned int height, float scale) : Width(width), Height(height), Scale(scale)
{
    Map = new bool[width, height];
}

void Terrain::SetOffset(float x, float y)
{

}


bool Terrain::GetValue(unsigned int x, unsigned int y)
{
    return Map[y * Width + x];
}

inline void Terrain::SetValue(unsigned int x, unsigned int y, bool state)
{
    Map[y * Width + x] = state;
}

bool Terrain::CheckCollisionCircle(float _posX, float _posY, float _radius)
{
    // SIMD 사용?
    DirectX::XMFLOAT2 pos(_posX, _posY);
    //DirectX::
}

bool Terrain::CheckCollisionAABB(float _minX, float _minY, float _maxX, float _maxY, float _posX, float _posY)
{
    // terrain 좌표 기준으로 scale down해서 계산함 (collision box의 크기를 terrain 좌표계와 일치시킴)
    DirectX::XMFLOAT2 TopRight = { static_cast<int>(_maxX / Scale), static_cast<int>(_maxY / Scale) };
    DirectX::XMFLOAT2 BottomLeft = { static_cast<int>(_minX / Scale), static_cast<int>(_minY / Scale) };

    std::pair<unsigned int, unsigned int> TopRightInt = { static_cast<int>(_maxX / Scale), static_cast<int>(_maxY / Scale) };

}

bool Terrain::CheckCollisionOBB(float _posX, float _posY, float rotation)
{
    return false;
}

float Terrain::GetNormal(float _posX, float _posY)
{
    
}

float Terrain::GetGround(float _posX, float _posY)
{
    
}

}


void GeneratePlain(int* heightMap, unsigned int length)
{
    for (size_t i = 0; i < length; i++)
    {
        heightMap[i] = 0;
    }
}


void GeneratePerlinNoise1D(size_t frequency, int amplitude, int* heightMap, unsigned int length)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(-amplitude, amplitude);

    int numSamplePoint = static_cast<int>(ceil(length / static_cast<float>(frequency)));
    int* samplePoints = new int[numSamplePoint];
    for (int i = 0; i < numSamplePoint; i++)
    {
        samplePoints[i] = distrib(gen);
    }


}