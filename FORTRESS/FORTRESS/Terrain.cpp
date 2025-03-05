#include "Terrain.h"
#include <vector>
#include <algorithm>
#include <random>
const float PI = 3.14159265358979;

template <typename T>
T clip(const T& n, const T& lower, const T& upper) {
    return (std::max)(lower, (std::min)(n, upper)); // Use parentheses around std::max and std::min to avoid macro issues
}
void GeneratePerlinNoise1D(size_t numPoints, size_t numSameplePoints, float* heightMap, float scale = 0.5);

Terrain::Terrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext, UINT width, UINT height, float scale, FVector3 pos)
    : GameObject(device, deviceContext), Width(width), Height(height), Scale(scale), 
    WidthMinusDelta(nextafterf(static_cast<float>(width), static_cast<float>(width-1))), HeightMinusDelta(nextafterf(static_cast<float>(height), static_cast<float>(height - 1)))
{
    // Scale은 1로 고정.
    assert(Scale == 1.f);
    // stick to leftbottom corner
    if (pos.x == 0 && pos.y == 0 && pos.z == 0)
    {
        pos = { -FRAME_BUFFER_WIDTH / 2 , -FRAME_BUFFER_HEIGHT / 2, 0 };
    }
    
    _tf.SetPosition(pos);
    _tf.SetRotation({ 0,0,0 });
    _tf.SetScale({scale,scale,scale});

    Map = new byte[height * width]();

    //GenerateRandomMap();
    GeneratePerlinMap(Height/4, Width / 4, 10);

    _vertexBuffer = new VertexBuffer<FVertexSimple>(device);

    _indexBuffer = new IndexBuffer(device);

    UpdateMesh();

}

Terrain::~Terrain()
{
    //if (MapTextureRSV) MapTextureRSV->Release();
    //if (MapTexture) MapTexture->Release();
    delete[] Map;
}

void Terrain::Update(double deltaTime)
{
    DestroyOnClickDebug();

    //UpdateMesh();
}

//void Terrain::Render()
//{
//    //UINT stride = sizeof(Vertex);
//    //UINT offset = 0;
//    //_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
//    //_deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//    //_deviceContext->DrawIndexed(6, 0, 0);  // ? 6개의 인덱스로 Quad 렌더링
//}

//void Terrain::SetOffset(float x, float y)
//{
//
//}


inline byte Terrain::GetValue(UINT x, UINT y) const
{
    //return Map[y * Width + x];
    return Map[x * Height + y];
}

inline void Terrain::SetValue(UINT x, UINT y, bool state)
{
    Map[x * Height + y] = state;
}

// [x-scanInterval, x+scanInterval]에서의 slope를 구함
// y값은 ray를 쏘는 평면의 높이 -> 땅바닥이 아님
// x = 0, scanInterval = 1 일경우 [-1,1] 검사
// scanInterval = 1, 일경우 자기와 양옆을 기준으로 normal 산출
// ray는 정수 좌표에서만 쏨
// 항상 기울기는 위쪽
float Terrain::GetNormalDownward(XMFLOAT3 scanlinePos, float scanInterval = 1)
{
    XMFLOAT3 scanlinePosInTerrainSpace = RevertToTerrainSpace(scanlinePos);
    float x = scanlinePosInTerrainSpace.x;
    float y = scanlinePosInTerrainSpace.y;

    //// map의 스케일로 변경
    //x = x / Scale;
    //y = y / Scale;

    if (!IsInsideMap(MapCoordFloat({ x,y })))
    {
        return 0.5 * PI;
    }

    int xMin = static_cast<int>(floor(x - scanInterval));
    int xMax = static_cast<int>(ceil(x + scanInterval));

    // map 안에 들어오도록 조정
    xMin = std::min<int>(0, xMin);
    xMax = std::max<int>(Width - 1, xMax);
    //xMin = xMin < 0 ? 0 : xMin;
    //xMax = xMax > static_cast<int>(Width - 1) ? static_cast<int>(Width - 1) : xMax;

    float xCenter = (xMax + xMin) / 2.f;
    //float xCenter = (static_cast<float>(xMax) + static_cast<float>(xMin)) / 2.f;
    int numRay = xMax - xMin + 1;
    if (numRay <= 1)
    {
        return 0.f;
    }

    std::vector<MapCoord> heightScanned;
    for (int i = 0; i < numRay; i++)
    {
        UINT scanPoint = xMin + i;
        UINT scanHeight = FindSurfaceDownwardInternal(scanPoint, y);

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

}

float Terrain::GetNormalDownwardSimple(XMFLOAT3 scanlinePos)
{
    XMFLOAT3 scanlinePosInTerrainSpace = RevertToTerrainSpace(scanlinePos);

    float x = scanlinePosInTerrainSpace.x;
    float y = scanlinePosInTerrainSpace.y;

    if (!IsInsideMap(MapCoordFloat({ x,y })))
    {
        return 0.5 * PI;
    }
    return GetNormalSimpleInternal({ static_cast<UINT>(x), static_cast<UINT>(y) });
}

void Terrain::GenerateVerticesNaive()
{
    
    _vertices.resize(Width * Height);
    for (int x = 0; x < Width; x++)
    {
        for (int y = 0; y < Height; y++)
        {
            FVertexSimple v;
            v.x = x;
            v.y = y;
            v.z = 0.f;
            v.r = v.g = v.b = v.a = 1.f;

            _vertices[x * Height + y] = v;
        }
    }

    //_vertexBuffer = new VertexBuffer<FVertexSimple>(_device);
    //_vertexBuffer->Create(_vertices);
}

void Terrain::GenerateIndicesNaive()
{
    for (int i = 0; i < Width * Height; i++)
    {
        UINT x = i / Height;
        UINT y = i % Height;
        // 맨 끝 좌표는 안함 -> x좌표 width - 2, y좌표 height-2가 맵 전체에서 가장 오른쪽위 사각형의 bottom-left corner
        if (x > Width - 2 || y > Height - 2)
        {
            continue;
        }
        // 사각형이 만들어지려면 네 모서리 모두 있어야함
        if (GetValue(x, y) && GetValue(x+1,y) && GetValue(x,y+1) && GetValue(x+1,y+1))
        {
            // 왼쪽 아래 코너좌표의 vertices속 인덱스
            UINT32 bottomLeft = x * Height + y;
            // 왼쪽 위 코너좌표의 vertices속 인덱스
            UINT32 upperLeft = x * Height + (y + 1);
            // 오른쪽위 코너좌표의 vertices속 인덱스
            UINT32 upperRight = (x + 1) * Height + (y + 1);
            // 오른쪽 아래 코너좌표의 vertices속 인덱스
            UINT32 bottomRight = (x + 1) * Height + y;

            // 왼쪽아래 삼각형
            _indices.push_back(bottomLeft);
            _indices.push_back(upperLeft);
            _indices.push_back(upperRight);

            // 오른쪽 위 삼각형
            _indices.push_back(bottomLeft);
            _indices.push_back(upperRight);
            _indices.push_back(bottomRight);

        }
    }

    //_indexBuffer = new IndexBuffer(device);
    //_indexBuffer->Create(_indices);

}

float Terrain::GetNormalSimpleInternal(MapCoord point) const
{
    UINT x = point.x;
    UINT y = point.y;

    MapCoord left = x > 0 ? MapCoord{x-1, FindSurfaceDownwardInternal(x-1, y)} : MapCoord{x, FindSurfaceDownwardInternal(x, y)};
    MapCoord right = x < Width ? MapCoord{ x + 1, FindSurfaceDownwardInternal(x + 1, y) } : MapCoord{ x, FindSurfaceDownwardInternal(x, y) };
    
    float dx = right.x - left.x;
    float dy = right.y - left.y;

    // 기울기의 -역수 = 노말
    float normal = atan(-dx / dy);
    // -pi/2 ~ pi/2 에서 0 ~ pi/2 범위로 수정
    normal = normal > 0 ? normal : normal + PI;
    return normal;
}

float Terrain::FindSurfaceUpward(XMFLOAT3 pos)
{
    XMFLOAT3 scanlinePosInTerrainSpace = RevertToTerrainSpace(pos);
    float xfInternal = scanlinePosInTerrainSpace.x;
    float yfInternal = scanlinePosInTerrainSpace.y;

    if (!IsInsideMap(MapCoordFloat({ xfInternal, yfInternal })))
    {
        return -1;
    }

    UINT xInternal = static_cast<UINT>(xfInternal);
    UINT yInternal = static_cast<UINT>(yfInternal);

    if (!IsInsideMap(MapCoord({ xInternal, yInternal })))
    {
        return -1;
    }

    return FindSurfaceUpwardInternal(xInternal, yfInternal);
}
        //return FindSurfaceUpwardInternal();

float Terrain::FindSurfaceDownward(XMFLOAT3 pos)
{
    XMFLOAT3 scanlinePosInTerrainSpace = RevertToTerrainSpace(pos);

    float xfInternal = scanlinePosInTerrainSpace.x;
    float yfInternal = scanlinePosInTerrainSpace.y;

    if (!IsInsideMap(MapCoordFloat({ xfInternal, yfInternal })))
    {
        return -1;
    }

    UINT xInternal = static_cast<UINT>(xfInternal);
    UINT yInternal = static_cast<UINT>(yfInternal);

    if (!IsInsideMap(MapCoord({ xInternal, yInternal })))
    {
        return -1;
    }

    return FindSurfaceDownwardInternal(xInternal, yfInternal);
}

bool Terrain::CheckCollision(BoundingOrientedBox obox)
{

    return false;
}

bool Terrain::CheckCollision(BoundingSphere sphere)
{
    XMFLOAT3 center = sphere.Center;
    float radius = sphere.Radius;

    center = RevertToTerrainSpace(center);
    radius = radius / Scale;

    return CheckCollisionCircleInternal(center.x, center.y, radius);
}

bool Terrain::CheckCollision(BoundingBox box)
{
    XMFLOAT3 corners[BoundingBox::CORNER_COUNT];
    box.GetCorners(corners);

    float xMin, yMin, xMax, yMax;
    xMin = xMax = box.Center.x;
    yMin = yMax = box.Center.y;
    for (int i = 1; i < BoundingBox::CORNER_COUNT; i++)
    {
        float x = corners[i].x;
        float y = corners[i].y;

        xMin = min(x, xMin);
        xMax = max(x, xMax);
        yMin = min(y, yMin);
        yMax = max(y, yMax);
    }

    return CheckCollisionAABBInternal(xMin, yMin, xMax, yMax);
}

const XMVECTOR pow2 = XMVectorReplicate(2.f);
bool Terrain::CheckCollisionCircleInternal(float xPos, float yPos, float radius)
{
    //float leftBorder = static_cast<int>((xPos - radius)/Scale);
    //float rightBorder = static_cast<int>((xPos + radius)/Scale);
    //float downBorder = static_cast<int>((yPos - radius)/Scale);
    //float upperBorder = static_cast<int>((yPos + radius)/Scale);
    //float leftBorder = (xPos - radius) / Scale;
    //float rightBorder = (xPos + radius) / Scale;
    //float downBorder = (yPos - radius) / Scale;
    //float upperBorder = (yPos + radius) / Scale;
    
    // Width가 되어버리면 IndexError 발생 -> Width보다 조금 작은 값을 이용.
    //leftBorder = clip<float>(leftBorder, 0, WidthMinusDelta);
    //rightBorder = clip<float>(rightBorder, 0, WidthMinusDelta);
    //downBorder = clip<float>(downBorder, 0, HeightMinusDelta);
    //upperBorder = clip<float>(upperBorder, 0, HeightMinusDelta);

    float leftBorder = clip<float>(xPos - radius, 0, WidthMinusDelta);
    float rightBorder = clip<float>(xPos + radius, 0, WidthMinusDelta);
    float downBorder = clip<float>(yPos - radius, 0, HeightMinusDelta);
    float upperBorder = clip<float>(yPos + radius, 0, HeightMinusDelta);

    if (!CheckCollisionAABBInternal(leftBorder, rightBorder, downBorder, upperBorder))
    {
        return false;
    }

    XMVECTOR yPoses = XMVectorReplicate(yPos);
    XMVECTOR radiuses = XMVectorReplicate(radius);

    for (int x = leftBorder; x < rightBorder; x++)
    {
        float dx = xPos - x;
        float dx2 = dx * dx;
        XMVECTOR dx2s = XMVectorReplicate(dx2);
        
        for (int y = downBorder; y < upperBorder; y += 4)
        {
            // 하나라도 땅이 있으면 계산
            if (GetValue(x, y) || GetValue(x, y + 1) || GetValue(x, y + 2) || GetValue(x, y + 3))
            {
                XMFLOAT4 _ys = { static_cast<float>(y), static_cast<float>(y + 1), static_cast<float>(y + 2), static_cast<float>(y + 3)};
                XMVECTOR ys = XMLoadFloat4(&_ys);
                XMVECTOR dys = XMVectorSubtract(ys, yPoses);
                XMVECTOR dy2s = XMVectorPow(dys, pow2);

                XMVECTOR distances = XMVectorAdd(dx2s, dy2s);
                distances = XMVectorSqrtEst(distances);
                
                XMVECTOR isNear = XMVectorLessOrEqual(distances, radiuses); // 거리가 반지름보다 가까우면 0xFFFFFFFF 아니면 0x00000000

                // 만약 공중이 아니면 한 비트라도 1임 (!0)/ 양수이므로 (UINT) -> MSB = 0
                XMFLOAT4 _mask = { static_cast<float>(GetValue(x, y)) , static_cast<float>(GetValue(x, y + 1)) , static_cast<float>(GetValue(x, y + 2)) , static_cast<float>(GetValue(x, y + 3)) };
                XMVECTOR mask = XMLoadFloat4(&_mask);

                // AND operation -> 공중이 아니고 거리가 반지름보다 가까우면 한 비트라도 1임 (!0). 단 MSB = 0
                XMVECTOR isCollisionVector = XMVectorAndInt(isNear, mask); // 땅이 있고 가까우면 1 / 아니면 0
                uint32_t isCollisionVectorRecording = XMVector4GreaterR(isCollisionVector, XMVectorZero()); // 땅이 있고 가까우면 1 / 아니면 0
                bool isCollision = XMComparisonAnyTrue(isCollisionVectorRecording);
                
                //uint32_t isCollisionVectorRecording = XMVector4EqualIntR(isCollisionVector, XMVectorZero()); // 땅이 있고 가까우면 0 / 아니면 1
                //bool isCollision = XMComparisonAnyFalse(isCollisionVectorRecording);
               
                if (isCollision)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Terrain::CheckCollisionAABBInternal(float xMin, float yMin, float xMax, float yMax)
{
    // terrain 좌표 기준으로 scale down해서 계산함 (collision box의 크기를 terrain 좌표계와 일치시킴) <- XXXX Internal함수로 변경
    //float leftBorder = static_cast<int>(xMin / Scale);
    //float rightBorder = static_cast<int>(xMax / Scale);
    //float downBorder = static_cast<int>(yMin / Scale);
    //float upperBorder = static_cast<int>(yMax / Scale);

    //leftBorder = clip<float>(leftBorder, 0, Width - 1);
    //rightBorder = clip<float>(rightBorder, 0, Width - 1);
    //downBorder = clip<float>(downBorder, 0, Height - 1);
    //upperBorder = clip<float>(upperBorder, 0, Height - 1);

    float leftBorder = clip<float>(xMin, 0, WidthMinusDelta);
    float rightBorder = clip<float>(xMax, 0, WidthMinusDelta);
    float downBorder = clip<float>(yMin, 0, HeightMinusDelta);
    float upperBorder = clip<float>(yMax, 0, HeightMinusDelta);

    for (int x = std::ceil(xMin); x < xMax; x++)
    {
        for (int y = std::ceil(yMin); y < yMax; y += 4)
        {
            //XMFLOAT4 _samplePoints = { static_cast<float>(y), static_cast<float>(y + 1), static_cast<float>(y + 2), static_cast<float>(y + 3) };
            //XMVECTOR samplePoints = XMLoadFloat4(&_samplePoints);

            XMFLOAT4 _mask = { static_cast<float>(GetValue(x, y)) , static_cast<float>(GetValue(x, y + 1)) , static_cast<float>(GetValue(x, y + 2)) , static_cast<float>(GetValue(x, y + 3)) };
            XMVECTOR mask = XMLoadFloat4(&_mask);

            uint32_t isCollisionVectorRecording = XMVector4GreaterR(mask, XMVectorZero());
            bool isCollision = XMComparisonAnyTrue(isCollisionVectorRecording);
            if (isCollision)
            {
                return true;
            }
        }

        // 나머지
        for (int y = (static_cast<int>(yMax) / 4) * 4; y < yMax; y++)
        {
            if (GetValue(x, y))
            {
                return true;
            }
        }
    }
    return false;
}

bool Terrain::CheckCollisionOBBInternal(XMFLOAT2 c0, XMFLOAT2 c1, XMFLOAT2 c2, XMFLOAT2 c3)
{
    return false;
}

// pos로부터 radius 거리의 값을 지움
void Terrain::Destroy(XMFLOAT3 pos, float radius)
{
    XMFLOAT3 posInTerrainSpace = RevertToTerrainSpace(pos);
    float radiusInternal = radius / Scale;

    float xInternal = posInTerrainSpace.x;
    float yInternal = posInTerrainSpace.y;
    
    UINT leftBorder = clip<int>( static_cast<int>( floor(xInternal - radiusInternal)), 0, static_cast<int>(Width - 1));
    UINT rightBorder = clip<int>(static_cast<int>(ceil(xInternal + radiusInternal)), 0, static_cast<int>(Width - 1));
    UINT bottomBorder = clip<int>(static_cast<int>(floor(yInternal - radiusInternal)), 0, static_cast<int>(Height - 1));
    UINT upperBorder = clip<int>(static_cast<int>(ceil(yInternal + radiusInternal)), 0, static_cast<int>(Height - 1));

    for (int x = leftBorder; x < rightBorder; x++)
    {
        for (int y = bottomBorder; y < upperBorder; y++)
        {
            float distanceSquared = (x - xInternal) * (x - xInternal) + (y - yInternal) * (y - yInternal);
            static const float radiusSquared = radiusInternal * radiusInternal;
            if (distanceSquared < radiusSquared)
            {
                SetValue(x, y, 0);
            }
        }
    }

    UpdateMesh();
}



//bool Terrain::CheckCollisionOBB(float _posX, float _posY, float rotation)
//{
//    return false;
//}


void Terrain::UpdateMesh()
{
    assert(Scale == 1);
    _vertices.clear();
    _indices.clear();

    GenerateVerticesNaive();
    GenerateIndicesNaive();

    _vertexBuffer->Create(_vertices);

    _indexBuffer->Create(_indices);
}

void Terrain::GeneratePlainMap()
{
    int halfHeight = Height / 2;
    for (int x = 0; x < Width; x++)
    {
        for (int y = 0; y < halfHeight; y++)
        {
            SetValue(x, y, 1);
        }
        for (int y = halfHeight; y < Height; y++)
        {
            SetValue(x, y, 0);
        }
    }

}

void Terrain::GenerateRandomMap()
{
    int halfHeight = Height / 2;
    int amplitude = halfHeight / 4;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(halfHeight - amplitude, halfHeight + amplitude);
    
    UINT* heightMap = new UINT[Width]();

    // 래덤
    for (int x = 0; x < Width; x++)
    {
        heightMap[x] = distrib(gen);
    }

    // smoothing

    for (int n = 0; n < 50; n++)
    {
        heightMap[0] = heightMap[1];
        heightMap[Width - 1] = heightMap[Width - 2];
        for (int x = 1; x < Width - 2; x++)
        {
            heightMap[x] = (heightMap[x - 1] + heightMap[x] + heightMap[x + 1]) / 3;
        }
    }

    for (int x = 0; x < Width; x++)
    {

        for (int y = 0; y < heightMap[x]; y++)
        {
            SetValue(x, y, 1);
        }
    }

    delete[] heightMap;

}

void Terrain::GeneratePerlinMap(UINT amplitude, UINT period, UINT octaves)
{
    assert(amplitude <= Height / 4); // amplitude는 작아야함

    float* heightMapOctave = new float[Width]();
    std::fill(heightMapOctave, heightMapOctave + Width, Height * 0.3); // 전체 높이의 0.3을 기준으로 시작.

    float* heightMapPerStep = new float[Width];
    float currentAmplitude = amplitude;
    int numSamplePoint = Width / period;
    for (int oct = 0; oct < octaves; oct++)
    {
        GeneratePerlinNoise1D(Width, numSamplePoint, heightMapPerStep, currentAmplitude);

        numSamplePoint *= OCTAVE_NUM_SAMPLEPOINT_MULTIPLIER;
        currentAmplitude *= OCTAVE_AMPLITUDE_MULTIPLIER;

        for (int i = 0; i < Width; i++)
        {
            heightMapOctave[i] += heightMapPerStep[i];
        }
    }

    for (int x = 0; x < Width; x++)
    {

        for (int y = 0; y < heightMapOctave[x]; y++)
        {
            SetValue(x, y, 1);
        }
    }
    delete[] heightMapOctave;
    delete[] heightMapPerStep;

}

XMFLOAT3 Terrain::RevertToTerrainSpace(XMFLOAT3 worldPos)
{
    worldPos.x -= _tf.GetPosition().x;
    worldPos.y -= _tf.GetPosition().y;
    worldPos.z -= _tf.GetPosition().z;

    return worldPos;

    XMFLOAT4X4 worldTransform = _tf.GetWorldMatrix();
    XMMATRIX world = XMLoadFloat4x4(&worldTransform);

    XMMATRIX worldInv = XMMatrixInverse(nullptr, world);

    XMVECTOR worldPosVector = XMLoadFloat3(&worldPos);
    XMVECTOR modelPos = XMVector3TransformCoord(worldPosVector, worldInv);

    XMFLOAT3 modelPosFloat;
    XMStoreFloat3(&modelPosFloat, modelPos);
    
    return modelPosFloat;
}

void Terrain::DestroyOnClickDebug()
{
    static int x = -200;
    if (Input::Instance()->IsMouseButtonPressed(0))
    {
        Destroy({ (float)x,-200,0 }, 20);
        x += 100;
    }
}

//void GeneratePerlinNoise1D(size_t stride, int amplitude, int* heightMap, UINT length)
//{
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_int_distribution<> distrib(-1, 1);
//
//    int numSamplePoint = static_cast<int>(ceil(length / static_cast<float>(stride)));
//    int* samplePoints = new int[numSamplePoint];
//    for (int i = 0; i < numSamplePoint; i++)
//    {
//        samplePoints[i] = amplitude * distrib(gen);
//    }
//    // sample point만 잡음
//     
//    for (int i = 0; i < numSamplePoint - 1; i++)
//    {
//        heightMap[i * stride] = samplePoints[i];
//        for (int dx = 1; dx < stride; dx++)
//        {
//            int x = stride * i + dx;
//            float distnaceVectorLeft = dx / static_cast<float>(stride);
//            float distanceVectorRight = (stride - dx) / static_cast<float>(stride);
//            
//            float weightLeft = distnaceVectorLeft * samplePoints[i];
//            float weightLeft = distanceVectorRight * samplePoints[i+1];
//
//            int interpolationRatio = 6 * powf(distnaceVectorLeft, 6.f) - 15 * powf(distnaceVectorLeft, )
//        }
//    }
//
//    // 마지막 sample point
//    for (int x = stride * (numSamplePoint - 1); x < length; x++)
//    {
//
//    }
//    
//
//}

// 윗 방향으로 surface를 찾습니다.
UINT Terrain::FindSurfaceUpwardInternal(UINT x, float y) const
{
    for (int sampleY = static_cast<int>(ceil(y)); sampleY < Height; sampleY++)
    {
        if (IsSurface({ x, static_cast<UINT>(sampleY) }))
        {
            return sampleY;
        }
    }
    return Height - 1;
}

UINT Terrain::FindSurfaceDownwardInternal(UINT x, float y) const
{
    if (!IsInsideMap(MapCoordFloat({ static_cast<float>(x), y })))
    {
        return 0;
    }

    for (int sampleY = static_cast<int>(floor(y)); sampleY >= 0; sampleY--)
    {
        if (IsSurface({ x, static_cast<UINT>(sampleY) }))
        {
            return sampleY;
        }
    }
    return 0;
}


// 해당 좌표가 땅이고 바로 위가 하늘인지 확인
bool Terrain::IsSurface(MapCoord point) const
{
    if (!GetValue(point.x, point.y))
    {
        return false;
    }

    return !GetValue(point.x, point.y + 1);
}

bool Terrain::IsInsideMap(MapCoord point) const
{
    if (point.x < 0 || point.x >= Width)
    {
        return false;
    }
    if (point.y < 0 || point.y >= Height)
    {
        return false;
    }
    return true;
}

bool Terrain::IsInsideMap(MapCoordFloat point) const
{
    if (point.x < 0 || point.x >= Width)
    {
        return false;
    }
    if (point.y < 0 || point.y >= Height)
    {
        return false;
    }
    return true;
}

void GeneratePerlinNoise1D(size_t numPoints, size_t numSameplePoints, float* heightMap, float scale)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 1);

    int* grads = new int[numSameplePoints + 1]; // stride가 정수일때 index 넘어서 접근할 수 있음 -> +1 추가
    //float* heightMap = new float[numPoints];
    float stride = numSameplePoints / (float)numPoints;

    for (int i = 0; i < numSameplePoints + 1; i++)
    {
        grads[i] = distrib(gen) * 2 - 1;
    }

    for (int i = 0; i < numPoints; i++)
    {
        float point = i * stride;
        int leftSamplePointIndex = static_cast<int>(point);

        float distLeft = point - floor(point);
        float distRight = distLeft - 1; // point - floor(point + 1)

        float influenceLeft = distLeft * grads[leftSamplePointIndex];
        float influenceRight = distRight * grads[leftSamplePointIndex + 1];

        float dx = distLeft;
        float t = 6 * powf(dx, 5.f) - 15 * powf(dx, 4.f) + 10 * powf(dx, 3.f);

        float interpolated = influenceLeft * (1 - t) + influenceRight * t;

        heightMap[i] = interpolated * scale;
    }

    delete[] grads;
}