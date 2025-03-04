#pragma once
#include "GameObject.h"
#include <array>

constexpr unsigned int MAP_WIDTH = 800;
constexpr float MAP_RATIO = 0.5;
constexpr unsigned int MAP_HEIGHT = static_cast<unsigned int>(MAP_WIDTH * MAP_RATIO);
constexpr unsigned int MAP_SIZE = MAP_WIDTH * MAP_HEIGHT;

struct MapCoord
{
    int x;
    int y;
};

class Terrain : public GameObject
{
public:
    Terrain(unsigned int width, unsigned int height, float scale);
    
    //void SetOffset(float x, float y);
    
    float GetNormal(float x, float y, float scanInterval);

    bool CheckCollisionCircle(float _posX, float _posY, float _radius);
    bool CheckCollisionAABB(float _minX, float _minY, float _maxX, float _maxY, float _posX, float _posY);

private:
    bool* Map;
    unsigned int Width;
    unsigned int Height;
    float Scale;

    bool GetValue(unsigned int x, unsigned int y) const;
    void SetValue(unsigned int x, unsigned int y, bool state);
    
    unsigned int FindSurfaceUpward(unsigned int x, float y) const;
    unsigned int FindSurfaceDownward(unsigned int x, float y) const;

    bool IsSurface(unsigned int x, unsigned int y) const;
    

};


