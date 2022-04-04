#pragma once
#include "NonPlayerCharacter.h"

class CEnemyTestCharacter :
  public CNonPlayerCharacter
{
public:
  CEnemyTestCharacter(const char* name, const Vector3& location, const float run = 0, CWeapon* weapon = nullptr);
  ~CEnemyTestCharacter();

  Vector3 Chase(CGameObject* target = g_cPlayer);
  void move();
  void draw();
  void kill();

private:
  C3DSprite* m_pDeadSprite;
  float m_fEngageDistance;
  float m_fAttackRange;
};

