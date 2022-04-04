#pragma once
#include "Object.h"

class CWeapon : public CGameObject
{
public:
  //constructor/destructor
  CWeapon(ObjectType object, ObjectType object_equipped, const char* name, const Vector3& location, const Vector3& velocity);
  ~CWeapon();

  int m_nBullets;
  int m_nBulletsPerMinute;
  bool active;
  bool m_bMeleeWeapon;
  Vector3 m_fOffset;
  float m_fOrientationOffset;
  
  virtual void draw();
  virtual void move();
  virtual void set_pos(Vector3 owner_pos, float orientation);
  int getBullets();
  void equipped(CGameObject* owner);
  void unequipped();
  bool isequipped();
  const char * get_name();
  C3DSprite* getequippedsprite();
  virtual void activate();
  virtual void deactivate();
  virtual void process();

  C3DSprite* m_pEquippedSprite;
  CGameObject* m_pOwner;
  int m_nLastWeaponTime;
private:
  bool m_bIsEquipped;
};