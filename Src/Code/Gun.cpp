#include "Gun.h"
#include "SpriteMan.h"
#include "debug.h"


CGun::CGun(const char * name, const Vector3 & location, const Vector3 & velocity) : CGameObject(GUN_OBJECT, name, location, velocity)
{
}

CGun::~CGun()
{
}

void CGun::draw()
{
  CGameObject::draw();
}

void CGun::move()
{
}
