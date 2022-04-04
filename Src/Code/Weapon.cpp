#include "Weapon.h"
#include "SpriteMan.h"
#include "debug.h"
#include "ObjMan.h"
#include "Random.h"
#include "player.h"
#include "NonPlayerCharacter.h"

extern CSpriteManager g_cSpriteManager;
extern CObjectManager g_cObjectManager;
extern CRandom g_cRandom;

CWeapon::CWeapon(ObjectType object, ObjectType object_equipped, const char * name, const Vector3 & location, const Vector3 & velocity)
  : CGameObject(object, name, location, velocity)
{
  m_pEquippedSprite = g_cSpriteManager.GetSprite(object_equipped);
  m_bIsEquipped = false;
  active = false;
  m_eCollisionType = PICKUP_COLLISION;
}

CWeapon::~CWeapon()
{
}

int CWeapon::getBullets()
{
   return m_nBullets;  
}

C3DSprite* CWeapon::getequippedsprite()
{
  return m_pEquippedSprite;
}

void CWeapon::equipped(CGameObject* owner)
{
  m_pOwner = owner;
  m_bIsEquipped = true;
}

void CWeapon::unequipped()
{
  m_bIsEquipped = false;
  m_fOrientation = 0;
  active = false;
  m_pOwner = nullptr;
}

bool CWeapon::isequipped()
{
  return m_bIsEquipped;
}

const char * CWeapon::get_name()
{
  return m_sName;
}

void CWeapon::draw()
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
    if(m_fOffset.x != 0 && m_fOffset.y != 0)
    {
      Vector3 Weapon_pos;
      if (m_pAnimation != nullptr && active)
      {
        Weapon_pos.x = m_fOffset.x * cos(angle) - m_fOffset.y * sin(angle);
        Weapon_pos.y = m_fOffset.x * sin(angle) + m_fOffset.y * cos(angle);
        Weapon_pos.z = -1;
        Weapon_pos += m_vPos;
        getequippedsprite()->Draw(Weapon_pos, angle + PI / 2);
      }
      else
      {
        Weapon_pos.x = m_fOffset.x * cos(angle) - m_fOffset.y * sin(angle);
        Weapon_pos.y = m_fOffset.x * sin(angle) + m_fOffset.y * cos(angle);
        Weapon_pos.z = -1;
        Weapon_pos += m_vPos;
        getequippedsprite()->Draw(Weapon_pos, angle + PI / 3);
      }
    }
    else
    {
      getequippedsprite()->Draw(Vector3(m_pOwner->m_vPos.x, m_pOwner->m_vPos.y, m_pOwner->m_vPos.z - 1), angle + PI /2);
    }
  }
}

void CWeapon::move()
{
}

void CWeapon::set_pos(Vector3 owner_pos, float orientation)
{
  m_vPos.x = owner_pos.x;
  m_vPos.y = owner_pos.y;
  m_vPos.z = owner_pos.z;
  m_fOrientation = orientation - PI/2;
}

void CWeapon::activate()
{
  active = true;
}

void CWeapon::deactivate()
{
  active = false;
}

void CWeapon::process()
{
}
