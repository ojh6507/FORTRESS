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
    // Scale�� 1�� ����.
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
//    //_deviceContext->DrawIndexed(6, 0, 0);  // ? 6���� �ε����� Quad ������
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

// [x-scanInterval, x+scanInterval]������ slope�� ����
// y���� ray�� ��� ����� ���� -> ���ٴ��� �ƴ�
// x = 0, scanInterval = 1 �ϰ�� [-1,1] �˻�
// scanInterval = 1, �ϰ�� �ڱ�� �翷�� �������� normal ����
// ray�� ���� ��ǥ������ ��
// �׻� ����� ����
float Terrain::GetNormalDownward(XMFLOAT3 scanlinePos, float scanInterval = 1)
{
    XMFLOAT3 scanlinePosInTerrainSpace = RevertToTerrainSpace(scanlinePos);
    float x = scanlinePosInTerrainSpace.x;
    float y = scanlinePosInTerrainSpace.y;

    //// map�� �����Ϸ� ����
    //x = x / Scale;
    //y = y / Scale;

    if (!IsInsideMap(MapCoordFloat({ x,y })))
    {
        return 0.5 * PI;
    }

    int xMin = static_cast<int>(floor(x - scanInterval));
    int xMax = static_cast<int>(ceil(x + scanInterval));

    // map �ȿ� �������� ����
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
        // �� �� ��ǥ�� ���� -> x��ǥ width - 2, y��ǥ height-2�� �� ��ü���� ���� �������� �簢���� bottom-left corner
        if (x > Width - 2 || y > Height - 2)
        {
            continue;
        }
        // �簢���� ����������� �� �𼭸� ��� �־����
        if (GetValue(x, y) && GetValue(x+1,y) && GetValue(x,y+1) && GetValue(x+1,y+1))
        {
            // ���� �Ʒ� �ڳ���ǥ�� vertices�� �ε���
            UINT32 bottomLeft = x * Height + y;
            // ���� �� �ڳ���ǥ�� vertices�� �ε���
            UINT32 upperLeft = x * Height + (y + 1);
            // �������� �ڳ���ǥ�� vertices�� �ε���
            UINT32 upperRight = (x + 1) * Height + (y + 1);
            // ������ �Ʒ� �ڳ���ǥ�� vertices�� �ε���
            UINT32 bottomRight = (x + 1) * Height + y;

            // ���ʾƷ� �ﰢ��
            _indices.push_back(bottomLeft);
            _indices.push_back(upperLeft);
            _indices.push_back(upperRight);

            // ������ �� �ﰢ��
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

    // ������ -���� = �븻
    float normal = atan(-dx / dy);
    // -pi/2 ~ pi/2 ���� 0 ~ pi/2 ������ ����
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
    
    // Width�� �Ǿ������ IndexError �߻� -> Width���� ���� ���� ���� �̿�.
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
            // �ϳ��� ���� ������ ���
            if (GetValue(x, y) || GetValue(x, y + 1) || GetValue(x, y + 2) || GetValue(x, y + 3))
            {
                XMFLOAT4 _ys = { static_cast<float>(y), static_cast<float>(y + 1), static_cast<float>(y + 2), static_cast<float>(y + 3)};
                XMVECTOR ys = XMLoadFloat4(&_ys);
                XMVECTOR dys = XMVectorSubtract(ys, yPoses);
                XMVECTOR dy2s = XMVectorPow(dys, pow2);

                XMVECTOR distances = XMVectorAdd(dx2s, dy2s);
                distances = XMVectorSqrtEst(distances);
                
                XMVECTOR isNear = XMVectorLessOrEqual(distances, radiuses); // �Ÿ��� ���������� ������ 0xFFFFFFFF �ƴϸ� 0x00000000

                // ���� ������ �ƴϸ� �� ��Ʈ�� 1�� (!0)/ ����̹Ƿ� (UINT) -> MSB = 0
                XMFLOAT4 _mask = { static_cast<float>(GetValue(x, y)) , static_cast<float>(GetValue(x, y + 1)) , static_cast<float>(GetValue(x, y + 2)) , static_cast<float>(GetValue(x, y + 3)) };
                XMVECTOR mask = XMLoadFloat4(&_mask);

                // AND operation -> ������ �ƴϰ� �Ÿ��� ���������� ������ �� ��Ʈ�� 1�� (!0). �� MSB = 0
                XMVECTOR isCollisionVector = XMVectorAndInt(isNear, mask); // ���� �ְ� ������ 1 / �ƴϸ� 0
                uint32_t isCollisionVectorRecording = XMVector4GreaterR(isCollisionVector, XMVectorZero()); // ���� �ְ� ������ 1 / �ƴϸ� 0
                bool isCollision = XMComparisonAnyTrue(isCollisionVectorRecording);
                
                //uint32_t isCollisionVectorRecording = XMVector4EqualIntR(isCollisionVector, XMVectorZero()); // ���� �ְ� ������ 0 / �ƴϸ� 1
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
    // terrain ��ǥ �������� scale down�ؼ� ����� (collision box�� ũ�⸦ terrain ��ǥ��� ��ġ��Ŵ) <- XXXX Internal�Լ��� ����
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

        // ������
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

// pos�κ��� radius �Ÿ��� ���� ����
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

    // ����
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
    assert(amplitude <= Height / 4); // amplitude�� �۾ƾ���

    float* heightMapOctave = new float[Width]();
    std::fill(heightMapOctave, heightMapOctave + Width, Height * 0.3); // ��ü ������ 0.3�� �������� ����.

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
//    // sample point�� ����
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
//    // ������ sample point
//    for (int x = stride * (numSamplePoint - 1); x < length; x++)
//    {
//
//    }
//    
//
//}

// �� �������� surface�� ã���ϴ�.
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


// �ش� ��ǥ�� ���̰� �ٷ� ���� �ϴ����� Ȯ��
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

    int* grads = new int[numSameplePoints + 1]; // stride�� �����϶� index �Ѿ ������ �� ���� -> +1 �߰�
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