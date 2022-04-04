#include "NonPlayerCharacter.h"
#include "Sound.h"
#include "Random.h"


extern CSpriteManager g_cSpriteManager;
extern CObjectManager g_cObjectManager;
extern CObjectManager g_cWallManager;
extern CObjectManager g_cNPCManager;
extern CObjectManager g_cDeadManager;
extern CSoundManager* g_pSoundManager;
extern CPlayerCharacter* g_cPlayer;
extern CRandom g_cRandom;
extern CTimer g_cTimer;
extern int g_nNPCsKilled;
extern bool g_bPlayerHit;
extern float g_fPitch;

CNonPlayerCharacter::CNonPlayerCharacter(const char* name, const Vector3& location, const Vector3& velocity)
  : CGameObject(NPC_OBJECT, name, location, velocity)
{
  m_eCollisionType = NPC_COLLISION;
}

CNonPlayerCharacter::~CNonPlayerCharacter()
{
}

CWeapon * CNonPlayerCharacter::get_weapon()
{
  return m_pWeapon;
}

void CNonPlayerCharacter::equip_weapon(CWeapon * weapon)
{
  m_pWeapon = weapon;
  m_pWeapon->equipped(this);
  g_cObjectManager.TakeObjectByName((CGameObject*)m_pWeapon);
}

void CNonPlayerCharacter::unequip_weapon()
{
  m_pWeapon->unequipped();
  g_cObjectManager.AddObjectByName(m_pWeapon->get_name(), (CGameObject*)m_pWeapon);
  m_pWeapon = NULL;
}

void CNonPlayerCharacter::dealDamage(float damage)
{
  m_fHealth -= damage;
  if (m_fHealth <= 0)
    kill();
  else
  {
    const int rand = g_cRandom.number(14, 16);
    m_nSoundInstance = g_pSoundManager->play(rand);
    g_pSoundManager->volume(.3);
    if (g_bPlayerHit)
    {
      g_pSoundManager->pitch(.4, -1, -1);
      g_pSoundManager->volume(.5);
    }
    else
      g_pSoundManager->pitch(1, -1, -1);
  }
}

void CNonPlayerCharacter::kill()
{
  const int rand = g_cRandom.number(11, 13);
  m_nSoundInstance = g_pSoundManager->play(rand);
  g_pSoundManager->volume(.2);
  if (g_bPlayerHit)
  {
    g_pSoundManager->pitch(.4, -1, -1);
    g_pSoundManager->volume(.5);
  }
  else
    g_pSoundManager->pitch(1, -1, -1);
  m_eState = DEAD;
  m_bIsDead = true;
  g_nNPCsKilled++;
}

Vector3 CNonPlayerCharacter::Roam()
{
  if (g_cTimer.time() - m_nRoamTime >= 10000)
    setRoaming();
  return m_vDesiredVelocity;
}

Vector3 CNonPlayerCharacter::Chase(CGameObject * target)
{
  m_vTargetVector = target->m_vPos - m_vPos;
  m_vTargetVector.Normalize();
  m_vDesiredVelocity = (m_fRunMultiplier * m_fMoveSpeed) * m_vTargetVector;
  return m_vDesiredVelocity;
}

void CNonPlayerCharacter::setRoaming()
{
  //wait to reroam
  if (g_cTimer.time() - m_nLastReRoamTime < 500)
    return;

  //set state
  m_eState == ROAMING;
  //set timer
  m_nRoamTime = g_cTimer.time();
  m_nLastReRoamTime = g_cTimer.time();
  //set targetVector to random
  m_vTargetVector.x = (double)g_cRandom.number(-100, 100);
  m_vTargetVector.y = (double)g_cRandom.number(-100, 100);
  m_vTargetVector.z = 0;
  m_vTargetVector.Normalize();
  //set desired vector
  m_vDesiredVelocity = m_fMoveSpeed * m_vTargetVector;
  return;
}

void CNonPlayerCharacter::setAlert()
{
  m_eState = ALERT;
  m_cTargetObject = g_cPlayer;
  return;
}

void CNonPlayerCharacter::onCollision(CGameObject *other)
{
    //moved this block to the move function on collision testing
 /* if (m_eState == ROAMING && other->m_eCollisionType == WALL_COLLISION)
    setRoaming();*/
  //DEBUGPRINTF("collision in npc\n");
}

void CNonPlayerCharacter::draw()
{
  if (m_pSprite == nullptr || m_pUpperSprite == nullptr)return;
  if (m_bIsDead)return; //bail if already dead

  int t = m_nFrameInterval;
  if (m_bCycleSprite)
    t = (int)(t / (1.5f + fabs(m_vVelocity.Length())));

  //animation sequence
  if (m_pAnimation != nullptr) { //if there's an animation sequence
                                 //draw current frame
    m_pSprite->Draw(m_vPos, m_fOrientation, m_pAnimation[m_nCurrentFrame]);
    //advance to next frame
    if (m_vVelocity.Length() <= 0.001 && m_nCurrentFrame == 0)//if not moving dont advance past 0
      m_nCurrentFrame = 0;
    else if (g_cTimer.elapsed(m_nLastFrameTime, t)) //if enough time passed and moving
      if (++m_nCurrentFrame >= m_nAnimationFrameCount && m_bCycleSprite) //increment and loop if necessary
        m_nCurrentFrame = 0;
  } //if
  else
    m_pSprite->Draw(m_vPos, m_fOrientation); //assume only one frame

  //draw weapon
  if (m_pWeapon)
  {
    //m_pWeapon->getequippedsprite()->Draw(Vector3(m_vPos.x, m_vPos.y, m_vPos.z - .1), m_fOrientation + PI / 2);
    m_pWeapon->draw();
  }

  //draw top
  m_pUpperSprite->Draw(Vector3(m_vPos.x, m_vPos.y, m_vPos.z-2), m_fOrientation);
}

void CNonPlayerCharacter::move()
{
  const float SCALE = 32.0f; //to scale back motion

  const int time = g_cTimer.time(); //current time
  int tdelta = time - m_nLastMoveTime; //time since last move
  if (tdelta < 0)
    tdelta = 0;
  const float tfactor = tdelta / SCALE; //scaled time factor

  //parse State
  switch (m_eState)
  {
    case IDLE:
      m_vDesiredVelocity = Vector3(0, 0, 0);
      break;
    case ALERT:
      m_vDesiredVelocity = Chase();
      break;
    case ROAMING:
      m_vDesiredVelocity = Roam();
      break;
    case DEAD:
      return;
      break;
  }

  //set velocity using desired velocity
  if (m_vDesiredVelocity != m_vVelocity && m_fAccelerationRate > 0)// if not at the target velocity or the turn rate is too low
  {
    //get delta vector and normalize
    Vector3 deltaVector = m_vDesiredVelocity - m_vVelocity;
    deltaVector.Normalize(deltaVector);
    //apply the acceration rate over time
    m_vVelocity += (m_fAccelerationRate)*tfactor*deltaVector;

    //get a new delta for checking and normalize
    Vector3 newDeltaVector = m_vDesiredVelocity - m_vVelocity;
    newDeltaVector.Normalize(newDeltaVector);

    //check if we passed the desired velocity and if we did then set velocity to desired
    if ((newDeltaVector.x == 0 || deltaVector.x / newDeltaVector.x < 0) &&
      (newDeltaVector.y == 0 || deltaVector.y / newDeltaVector.y < 0))//vector passed desired vector
      m_vVelocity = m_vDesiredVelocity;
  }
  else
    m_vVelocity = m_vDesiredVelocity;

  if (m_vDesiredVelocity == Vector3(0, 0, 0))
  {
    //use m_fOrientation
  }
  //motion
  Vector3 moveDiff = m_vVelocity*tfactor; //position difference
  m_vPos += moveDiff; //move the player
  
  //check collision
  BOOL collid = false;
  vector<Vector3> normals;
  if (g_cWallManager.CheckCollision(this, WALL_COLLISION, normals) ||
      g_cNPCManager.CheckCollisionSimple(this, NPC_COLLISION, normals))
    collid = true;
  
  //check player collision
  Vector3 temp;
  if (g_cPlayer->m_eCollisionType == NPC_COLLISION)
  {
    if (CollisionDetectionSimple(this, g_cPlayer, temp))
    {
      normals.push_back(temp);
      collid = true;
    }
  }

  if(collid)
  {
    if (m_eState == ROAMING)
      setRoaming();
    //move back
    m_vPos -= moveDiff;
    //rotate normal 180 degrees for comparisons with move diff
    for (int i = 0; i < normals.size(); i++)
    {
      //ignore normals within 90 degres of move diff
      if (normals[i].Dot(moveDiff) >= 0)
        continue;
      normals[i].x *= -1;
      normals[i].y *= -1;
      Vector3 projection = Vector3(0, 0, 0);
      normals[i].Normalize(normals[i]);
      projection = normals[i];
      projection = projection.Dot(moveDiff) / projection.Length() * projection;
      moveDiff = moveDiff - projection;
    }
    //move with new diff
    m_vPos += moveDiff;
  }

  //set the orrientation
  if (m_vVelocity.Length() != 0)
  {
    m_fOrientation = atan2(m_vVelocity.y, m_vVelocity.x); //rotation around Z axis
    m_fOrientation += PI;
  }

  //move the gun with the npc
  if (m_pWeapon)
  {
    float angle = m_fOrientation;
    Vector3 offset = Vector3(-cos(angle), -sin(angle), 0);
    offset.Normalize();
    m_pWeapon->set_pos(m_vPos + 40 * offset, m_fOrientation);
  }

  //process weapon
  if (m_pWeapon)
    m_pWeapon->process();
  
  m_nLastMoveTime = time;  //record time of move
}
