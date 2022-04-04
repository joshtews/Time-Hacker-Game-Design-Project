#include "SubWeapons.h"
#include "debug.h"
#include "ObjMan.h"
#include "SpriteMan.h"
#include "Random.h"
#include "Sndlist.h"
#include "Sound.h"
#include "timer.h" 
#include "player.h"
#include "Projectile.h"
#include "NonPlayerCharacter.h"

extern CObjectManager g_cObjectManager;
extern CSpriteManager g_cSpriteManager;
extern CObjectManager g_cProjectileManager;
extern CObjectManager g_cNPCManager;
extern CSoundManager* g_pSoundManager;
extern CPlayerCharacter* g_cPlayer;
extern CRandom g_cRandom;
extern CTimer g_cTimer;
extern float g_fPitch;
extern bool g_bPlayerHit;
extern bool g_bPlayerDead;

/*--------------------------------------------CPistol----------------------------------------------*/

CPistol::CPistol(const Vector3& location, const Vector3 & velocity)
  : CWeapon(PISTOL_OBJECT, PISTOL_EQUIPPED, "pistol", location, velocity)
{
  m_fBarrelLength = 20;
  m_fBulletDamage = 10;
  m_nBulletsPerMinute = 170;
  m_nBullets = 30;
  m_fOffset.x = 10;
  m_fOffset.y = 5;
  m_fOffset.z = 0;
  m_bMeleeWeapon = false;
}

CPistol::~CPistol()
{
}

void CPistol::activate()
{
  active = true;
}

void CPistol::deactivate()
{
  active = false;
}

void CPistol::process()
{

  int time = g_cTimer.time(true);
  int tdeltaWeapon = time - m_nLastWeaponTime;
  if (tdeltaWeapon < 0)
  {
    tdeltaWeapon = 0;
    m_nLastWeaponTime = time;
  }
  if (tdeltaWeapon >= m_nBulletsPerMinute && active)
  {
    if (isequipped() && m_nBullets > 0)
    {
      const float fAngle = m_fOrientation + PI / 2;
      const float fSine = sin(fAngle);
      const float fCosine = cos(fAngle);

      //initial bullet position
      const Vector3 s = m_vPos - Vector3(0, 0, 1) + m_fBarrelLength*Vector3(-cos(fAngle), -sin(fAngle), 0);

      //velocity of bullet
      const float BULLETSPEED = 100.0f;
      const float rand = ((float)g_cRandom.number(-5, 5)) / 200;
      Vector3 v = BULLETSPEED * Vector3(-cos(fAngle + rand), -sin(fAngle + rand), 0) + m_vVelocity;
      if (g_cPlayer)
        v += g_cPlayer->m_vVelocity;
      g_cProjectileManager.AddObjectByName("bullet",
        new CProjectile("bullet", s, v, m_fBulletDamage, this)); //create bullet
      m_nSoundInstance = g_pSoundManager->play(PISTOL_SHOT_SOUND);
      if (g_bPlayerHit)
        g_pSoundManager->pitch(.4, -1, -1);
      else
        g_pSoundManager->pitch(1, -1, -1);
      m_nBullets--;
      m_nLastWeaponTime = time;
    }
  }
  deactivate();
}

void CPistol::draw()
{
  if (!isequipped())
    CGameObject::draw();
  else
  {
    float angle;
    if (m_pOwner->m_nObjectType == PLAYER_OBJECT)
      angle = ((CPlayerCharacter*)m_pOwner)->m_fUpperAngle;
    else if (m_pOwner->m_nObjectType == NPC_OBJECT)
      angle = ((CNonPlayerCharacter*)m_pOwner)->m_fOrientation;
    else
      return;
    Vector3 Weapon_pos;
    Weapon_pos.x = m_fOffset.x * cos(angle) - m_fOffset.y * sin(angle);
    Weapon_pos.y = m_fOffset.x * sin(angle) + m_fOffset.y * cos(angle);
    Weapon_pos.z = -1;
    Weapon_pos += m_vPos;
    getequippedsprite()->Draw(Weapon_pos, angle + PI / 2);
  }
}

/*--------------------------------------------CShotgun---------------------------------------------*/

CShotgun::CShotgun(const Vector3& location, const Vector3 & velocity)
  : CWeapon(SHOTGUN_OBJECT, SHOTGUN_EQUIPPED, "shotgun", location, velocity)
{
  m_fBarrelLength = 20;
  m_nBulletsPerMinute = 350;
  m_nBullets = 15;
  active = false;
  m_fBulletDamage = 10;
  m_fOffset.x = 0;
  m_fOffset.y = 0;
  m_fOffset.z = 0;
  m_bMeleeWeapon = false;
}

CShotgun::~CShotgun()
{
}

void CShotgun::activate()
{
  active = true;
}

void CShotgun::deactivate()
{
  active = false;
}

void CShotgun::process()
{
  int time = g_cTimer.time(true);
  int tdeltaWeapon = time - m_nLastWeaponTime;
  if (tdeltaWeapon < 0)
  {
    tdeltaWeapon = 0;
    m_nLastWeaponTime = time;
  }
  if (tdeltaWeapon >= m_nBulletsPerMinute && active)
  {
    if (isequipped() && m_nBullets > 0)
    {
      const float fAngle = m_fOrientation + PI / 2;

      //initial bullet position
      const Vector3 s = m_vPos - Vector3(0, 0, 1) + m_fBarrelLength*Vector3(-cos(fAngle), -sin(fAngle), 0);

      //velocity of bullet
      const float BULLETSPEED = 120.0f;

      for (int i = 0; i < 7; i++)
      {
        float rand = ((float)g_cRandom.number(-60, 60)) / 200;
        float rand2 = ((float)g_cRandom.number(8, 12)) / 10.0;
        Vector3 v = rand2 * BULLETSPEED * Vector3(-cos(fAngle + rand), -sin(fAngle + rand), 0) + m_vVelocity;

        g_cProjectileManager.AddObjectByName("bullet",
          new CProjectile("bullet", s, v, m_fBulletDamage, this)); //create bullet
      }

      m_nSoundInstance = g_pSoundManager->play(SHOTGUN_SHOT_SOUND);
      if (g_bPlayerHit)
        g_pSoundManager->pitch(.4, -1, -1);
      else
        g_pSoundManager->pitch(1, -1, -1);
      m_nBullets--;
      m_nLastWeaponTime = time;
    }
  }
  deactivate();
}

/*----------------------------------------------CLmg-----------------------------------------------*/

CLmg::CLmg(const Vector3& location, const Vector3 & velocity)
  : CWeapon(LMG_OBJECT, LMG_EQUIPPED, "lmg", location, velocity)
{
  m_fBarrelLength = 65;
  m_nBulletsPerMinute = 100;
  m_nBullets = 85;
  m_fBulletDamage = 5;
  m_fOffset.x = 0;
  m_fOffset.y = 0;
  m_fOffset.z = 0;
  active = false;
  m_bMeleeWeapon = false;
}

CLmg::~CLmg()
{
}

void CLmg::activate()
{
  active = true;
}

void CLmg::deactivate()
{
  active = false;
}

void CLmg::process()
{
  int time = g_cTimer.time(true);
  int tdeltaWeapon = time - m_nLastWeaponTime;
  if (tdeltaWeapon < 0)
  {
    tdeltaWeapon = 0;
    m_nLastWeaponTime = time;
  }
  if (tdeltaWeapon >= m_nBulletsPerMinute && active)
  {
    if (isequipped() && m_nBullets > 0)
    {
      const float fAngle = m_fOrientation + PI / 2;

      //initial bullet position
      const Vector3 s = m_vPos - Vector3(0, 0, 1) + m_fBarrelLength*Vector3(-cos(fAngle), -sin(fAngle), 0);

      //velocity of bullet
      const float BULLETSPEED = 100.0f;
      const float rand = ((float)g_cRandom.number(-18, 18)) / 200;
      Vector3 v = BULLETSPEED * Vector3(-cos(fAngle + rand), -sin(fAngle + rand), 0) + m_vVelocity;
      if (g_cPlayer)
        v += g_cPlayer->m_vVelocity;
      g_cProjectileManager.AddObjectByName("bullet",
        new CProjectile("bullet", s, v, m_fBulletDamage, this)); //create bullet
      m_nSoundInstance = g_pSoundManager->play(LMG_SHOT_SOUND);
      g_pSoundManager->volume(.5);
      if (g_bPlayerHit)
        g_pSoundManager->pitch(.4, -1, -1);
      else
        g_pSoundManager->pitch(1, -1, -1);
      m_nBullets--;
      m_nLastWeaponTime = time;
    }
  }
}

/*---------------------------------------------CSlugger--------------------------------------------*/

CSlugger::CSlugger(const Vector3& location, const Vector3 & velocity)
  : CWeapon(SLUGGER_OBJECT, SLUGGER_EQUIPPED, "slugger", location, velocity)
{
  m_nBullets = 0;
  m_nBulletsPerMinute = 200;
  m_fMeleeDamage = 100;
  m_fOffset.x = -23;
  m_fOffset.y = -13;
  m_fOffset.z = 0;
  active = false;
  m_bMeleeWeapon = true;
}

CSlugger::~CSlugger()
{
}

void CSlugger::activate()
{
  if (active)
    return;
  int frames[2] = {0, 1};
  m_pAnimation = frames;
  active = true;
  draw();
  if (m_pOwner->m_nObjectType == PLAYER_OBJECT)
    if(g_cNPCManager.CheckCollision((CGameObject*)this, PICKUP_COLLISION))
    {
      m_nSoundInstance = g_pSoundManager->play(BAT_IMPACT_SOUND);
      if (g_bPlayerHit)
        g_pSoundManager->pitch(.4, -1, -1);
      else
        g_pSoundManager->pitch(1, -1, -1);
    }
    else
    {
      const int rand = g_cRandom.number(6, 7);
      m_nSoundInstance = g_pSoundManager->play(rand);
      g_pSoundManager->volume(.4);
      if (g_bPlayerHit)
        g_pSoundManager->pitch(.4, -1, -1);
      else
        g_pSoundManager->pitch(1, -1, -1);
    }
  else if (m_pOwner->m_nObjectType == NPC_OBJECT)
    if (CollisionDetection((CGameObject*)this, (CGameObject*)g_cPlayer))
      onCollision(g_cPlayer);
  m_nLastFrameTime = g_cTimer.time();
  m_nLastWeaponTime = g_cTimer.time();
}

void CSlugger::onCollision(CGameObject* p)
{
  if (p->m_nObjectType == NPC_OBJECT && m_pOwner->m_nObjectType == PLAYER_OBJECT)
    ((CNonPlayerCharacter*)p)->dealDamage(m_fMeleeDamage);
  if (p->m_nObjectType == PLAYER_OBJECT && m_pOwner->m_nObjectType == NPC_OBJECT && !g_bPlayerDead)
    ((CPlayerCharacter*)p)->dealDamage(m_fMeleeDamage);
}

void CSlugger::draw()
{
  if (!isequipped())
    CGameObject::draw();
  else
  {
    float angle;
    if (m_pOwner->m_nObjectType == PLAYER_OBJECT)
      angle = ((CPlayerCharacter*)m_pOwner)->m_fUpperAngle;
    else if (m_pOwner->m_nObjectType == NPC_OBJECT)
      angle = ((CNonPlayerCharacter*)m_pOwner)->m_fOrientation;
    else
      return;
    Vector3 Weapon_pos;
    if (active)
    {
      Weapon_pos.x = (m_fOffset.x - 20) * cos(angle) - (m_fOffset.y + 20) * sin(angle);
      Weapon_pos.y = (m_fOffset.x - 20) * sin(angle) + (m_fOffset.y + 20) * cos(angle);
      Weapon_pos.z = -1;
      Weapon_pos += m_pOwner->m_vPos;
      m_fOrientation = angle + PI / 4;
      m_vPos = Weapon_pos;
      getequippedsprite()->Draw(m_vPos, m_fOrientation);
    }
    else
    {
      Weapon_pos.x = m_fOffset.x * cos(angle) - m_fOffset.y * sin(angle);
      Weapon_pos.y = m_fOffset.x * sin(angle) + m_fOffset.y * cos(angle);
      Weapon_pos.z = -1;
      Weapon_pos += m_pOwner->m_vPos;
      m_fOrientation = angle + PI / 2;
      m_vPos = Weapon_pos;
      getequippedsprite()->Draw(m_vPos, m_fOrientation);
    }
  }
}

void CSlugger::deactivate()
{
  active = false;
  m_pAnimation = nullptr;
}

void CSlugger::process()
{
  int time = g_cTimer.time(true);
  int tdeltaWeapon = time - m_nLastWeaponTime;
  if (tdeltaWeapon < 0)
  {
    tdeltaWeapon = 0;
    m_nLastWeaponTime = time;
  }
  if (tdeltaWeapon < 50 && active && m_pOwner->m_nObjectType == PLAYER_OBJECT)
  {
    if (m_pOwner->m_nObjectType == PLAYER_OBJECT)
      g_cNPCManager.CheckCollision((CGameObject*)this, PICKUP_COLLISION);
    else if (m_pOwner->m_nObjectType == NPC_OBJECT)
      if (CollisionDetection((CGameObject*)this, (CGameObject*)g_cPlayer))
        onCollision(g_cPlayer);
  }
  if (tdeltaWeapon > 300)
    deactivate();
}

/*---------------------------------------------CKnife---------------------------------------------*/

CKnife::CKnife(const Vector3& location, const Vector3 & velocity)
  : CWeapon(KNIFE_OBJECT, KNIFE_EQUIPPED, "knife", location, velocity)
{
  m_nBullets = 0;
  m_nBulletsPerMinute = 300;
  m_fMeleeDamage = 100;
  active = false;
  m_bMeleeWeapon = true;
  m_fOffset.x = -15;
  m_fOffset.y = 15;
  m_fOffset.z = 0;
}

CKnife::~CKnife()
{
}

void CKnife::activate()
{
  int frames[2] = { 0, 1 };
  m_pAnimation = frames;
  if (!active)
    m_nLastWeaponTime = g_cTimer.time();
  active = true;
  m_nLastFrameTime = g_cTimer.time();
  if (m_pOwner->m_nObjectType == PLAYER_OBJECT)
    if(g_cNPCManager.CheckCollision((CGameObject*)this, PICKUP_COLLISION))
    {
      m_nSoundInstance = g_pSoundManager->play(KNIFE_IMPACT_SOUND);
      if (g_bPlayerHit)
        g_pSoundManager->pitch(.4, -1, -1);
      else
        g_pSoundManager->pitch(1, -1, -1);
    }
    else
    {
      const int rand = g_cRandom.number(9, 10);
      m_nSoundInstance = g_pSoundManager->play(rand);
      g_pSoundManager->volume(.4);
      if (g_bPlayerHit)
        g_pSoundManager->pitch(.4, -1, -1);
      else
        g_pSoundManager->pitch(1, -1, -1);
    }
  else if (m_pOwner->m_nObjectType == NPC_OBJECT)
    if (CollisionDetection((CGameObject*)this, (CGameObject*)g_cPlayer))
      onCollision(g_cPlayer);
}

void CKnife::onCollision(CGameObject* p)
{
  if(p->m_nObjectType == NPC_OBJECT && m_pOwner->m_nObjectType == PLAYER_OBJECT)
    ((CNonPlayerCharacter*)p)->dealDamage(m_fMeleeDamage);
  if (p->m_nObjectType == PLAYER_OBJECT && m_pOwner->m_nObjectType == NPC_OBJECT && !g_bPlayerDead)
    ((CPlayerCharacter*)p)->dealDamage(m_fMeleeDamage);
}

void CKnife::draw()
{
  if (!isequipped())
    CGameObject::draw();
  else
  {
    float angle;
    if(m_pOwner->m_nObjectType == PLAYER_OBJECT)
      angle = ((CPlayerCharacter*)m_pOwner)->m_fUpperAngle;
    else if(m_pOwner->m_nObjectType == NPC_OBJECT)
      angle = ((CNonPlayerCharacter*)m_pOwner)->m_fOrientation;
    else
      return;
    Vector3 Weapon_pos;
    if (m_pAnimation != nullptr && active)
    {
      Weapon_pos.x = (m_fOffset.x - 10) * cos(angle) - m_fOffset.y * sin(angle);
      Weapon_pos.y = (m_fOffset.x - 10) * sin(angle) + m_fOffset.y * cos(angle);
      Weapon_pos.z = -1;
      Weapon_pos += m_pOwner->m_vPos;
      m_fOrientation = angle + PI / 4;
      m_vPos = Weapon_pos;
      getequippedsprite()->Draw(m_vPos, m_fOrientation);
    }
    else
    {
      Weapon_pos.x = m_fOffset.x * cos(angle) - m_fOffset.y * sin(angle);
      Weapon_pos.y = m_fOffset.x * sin(angle) + m_fOffset.y * cos(angle);
      Weapon_pos.z = -1;
      Weapon_pos += m_pOwner->m_vPos;
      m_fOrientation = angle + PI / 3;
      m_vPos = Weapon_pos;
      getequippedsprite()->Draw(m_vPos, m_fOrientation);
    }
  }
}

void CKnife::deactivate()
{
  active = false;
  m_pAnimation = nullptr;
}

void CKnife::process()
{
  int time = g_cTimer.time(true);
  int tdeltaWeapon = time - m_nLastWeaponTime;
  if (tdeltaWeapon < 0)
  {
    tdeltaWeapon = 0;
    m_nLastWeaponTime = time;
  }
  if (tdeltaWeapon < 60 && active && m_pOwner->m_nObjectType == PLAYER_OBJECT)
  {
    if (m_pOwner->m_nObjectType == PLAYER_OBJECT)
      g_cNPCManager.CheckCollision((CGameObject*)this, PICKUP_COLLISION);
    else if (m_pOwner->m_nObjectType == NPC_OBJECT)
      if (CollisionDetection((CGameObject*)this, (CGameObject*)g_cPlayer))
        onCollision(g_cPlayer);
  }
  if (tdeltaWeapon > 300)
    deactivate();
}

/*---------------------------------------------CChicagoSlugger--------------------------------------------*/

CChicagoSlugger::CChicagoSlugger(const Vector3& location, const Vector3 & velocity)
  : CWeapon(CHICAGO_SLUGGER_OBJECT, CHICAGO_SLUGGER_EQUIPPED, "chicagoslugger", location, velocity)
{
  m_nBullets = 0;
  m_nBulletsPerMinute = 200;
  m_fMeleeDamage = 100;
  m_fOffset.x = -23;
  m_fOffset.y = -13;
  m_fOffset.z = 0;
  active = false;
  m_bMeleeWeapon = true;
}

CChicagoSlugger::~CChicagoSlugger()
{
}

void CChicagoSlugger::activate()
{
  if (active)
    return;
  int frames[2] = { 0, 1 };
  m_pAnimation = frames;
  active = true;
  draw();
  if (m_pOwner->m_nObjectType == PLAYER_OBJECT)
    if (g_cNPCManager.CheckCollision((CGameObject*)this, PICKUP_COLLISION))
    {
      m_nSoundInstance = g_pSoundManager->play(BAT_IMPACT_SOUND);
      if (g_bPlayerHit)
        g_pSoundManager->pitch(.4, -1, -1);
      else
        g_pSoundManager->pitch(1, -1, -1);
    }
    else
    {
      const int rand = g_cRandom.number(6, 7);
      m_nSoundInstance = g_pSoundManager->play(rand);
      g_pSoundManager->volume(.4);
      if (g_bPlayerHit)
        g_pSoundManager->pitch(.4, -1, -1);
      else
        g_pSoundManager->pitch(1, -1, -1);
    }
  else if (m_pOwner->m_nObjectType == NPC_OBJECT)
    if (CollisionDetection((CGameObject*)this, (CGameObject*)g_cPlayer))
      onCollision(g_cPlayer);
  m_nLastFrameTime = g_cTimer.time();
  m_nLastWeaponTime = g_cTimer.time();
}

void CChicagoSlugger::onCollision(CGameObject* p)
{
  if (p->m_nObjectType == NPC_OBJECT && m_pOwner->m_nObjectType == PLAYER_OBJECT)
    ((CNonPlayerCharacter*)p)->dealDamage(m_fMeleeDamage);
  if (p->m_nObjectType == PLAYER_OBJECT && m_pOwner->m_nObjectType == NPC_OBJECT && !g_bPlayerDead)
    ((CPlayerCharacter*)p)->dealDamage(m_fMeleeDamage);
}

void CChicagoSlugger::draw()
{
  if (!isequipped())
    CGameObject::draw();
  else
  {
    float angle;
    if (m_pOwner->m_nObjectType == PLAYER_OBJECT)
      angle = ((CPlayerCharacter*)m_pOwner)->m_fUpperAngle;
    else if (m_pOwner->m_nObjectType == NPC_OBJECT)
      angle = ((CNonPlayerCharacter*)m_pOwner)->m_fOrientation;
    else
      return;
    Vector3 Weapon_pos;
    if (active)
    {
      Weapon_pos.x = (m_fOffset.x - 20) * cos(angle) - (m_fOffset.y + 20) * sin(angle);
      Weapon_pos.y = (m_fOffset.x - 20) * sin(angle) + (m_fOffset.y + 20) * cos(angle);
      Weapon_pos.z = -1;
      Weapon_pos += m_pOwner->m_vPos;
      m_fOrientation = angle + PI / 4;
      m_vPos = Weapon_pos;
      getequippedsprite()->Draw(m_vPos, m_fOrientation);
    }
    else
    {
      Weapon_pos.x = m_fOffset.x * cos(angle) - m_fOffset.y * sin(angle);
      Weapon_pos.y = m_fOffset.x * sin(angle) + m_fOffset.y * cos(angle);
      Weapon_pos.z = -1;
      Weapon_pos += m_pOwner->m_vPos;
      m_fOrientation = angle + PI / 2;
      m_vPos = Weapon_pos;
      getequippedsprite()->Draw(m_vPos, m_fOrientation);
    }
  }
}

void CChicagoSlugger::deactivate()
{
  active = false;
  m_pAnimation = nullptr;
}

void CChicagoSlugger::process()
{
  int time = g_cTimer.time(true);
  int tdeltaWeapon = time - m_nLastWeaponTime;
  if (tdeltaWeapon < 0)
  {
    tdeltaWeapon = 0;
    m_nLastWeaponTime = time;
  }
  if (tdeltaWeapon < 120 && active && m_pOwner->m_nObjectType == PLAYER_OBJECT)
  {
    if (m_pOwner->m_nObjectType == PLAYER_OBJECT)
      g_cNPCManager.CheckCollision((CGameObject*)this, PICKUP_COLLISION);
    else if (m_pOwner->m_nObjectType == NPC_OBJECT)
      if (CollisionDetection((CGameObject*)this, (CGameObject*)g_cPlayer))
        onCollision(g_cPlayer);
  }
  if (active && tdeltaWeapon > 300)
    deactivate();
}