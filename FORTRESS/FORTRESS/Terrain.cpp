#include "Terrain.h"
#include <vector>
#include <algorithm>
#include <DirectXMath.h>
#include <random>
const float PI = 3.14159265358979;

Terrain::Terrain(unsigned int width, unsigned int height, float scale) : Width(width), Height(height), Scale(scale)
{
    Map = new bool[width, height];
}

//void Terrain::SetOffset(float x, float y)
//{
//
//}


inline bool Terrain::GetValue(unsigned int x, unsigned int y) const
{
    return Map[y * Width + x];
}

inline void Terrain::SetValue(unsigned int x, unsigned int y, bool state)
{
    Map[y * Width + x] = state;
}

// [x-scanInterval, x+scanInterval]������ slope�� ����
// y���� ray�� ��� ����� ���� -> ���ٴ��� �ƴ�
// x = 0, scanInterval = 1 �ϰ�� [-1,1] �˻�
// scanInterval = 1, �ϰ�� �ڱ�� �翷�� �������� normal ����
// ray�� ���� ��ǥ������ ��
// �׻� ����� ����
float Terrain::GetNormal(float x, float y, float scanInterval = 1)
{
#if defined(_DEBUG)
    assert(x < Width && y < Height);
    assert(x >= 00 && y >= 0);
#endif
    // map�� �����Ϸ� ����
    x = x / Scale;
    y = y / Scale;

    int xMin = static_cast<int>(floor(x - scanInterval));
    int xMax = static_cast<int>(ceil(x + scanInterval));

    // map �ȿ� �������� ����
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

    // ���� ���� ������� ����
    std::sort(heightScanned.begin(), heightScanned.end(),
        [](const MapCoord& a, const MapCoord& b) {
            return a.y > b.y;
        });

    // ������ ���������� �����߽� �翷���� ������ �´������
    MapCoord firstHighest = heightScanned[0];
    MapCoord secondHighest = firstHighest;
    // �����߽��� ������ ���ʿ� ����
    if (firstHighest.x > xCenter)
    {
        // ���� ���� ��ġ ã�� (�� �������θ�)
        for (int i = 1; i < numRay; i++)
        {
            if (heightScanned[i].x < firstHighest.x)
            {
                secondHighest = heightScanned[i];
            }

        }
    }
    // �����߽��� ������ �����ʿ� ����
    else
    {
        // ���� ���� ��ġ ã�� (�� ���������θ�)
        for (int i = 1; i < numRay; i++)
        {
            if (heightScanned[i].x > firstHighest.x)
            {
                secondHighest = heightScanned[i];
            }

        }
    }

    // ���� ����
    float dx = firstHighest.x - secondHighest.x;
    float dy = firstHighest.y - secondHighest.y;

    // ������ -���� = �븻
    float normal = atan(-dx / dy);
    // -pi/2 ~ pi/2 ���� 0 ~ pi/2 ������ ����
    normal = normal > 0 ? normal : normal + PI;
    return normal;


    //for (int i = 0; i < numRay; i++)
    //{
    //    unsigned int scanPoint = xMin + i;
    //    unsigned int scanHeight = FindSurfaceDownward(scanPoint, y);

    //    heightScanned.push_back(std::pair<unsigned int, unsigned int>(scanPoint, scanHeight));
    //}

    //// ���� ���� ������� ����
    //std::sort(heightScanned.begin(), heightScanned.end(),
    //    [](const auto& a, const auto& b) {
    //        return a.second > b.second;
    //    });

    //// ������ ���������� �����߽� �翷���� ������ �´������
    //std::pair<unsigned int, unsigned int>firstHighest = heightScanned[0];
    //std::pair<unsigned int, unsigned int>secondHighest = firstHighest;
    //// �����߽��� ������ ���ʿ� ����
    //if (firstHighest.first > xCenter)
    //{
    //    // ���� ���� ��ġ ã�� (�� �������θ�)
    //    for (int i = 1; i < numRay; i++)
    //    {
    //        if (heightScanned[i].first < firstHighest.first)
    //        {
    //            secondHighest = heightScanned[i];
    //        }

    //    }
    //}
    //// �����߽��� ������ �����ʿ� ����
    //else
    //{
    //    // ���� ���� ��ġ ã�� (�� ���������θ�)
    //    for (int i = 1; i < numRay; i++)
    //    {
    //        if (heightScanned[i].first > firstHighest.first)
    //        {
    //            secondHighest = heightScanned[i];
    //        }

    //    }
    //}

    //// ���� ����
    //float dx = firstHighest.first - secondHighest.first;
    //float dy = firstHighest.second - secondHighest.second;

    //// ������ -���� = �븻
    //float normal = atan(-dx / dy);
    //// -pi/2 ~ pi/2 ���� 0 ~ pi/2 ������ ����
    //normal = normal > 0 ? : normal : normal + PI;
    //return normal;
}

bool Terrain::CheckCollisionCircle(float _posX, float _posY, float _radius)
{
    // SIMD ���?
    DirectX::XMFLOAT2 pos(_posX, _posY);
    //DirectX::
}

bool Terrain::CheckCollisionAABB(float xMin, float Ymin, float xMax, float yMax, float xPos, float yPos)
{
    // terrain ��ǥ �������� scale down�ؼ� ����� (collision box�� ũ�⸦ terrain ��ǥ��� ��ġ��Ŵ)
    DirectX::XMFLOAT2 TopRight = { static_cast<int>(xMax / Scale), static_cast<int>(yMax / Scale) };
    DirectX::XMFLOAT2 BottomLeft = { static_cast<int>(xMin / Scale), static_cast<int>(Ymin / Scale) };

    std::pair<unsigned int, unsigned int> TopRightInt = { static_cast<int>(xMax / Scale), static_cast<int>(yMax / Scale) };

}

//bool Terrain::CheckCollisionOBB(float _posX, float _posY, float rotation)
//{
//    return false;
//}


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
    // sample point�� ����


}

// �� �������� surface�� ã���ϴ�.
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
    // �Էµ� ��ǥ�� �� ���̿����� -> Map���� (x,y)�� true
    //assert(GetValue(x,y));
    //if (!GetValue(x, y))
    //{
    //    return false;
    //}

    //for (unsigned int sampleY = y; sampleY < Height; sampleY++)
    //{
    //    // �ϴ��ϰ��
    //    if (!GetValue(x, sampleY))
    //    {
    //        return sampleY;
    //    }
    //}

    //// �� ������ ���µ� ������ ������
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
    // �Էµ� ��ǥ�� �ϴ� ���̿����� -> Map���� (x,y)�� false
    //assert(!GetValue(x, y));
    //if (GetValue(x, y))
    //{
    //    return false;
    //}

    //// �Ʒ� �������� ã��
    //for (unsigned int sampleY = y - 1; sampleY >= 0; sampleY--)
    //{
    //    // ���ϰ��
    //    if (GetValue(x, sampleY))
    //    {
    //        return sampleY;
    //    }
    //}

    //// �� ������ ���µ� ������ �ϴ���
    //return 0;
}


// �ش� ��ǥ�� ���̰� �ٷ� ���� �ϴ����� Ȯ��
inline bool Terrain::IsSurface(unsigned int x, unsigned int y) const
{
    if (!GetValue(x, y))
    {
        return false;
    }

    return !GetValue(x, y + 1);
}

