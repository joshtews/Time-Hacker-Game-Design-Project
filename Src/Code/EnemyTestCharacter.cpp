#include "EnemyTestCharacter.h"
#include "spriteman.h"
#include "timer.h" 
#include "debug.h"
#include "Random.h"
#include "Weapon.h"
#include "SubWeapons.h"
#include "ObjMan.h"
#include "Collision.h"

extern CSpriteManager g_cSpriteManager;
extern CObjectManager g_cObjectManager;
extern CObjectManager g_cWallManager;
extern CObjectManager g_cNPCManager;
extern CObjectManager g_cDeadManager;
extern CRandom g_cRandom;
extern CTimer g_cTimer;

CEnemyTestCharacter::CEnemyTestCharacter(const char* name, const Vector3& location, const float run, CWeapon* weapon)
  : CNonPlayerCharacter(name, location, Vector3(0,0,0))
{
  m_pWeapon = nullptr;
  m_fAccelerationRate = 1;
  m_eCollisionType = NPC_COLLISION;
  m_fMoveSpeed = 1;
  if (run <= 0)
    m_fRunMultiplier = ((float)g_cRandom.number(5, 11));
  else
    m_fRunMultiplier = run;
  m_pUpperSprite = g_cSpriteManager.GetSprite(ENEMYTEST_UPPER);
  m_pDeadSprite = g_cSpriteManager.GetSprite(ENEMYTEST_DEAD);
  m_pSprite = g_cSpriteManager.GetSprite(ENEMYTEST_OBJECT);
  m_nWidth = m_pUpperSprite->m_nWidth;
  m_nHeight = m_pUpperSprite->m_nHeight;
  m_fHealth = 10;
  m_fEngageDistance = 5000;
  setRoaming();
  if (weapon)
    m_pWeapon = weapon;
  else
    m_pWeapon = new CKnife(m_vPos, Vector3(0,0,0));
  if (m_pWeapon->get_name() == "knife")
    m_fAttackRange = 40;
  if (m_pWeapon->get_name() == "slugger")
    m_fAttackRange = 50;
  m_pWeapon->equipped(this);
}


CEnemyTestCharacter::~CEnemyTestCharacter()
{
}

Vector3 CEnemyTestCharacter::Chase(CGameObject * target)
{
  m_vTargetVector = target->m_vPos - m_vPos;
 
  //if out of range
  if (m_vTargetVector.Length() >= m_fEngageDistance)
  {
    setRoaming();
    return m_vDesiredVelocity;
  }
  //if in attack range
  if (m_pWeapon && m_pWeapon->m_bMeleeWeapon)
    if (m_vTargetVector.Length() <= m_fAttackRange && !m_pWeapon->active)
      m_pWeapon->activate();
  
  m_vTargetVector.Normalize();
  m_vDesiredVelocity = (m_fRunMultiplier * m_fMoveSpeed) * m_vTargetVector;
  return m_vDesiredVelocity;
}

void CEnemyTestCharacter::move()
{
  if ((g_cPlayer->m_vPos - m_vPos).Length() < m_fEngageDistance && m_eState != DEAD)
    setAlert();
  CNonPlayerCharacter::move();
}

void CEnemyTestCharacter::draw()
{
  if (m_eState != DEAD)
    CNonPlayerCharacter::draw();
  else
  {
    m_pUpperSprite->Draw(Vector3(m_vPos.x, m_vPos.y, m_vPos.z), m_fOrientation);
  }
}

void CEnemyTestCharacter::kill()
{
  m_pUpperSprite = m_pDeadSprite;
  m_pSprite = nullptr;
  CNonPlayerCharacter::kill();
}

