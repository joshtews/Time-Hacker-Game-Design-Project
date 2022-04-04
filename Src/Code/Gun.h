#pragma once
#include "Object.h"

class CGun : public CGameObject
{
public:
  //constructor/destructor
  CGun(const char* name, const Vector3& location, const Vector3& velocity);
  ~CGun();

  virtual void draw();
  virtual void move();

private:
  //variables
  
};