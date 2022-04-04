#pragma once
#include "Weapon.h"

/*--------------------------------------------CPistol----------------------------------------------*/

class CPistol : public CWeapon
{
public:
  CPistol(const Vector3& location, const Vector3& velocity);
  ~CPistol();

  virtual void activate();
  virtual void deactivate();
  virtual void process();
  void draw();
  float m_fBarrelLength;
  float m_fBulletDamage;

private:
};

/*--------------------------------------------CShotgun---------------------------------------------*/

class CShotgun : public CWeapon
{
public:
  CShotgun(const Vector3& location, const Vector3& velocity);
  ~CShotgun();
  
  virtual void activate();
  virtual void deactivate();
  virtual void process();
  float m_fBarrelLength;
  float m_fBulletDamage;
 
private:
};

/*----------------------------------------------CLmg-----------------------------------------------*/

class CLmg : public CWeapon
{
public:
  CLmg(const Vector3& location, const Vector3& velocity);
  ~CLmg();

  virtual void activate();
  virtual void deactivate();
  virtual void process();
  float m_fBarrelLength;
  float m_fBulletDamage;

private:
};

/*---------------------------------------------CSlugger--------------------------------------------*/

class CSlugger : public CWeapon
{
public:
  CSlugger(const Vector3& location, const Vector3& velocity);
  ~CSlugger();

  virtual void activate();
  virtual void deactivate();
  virtual void process();
  virtual void onCollision(CGameObject*);
  void draw();

  float m_fMeleeDamage;

private:
};

/*---------------------------------------------CKnife---------------------------------------------*/

class CKnife : public CWeapon
{
public:
  CKnife(const Vector3& location, const Vector3& velocity);
  ~CKnife();

  virtual void activate();
  virtual void deactivate();
  virtual void process();
  virtual void onCollision(CGameObject*);
  void draw();
  float m_fMeleeDamage;

private:
};

/*---------------------------------------------CChicagoSlugger--------------------------------------------*/

class CChicagoSlugger : public CWeapon
{
public:
  CChicagoSlugger(const Vector3& location, const Vector3& velocity);
  ~CChicagoSlugger();

  virtual void activate();
  virtual void deactivate();
  virtual void process();
  virtual void onCollision(CGameObject*);
  void draw();

  float m_fMeleeDamage;

private:
};