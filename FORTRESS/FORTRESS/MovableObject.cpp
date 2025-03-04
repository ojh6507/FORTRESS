#include "MovableObject.h"

MovableObject::~MovableObject()
{
    
}

void MovableObject::SetHP(float newHP)
{
    hp = (newHP <= 0) ? 0 : newHP;
    if (hp == 0)
        SetIsDead(true);
}
