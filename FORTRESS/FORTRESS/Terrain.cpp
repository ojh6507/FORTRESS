#include "Terrain.h"
#include <vector>
#include <algorithm>
#include <DirectXMath.h>
#include <random>
const float PI = 3.14159265358979;

Terrain::Terrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext, unsigned int width, unsigned int height, float scale)
    : GameObject(device, deviceContext), Width(width), Height(height), Scale(scale)
{
    Map = new byte[width, height];

    GeneratePlain();

    //D3D11_SUBRESOURCE_DATA initData = {};
    //initData.pSysMem = Map;
    //initData.SysMemPitch = width * sizeof(byte); // texture에서 한 줄 크기
    //initData.SysMemSlicePitch = 0;

    //D3D11_TEXTURE2D_DESC textureDesc = {};
    //textureDesc.Width = width;
    //textureDesc.Height = height;
    //textureDesc.MipLevels = 1;
    //textureDesc.ArraySize = 1;
    //textureDesc.Format = DXGI_FORMAT_R8_UINT;
    //textureDesc.SampleDesc.Count = 1;
    //textureDesc.Usage = D3D11_USAGE_DEFAULT; // 수정해야할수도있음
    //textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    //device->CreateTexture2D(&textureDesc, &initData, &MapTexture);

    //D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    //srvDesc.Format = DXGI_FORMAT_R8_UINT;
    //srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    //srvDesc.Texture2D.MostDetailedMip = 0;
    //srvDesc.Texture2D.MipLevels = 1;

    //_device->CreateShaderResourceView(MapTexture, &srvDesc, &MapTextureRSV);

    //D3D11_BUFFER_DESC vertexDesc = {};
    //vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    //vertexDesc.ByteWidth = sizeof(quadVertices);
    //vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    //D3D11_SUBRESOURCE_DATA vertexData = { quadVertices };
    //_device->CreateBuffer(&vertexDesc, &vertexData, &vertexBuffer);

    //// ? 4. 인덱스 버퍼 생성
    //D3D11_BUFFER_DESC indexDesc = {};
    //indexDesc.Usage = D3D11_USAGE_DEFAULT;
    //indexDesc.ByteWidth = sizeof(quadIndices);
    //indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    //D3D11_SUBRESOURCE_DATA indexData = { quadIndices };
    //_device->CreateBuffer(&indexDesc, &indexData, &indexBuffer);
}

Terrain::~Terrain()
{
    if (MapTextureRSV) MapTextureRSV->Release();
    if (MapTexture) MapTexture->Release();
    delete Map;
}

void Terrain::Update()
{
}

void Terrain::Render()
{
    //UINT stride = sizeof(Vertex);
    //UINT offset = 0;
    //_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    //_deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    //_deviceContext->DrawIndexed(6, 0, 0);  // ? 6개의 인덱스로 Quad 렌더링
}

//void Terrain::SetOffset(float x, float y)
//{
//
//}


inline byte Terrain::GetValue(unsigned int x, unsigned int y) const
{
    return Map[y * Width + x];
}

inline void Terrain::SetValue(unsigned int x, unsigned int y, bool state)
{
    Map[y * Width + x] = state;
}

// [x-scanInterval, x+scanInterval]에서의 slope를 구함
// y값은 ray를 쏘는 평면의 높이 -> 땅바닥이 아님
// x = 0, scanInterval = 1 일경우 [-1,1] 검사
// scanInterval = 1, 일경우 자기와 양옆을 기준으로 normal 산출
// ray는 정수 좌표에서만 쏨
// 항상 기울기는 위쪽
float Terrain::GetNormal(float x, float y, float scanInterval = 1)
{
#if defined(_DEBUG)
    assert(x < Width && y < Height);
    assert(x >= 00 && y >= 0);
#endif
    // map의 스케일로 변경
    x = x / Scale;
    y = y / Scale;

    int xMin = static_cast<int>(floor(x - scanInterval));
    int xMax = static_cast<int>(ceil(x + scanInterval));

    // map 안에 들어오도록 조정
    xMax = xMax > static_cast<int>(Width - 1) ? static_cast<int>(Width - 1) : xMax;
    xMin = xMin < 0 ? 0 : xMin;


    float xCenter = (static_cast<float>(xMax) + static_cast<float>(xMin)) / 2.f;
    int numRay = xMax - xMin + 1;
    if (numRay <= 1)
    {
        return 0.f;
    }

    std::vector<MapCoord> heightScanned;
    for (int i = 0; i < numRay; i++)
    {
        unsigned int scanPoint = xMin + i;
        unsigned int scanHeight = FindSurfaceDownward(scanPoint, y);

        heightScanned.push_back({ scanPoint, scanHeight });
    }

    // 가장 높은 순서대로 나열
    std::sort(heightScanned.begin(), heightScanned.end(),
        [](const MapCoord& a, const MapCoord& b) {
            return a.y > b.y;
        });

    // 위에서 떨어졌을때 무게중심 양옆으로 지면이 맞닿아있음
    MapCoord firstHighest = heightScanned[0];
    MapCoord secondHighest = firstHighest;
    // 무게중심이 받힘점 왼쪽에 있음
    if (firstHighest.x > xCenter)
    {
        // 다음 높은 위치 찾음 (단 왼쪽으로만)
        for (int i = 1; i < numRay; i++)
        {
            if (heightScanned[i].x < firstHighest.x)
            {
                secondHighest = heightScanned[i];
            }

        }
    }
    // 무게중심이 받힘점 오른쪽에 있음
    else
    {
        // 다음 높은 위치 찾음 (단 오른쪽으로만)
        for (int i = 1; i < numRay; i++)
        {
            if (heightScanned[i].x > firstHighest.x)
            {
                secondHighest = heightScanned[i];
            }

        }
    }

    // 기울기 구함
    float dx = firstHighest.x - secondHighest.x;
    float dy = firstHighest.y - secondHighest.y;

    // 기울기의 -역수 = 노말
    float normal = atan(-dx / dy);
    // -pi/2 ~ pi/2 에서 0 ~ pi/2 범위로 수정
    normal = normal > 0 ? normal : normal + PI;
    return normal;


    //for (int i = 0; i < numRay; i++)
    //{
    //    unsigned int scanPoint = xMin + i;
    //    unsigned int scanHeight = FindSurfaceDownward(scanPoint, y);

    //    heightScanned.push_back(std::pair<unsigned int, unsigned int>(scanPoint, scanHeight));
    //}

    //// 가장 높은 순서대로 나열
    //std::sort(heightScanned.begin(), heightScanned.end(),
    //    [](const auto& a, const auto& b) {
    //        return a.second > b.second;
    //    });

    //// 위에서 떨어졌을때 무게중심 양옆으로 지면이 맞닿아있음
    //std::pair<unsigned int, unsigned int>firstHighest = heightScanned[0];
    //std::pair<unsigned int, unsigned int>secondHighest = firstHighest;
    //// 무게중심이 받힘점 왼쪽에 있음
    //if (firstHighest.first > xCenter)
    //{
    //    // 다음 높은 위치 찾음 (단 왼쪽으로만)
    //    for (int i = 1; i < numRay; i++)
    //    {
    //        if (heightScanned[i].first < firstHighest.first)
    //        {
    //            secondHighest = heightScanned[i];
    //        }

    //    }
    //}
    //// 무게중심이 받힘점 오른쪽에 있음
    //else
    //{
    //    // 다음 높은 위치 찾음 (단 오른쪽으로만)
    //    for (int i = 1; i < numRay; i++)
    //    {
    //        if (heightScanned[i].first > firstHighest.first)
    //        {
    //            secondHighest = heightScanned[i];
    //        }

    //    }
    //}

    //// 기울기 구함
    //float dx = firstHighest.first - secondHighest.first;
    //float dy = firstHighest.second - secondHighest.second;

    //// 기울기의 -역수 = 노말
    //float normal = atan(-dx / dy);
    //// -pi/2 ~ pi/2 에서 0 ~ pi/2 범위로 수정
    //normal = normal > 0 ? : normal : normal + PI;
    //return normal;
}

bool Terrain::CheckCollisionCircle(float _posX, float _posY, float _radius)
{
    // SIMD 사용?
    //DirectX::XMFLOAT2 pos(_posX, _posY);
    //DirectX::
    return false;
}

bool Terrain::CheckCollisionAABB(float xMin, float Ymin, float xMax, float yMax, float xPos, float yPos)
{
    // terrain 좌표 기준으로 scale down해서 계산함 (collision box의 크기를 terrain 좌표계와 일치시킴)
    //DirectX::XMFLOAT2 TopRight = { static_cast<int>(xMax / Scale), static_cast<int>(yMax / Scale) };
    //DirectX::XMFLOAT2 BottomLeft = { static_cast<int>(xMin / Scale), static_cast<int>(Ymin / Scale) };

    //std::pair<unsigned int, unsigned int> TopRightInt = { static_cast<int>(xMax / Scale), static_cast<int>(yMax / Scale) };
    return false;
}

void Terrain::UpdateMapTexture()
{

}

void Terrain::GetMapTexture()
{
    //return MapTexture;
}

//bool Terrain::CheckCollisionOBB(float _posX, float _posY, float rotation)
//{
//    return false;
//}


void Terrain::GeneratePlain()
{
    int halfHeight = Height / 2;
    for (int y = 0; y < halfHeight; y++)
    {
        for (int x = 0; x < Width; x++)
        {
            SetValue(x, y, 1);
        }
    }
    for (int y = halfHeight; y < Height; y++)
    {
        for (int x = 0; x < Width; x++)
        {
            SetValue(x, y, 0);
        }
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
    // sample point만 잡음


}

// 윗 방향으로 surface를 찾습니다.
unsigned int Terrain::FindSurfaceUpward(unsigned int x, float y) const
{
    for (int sampleY = static_cast<int>(ceil(y)); sampleY < Height; sampleY++)
    {
        if (IsSurface(x, sampleY))
        {
            return sampleY;
        }
    }
    return Height - 1;
    // 입력된 좌표가 땅 속이여야함 -> Map에서 (x,y)가 true
    //assert(GetValue(x,y));
    //if (!GetValue(x, y))
    //{
    //    return false;
    //}

    //for (unsigned int sampleY = y; sampleY < Height; sampleY++)
    //{
    //    // 하늘일경우
    //    if (!GetValue(x, sampleY))
    //    {
    //        return sampleY;
    //    }
    //}

    //// 맵 끝까지 갔는데 없으면 땅속임
    //return Height;
}

unsigned int Terrain::FindSurfaceDownward(unsigned int x, float y) const
{
    for (int sampleY = static_cast<int>(floor(y)); sampleY >= 0; sampleY--)
    {
        if (IsSurface(x, sampleY))
        {
            return sampleY;
        }
    }
    return 0;
    // 입력된 좌표가 하늘 속이여야함 -> Map에서 (x,y)가 false
    //assert(!GetValue(x, y));
    //if (GetValue(x, y))
    //{
    //    return false;
    //}

    //// 아래 방향으로 찾음
    //for (unsigned int sampleY = y - 1; sampleY >= 0; sampleY--)
    //{
    //    // 땅일경우
    //    if (GetValue(x, sampleY))
    //    {
    //        return sampleY;
    //    }
    //}

    //// 맵 끝까지 갔는데 없으면 하늘임
    //return 0;
}


// 해당 좌표가 땅이고 바로 위가 하늘인지 확인
inline bool Terrain::IsSurface(unsigned int x, unsigned int y) const
{
    if (!GetValue(x, y))
    {
        return false;
    }

    return !GetValue(x, y + 1);
}

