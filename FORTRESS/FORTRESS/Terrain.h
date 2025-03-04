#pragma once
#include "GameObject.h"
#include <array>
#include <cstddef>

constexpr byte AIR = 0;
constexpr byte DIRT = 1;
constexpr byte STONE = 2;


constexpr unsigned int MAP_WIDTH = 800;
constexpr float MAP_RATIO = 0.5;
constexpr unsigned int MAP_HEIGHT = static_cast<unsigned int>(MAP_WIDTH * MAP_RATIO);
constexpr unsigned int MAP_SIZE = MAP_WIDTH * MAP_HEIGHT;

struct MapCoord
{
    unsigned int x;
    unsigned int y;
};

class Terrain : protected GameObject
{
public:
    Terrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext, unsigned int width, unsigned int height, float scale);
    
    ~Terrain();

    void Update();
    void Render();
    //void SetOffset(float x, float y);
    
    float GetNormal(float x, float y, float scanInterval);
    

    bool CheckCollisionCircle(float _posX, float _posY, float _radius);
    bool CheckCollisionAABB(float _minX, float _minY, float _maxX, float _maxY, float _posX, float _posY);

    // renderings
    void UpdateMapTexture();
    void GetMapTexture();

    void GeneratePlain();


private:
    byte* Map;
    unsigned int Width;
    unsigned int Height;
    float Scale;

    ID3D11Texture2D* MapTexture;
    ID3D11ShaderResourceView* MapTextureRSV;
    //Vertex

    byte GetValue(unsigned int x, unsigned int y) const;
    void SetValue(unsigned int x, unsigned int y, bool state);
    
    unsigned int FindSurfaceUpward(unsigned int x, float y) const;
    unsigned int FindSurfaceDownward(unsigned int x, float y) const;

    bool IsSurface(unsigned int x, unsigned int y) const;
    


    // test
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;
};



struct Vertex
{
    float x, y, z;
    float u, v;
};

Vertex quadVertices[] = {
    {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
    {-1.0f,  1.0f, 0.0f, 0.0f, 0.0f},
    { 1.0f, -1.0f, 0.0f, 1.0f, 1.0f},
    { 1.0f,  1.0f, 0.0f, 1.0f, 0.0f} 
};

uint32_t quadIndices[] = {
     0, 1, 2, 2, 1, 3   };