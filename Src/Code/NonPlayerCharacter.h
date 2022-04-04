#pragma once
#include "Object.h"
#include "Weapon.h"
#include "spriteman.h"
#include "timer.h" 
#include "debug.h"
#include "ObjMan.h"
#include "Collision.h"
#include "Random.h"

extern CPlayerCharacter* g_cPlayer;
extern CObjectManager    g_cDeadManager;

class CNonPlayerCharacter :
  public CGameObject
{
public:
  CNonPlayerCharacter(const char* name, const Vector3& location, const Vector3& velocity);
  ~CNonPlayerCharacter();

  virtual void draw();
  virtual void move();
  CWeapon* get_weapon();
  void equip_weapon(CWeapon * weapon);
  void unequip_weapon();
  void dealDamage(float damage);
  virtual void kill();
  virtual Vector3 Roam();
  virtual Vector3 Chase(CGameObject* target = g_cPlayer);
  virtual void setRoaming();
  virtual void setAlert();
  virtual void onCollision(CGameObject*);

protected:
  //variables
  float m_fHealth;     //NPC Health
  float m_fMoveSpeed;
  float m_fRunMultiplier;
  Vector3 m_vTargetVector;
  CGameObject* m_cTargetObject;
  int m_nRoamTime;
  int m_nLastReRoamTime;
  NPCState m_eState;   //State used for AI
  CWeapon* m_pWeapon;

  Vector3 m_vDesiredVelocity; //desired velocity vector for interpolating
  float m_fDesiredOrientation; //orientation if there is not move Velocity
  C3DSprite* m_pUpperSprite; //upperbody sprite
};

