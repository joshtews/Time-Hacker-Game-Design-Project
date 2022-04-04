#pragma once
#include "Object.h"
#include "Defines.h"


class CProjectile : public CGameObject
{
  
public:
  CProjectile(const char* name, const Vector3& location, 
              const Vector3& velocity, float damage, CGameObject* owner);
  ~CProjectile();

  virtual void onCollision(CGameObject* other);
  virtual void onCollision(CGameObject* other, Vector3 pos, Vector3 normal);

  CGameObject* m_cOwner;
  float        m_fDamage;
};