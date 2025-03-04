#include "Terrain.h"
#include <DirectXMath.h>
const float PI = 3.14159265358979;

Terrain::Terrain(unsigned int width, unsigned int height, float scale) : Width(width), Height(height), Scale(scale)
{
    Map = new bool[width, height];
}

//void Terrain::SetOffset(float x, float y)
//{
//
//}


inline bool Terrain::GetValue(unsigned int x, unsigned int y)
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
    assert(x >= 0 0 && y >= 0);
#endif
    // map의 스케일로 변경
    x = x / Scale;
    y = y / Scale;

    int xMin = static_cast<int>ceil(x - scanInterval);
    int xMax = static_cast<int>floor(x + scanInterval);
    
    // map 안에 들어오도록 조정
    xMax = xMax > Width - 1 ? Width - 1 : xMax;
    xMin = xMin < 0 ? 0 : xMin;
    yMax = yMax > Height - 1 ? Height - 1 : yMax;
    yMin = yMin < 0 ? 0 : yMin;

    float xCenter = (static_cast<float>xMax + static_cast<float>xMin) / 2.f;
    int numRay = xMax - xMin + 1;
    if (numRay <= 1)
    {
        return 0.f;
    }

    std::array<std::pair<unsigned int, unsigned int>, numRay> heightScanned{};
    for (int i = 0; i < numRay; i++)
    {
        unsigned int scanPoint = xMin + i;
        heightScanned[i].first = scanPoint;

        // 높이 y로부터 ray 쏘기
        heightScanned[i].second = FindSurfaceDownward(scanPoint, y);
    }

    // 가장 높은 순서대로 나열
    std::sort(heightScanned.begin(), heightScanned.end(),
        [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

    // 위에서 떨어졌을때 무게중심 양옆으로 지면이 맞닿아있음
    std::pair<unsigned int, unsigned int>firstHighest = heightScanned[0];
    std::pair<unsigned int, unsigned int>secondHighest = firstHighest;
    // 무게중심이 받힘점 왼쪽에 있음
    if (firstHighest.first > xCenter)
    {
        // 다음 높은 위치 찾음 (단 왼쪽으로만)
        for (int i = 1; i < numRay; i++)
        {
            if (heightScanned[i].first < firstHighest.first)
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
            if (heightScanned[i].first > firstHighest.first)
            {
                secondHighest = heightScanned[i];
            }

        }
    }

    // 기울기 구함
    float dx = firstHighest.first - secondHighest.first;
    float dy = firstHighest.second - secondHighest.second;

    // 기울기의 -역수 = 노말
    float normal = atan(-dx / dy);
    // -pi/2 ~ pi/2 에서 0 ~ pi/2 범위로 수정
    normal = normal > 0 ? : normal : normal + PI;
    return normal;
}

bool Terrain::CheckCollisionCircle(float _posX, float _posY, float _radius)
{
    // SIMD 사용?
    DirectX::XMFLOAT2 pos(_posX, _posY);
    //DirectX::
}

bool Terrain::CheckCollisionAABB(float xMin, float Ymin, float xMax, float yMax, float xPos, float yPos)
{
    // terrain 좌표 기준으로 scale down해서 계산함 (collision box의 크기를 terrain 좌표계와 일치시킴)
    DirectX::XMFLOAT2 TopRight = { static_cast<int>(xMax / Scale), static_cast<int>(yMax / Scale) };
    DirectX::XMFLOAT2 BottomLeft = { static_cast<int>(xMin / Scale), static_cast<int>(Ymin / Scale) };

    std::pair<unsigned int, unsigned int> TopRightInt = { static_cast<int>(xMax / Scale), static_cast<int>(yMax / Scale) };

}

bool Terrain::CheckCollisionOBB(float _posX, float _posY, float rotation)
{
    return false;
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
    // sample point만 잡음


}

// 윗 방향으로 surface를 찾습니다.
unsigned int Terrain::FindSurfaceUpward(unsigned int x, float y) const
{
    for (unsigned int sampleY = static_cast<unsigned int>ceil(y); sampleY < Height; sampleY++)
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
    for (unsigned int sampleY = static_cast<unsigned int>floor(y); sampleY >= 0; sampleY--)
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
inline bool Terrain::IsSurface(unsigned int x, unsigned y) const
{
    if (!GetValue(x, y))
    {
        return false;
    }

    return !GetValue(x, y + 1);
}

