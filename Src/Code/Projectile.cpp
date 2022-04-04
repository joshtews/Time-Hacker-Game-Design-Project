#include "Projectile.h"
#include "NonPlayerCharacter.h"
#include "Sndlist.h"
#include "Sound.h"
#include "debug.h"

extern CSoundManager* g_pSoundManager;
extern CObjectManager g_cObjectManager;

CProjectile::CProjectile(const char* name, const Vector3& location, 
                         const Vector3& velocity, float damage, CGameObject * owner)
  : CGameObject(BULLET_OBJECT, name, location, velocity)
{
  m_cOwner =  owner;
  m_fDamage = damage;
  move();
}

CProjectile::~CProjectile()
{
  CGameObject::~CGameObject();
}

void CProjectile::onCollision(CGameObject * other)
{
  if (other == m_cOwner)
    return;
  if (other->m_nObjectType == NPC_OBJECT)
  {
    ((CNonPlayerCharacter*)other)->dealDamage(m_fDamage);
    m_bIsDead = true;
  } 
  else if (other->m_nObjectType == WALL_OBJECT)
  {
    m_bIsDead = true;
    m_nLifeTime = 100;
    m_vVelocity = Vector3(0, 0, 0);
    //m_nSoundInstance = g_pSoundManager->play(BULLET_IMPACT_SOUND);
    //g_pSoundManager->volume(.3);
    //g_pSoundManager->move(m_vPos);
  }
}
void CProjectile::onCollision(CGameObject * other, Vector3 position, Vector3 normal)
{
  if (other == m_cOwner)
    return;

  if (other->m_nObjectType == WALL_OBJECT)
  {
    m_nLifeTime = 100;
    ////reflect bullet
    //normal = normal * -1;
    //normal.Normalize();
    //normal = normal.Dot(m_vVelocity) / normal.Length() * normal;
    //m_vVelocity = m_vVelocity - 2 * normal;
    m_vVelocity = Vector3(0, 0, 0);
    m_vPos = position;
    m_nSoundInstance = g_pSoundManager->play(BULLET_IMPACT_SOUND);
    g_pSoundManager->volume(.3);
    g_pSoundManager->move(m_vPos);
  }
}