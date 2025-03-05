#pragma once
#include "GameObject.h"
#include <array>
#include <cstddef>
#include <DirectXCollision.h>

constexpr byte AIR = 0;
constexpr byte DIRT = 1;
constexpr byte STONE = 2;

constexpr UINT MAP_WIDTH = 800;
constexpr float MAP_RATIO = 0.5;
constexpr UINT MAP_HEIGHT = static_cast<UINT>(MAP_WIDTH * MAP_RATIO);
constexpr UINT MAP_SIZE = MAP_WIDTH * MAP_HEIGHT;

constexpr float OCTAVE_NUM_SAMPLEPOINT_MULTIPLIER = 1.4;
constexpr float OCTAVE_AMPLITUDE_MULTIPLIER = 0.6; 
struct MapCoord
{
    UINT x;
    UINT y;
};

struct MapCoordFloat
{
    float x;
    float y;
};

class Terrain : public GameObject
{
public:
    Terrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext, UINT width, UINT height, float scale, FVector3 pos = {0, 0, 0});
    ~Terrain();

    float Scale;

    void Update(double deltaTime);
    //void Render();
    //void SetOffset(float x, float y);
    
    float GetNormalDownward(XMFLOAT3 scanlinePos, float scanInterval);
    float GetNormalDownwardSimple(XMFLOAT3 scanlinePos);

    float FindSurfaceUpward(XMFLOAT3 pos);
    float FindSurfaceDownward(XMFLOAT3 pos);

    bool CheckCollision(BoundingOrientedBox obox);
    bool CheckCollision(BoundingSphere sphere);
    bool CheckCollision(BoundingBox box);

    void Destroy(XMFLOAT3 pos, float radius);

    // renderings
    void UpdateMapTexture();
    void GetMapTexture();

    void GeneratePlainMap();
    void GenerateRandomMap();
    void GeneratePerlinMap(UINT amplitude, UINT period, UINT octaves = 1);

    XMFLOAT3 RevertToTerrainSpace(XMFLOAT3 worldPos);

    void DestroyOnClickDebug();

private:
    /// <summary>
    /// internal함수는 모두 terrain 좌표계에 맞춰서 작동함.
    /// </summary>
    byte* Map; // Map[x][y] 형식 : y축 방향 먼저 저장
    UINT Width;
    UINT Height;

    float WidthMinusDelta;
    float HeightMinusDelta;

    //Vertex
    void GenerateVerticesNaive();
    void GenerateIndicesNaive();

    float GetNormalSimpleInternal(MapCoord point) const;

    UINT FindSurfaceUpwardInternal(UINT x, float y) const;
    UINT FindSurfaceDownwardInternal(UINT x, float y) const;

    byte GetValue(UINT x, UINT y) const;
    void SetValue(UINT x, UINT y, bool state);
    
    bool IsSurface(MapCoord point) const;
    bool IsInsideMap(MapCoord point) const;
    bool IsInsideMap(MapCoordFloat point) const;

    bool CheckCollisionCircleInternal(float xPos, float yPos, float radius);
    bool CheckCollisionAABBInternal(float xMin, float yMin, float xMax, float yMax);
    bool CheckCollisionOBBInternal(XMFLOAT2 c0, XMFLOAT2 c1, XMFLOAT2 c2, XMFLOAT2 c3);

};



//struct Vertex
//{
//    float x, y, z;
//    float u, v;
//};
//
//Vertex quadVertices[] = {
//    {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
//    {-1.0f,  1.0f, 0.0f, 0.0f, 0.0f},
//    { 1.0f, -1.0f, 0.0f, 1.0f, 1.0f},
//    { 1.0f,  1.0f, 0.0f, 1.0f, 0.0f} 
//};
//
//uint32_t quadIndices[] = {
//     0, 1, 2, 2, 1, 3   };
//
