#pragma once
#include "Object.h"

class CPlayerCharacter : public CGameObject
{
friend class CWeapon;

public:
  //constructor/destructor
  CPlayerCharacter(const char* name, const Vector3& location, const Vector3& velocity);
  ~CPlayerCharacter();

  //functions
  void setDesiredVelocity(Vector3 velocity);
  void setVelocity(Vector3 velocity);

  Vector3 getVelocity( void );
  Vector3 getDesiredVelocity(void);
  float getMoveSpeed(void);
  void dealDamage(float damage);
  int getLives();

  void draw();
  void move();
  CWeapon* get_weapon();
  void equip_weapon(CWeapon * weapon);
  void unequip_weapon();

  float m_fUpperAngle;

  private:
  //variables
  float m_fHealth;
  int   m_nLives;
  float m_fMoveSpeed;
  int m_nTurnSpeed;
  CWeapon * m_pWeapon;

  Vector3 m_vDesiredVelocity; //desired velocity vector for interpolating
  C3DSprite* m_pUpperSprite; //upperbody sprite
};
