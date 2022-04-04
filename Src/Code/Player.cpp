#include "Player.h"
#include "spriteman.h"
#include "timer.h" 
#include "debug.h"
#include "Weapon.h"
#include "ObjMan.h"
#include "Collision.h"
#include "Sound.h"

extern CSpriteManager g_cSpriteManager;
extern CObjectManager g_cObjectManager;
extern CObjectManager g_cWallManager;
extern CSoundManager * g_pSoundManager;
extern CTimer g_cTimer;
extern bool g_bPlayerDead;
extern bool g_bPlayerHit;

CPlayerCharacter::CPlayerCharacter( const char* name, const Vector3& location, const Vector3& velocity)
	: CGameObject(PLAYER_OBJECT, name, location, velocity)
{
  m_pWeapon = nullptr;
  m_eCollisionType = NPC_COLLISION;
  m_fMoveSpeed = 15;
  m_fAccelerationRate = 4;
  m_pUpperSprite = g_cSpriteManager.GetSprite(PLAYER_UPPER);
  m_fUpperAngle = PI;
  m_nWidth = m_nWidth / 2;
  m_nHeight = m_nHeight / 2 ;
  m_fHealth = 100;
  m_nLives = 5;
  m_pWeapon = (CWeapon *)g_cObjectManager.createObject(CHICAGO_SLUGGER_OBJECT, "chicagoslugger", Vector3(0, 0, 0), Vector3(0, 0, 0), 0.0f);
  m_pWeapon = (CWeapon *)g_cObjectManager.TakeObjectByName((CGameObject*)m_pWeapon);
  m_pWeapon->equipped(this);
}

CPlayerCharacter::~CPlayerCharacter()
{
}

void CPlayerCharacter::setDesiredVelocity(Vector3 velocity)
{
    //normilize velocity
	m_vDesiredVelocity = velocity;
	/*if (velocity != Vector3(0, 0, 0))
	{
        velocity.Normalize(velocity);
		DEBUGPRINTF("x=%f \ny=%f\n", velocity.x, velocity.y);
	}*/
}

void CPlayerCharacter::setVelocity(Vector3 velocity)
{
	m_vVelocity = velocity;
}
  
Vector3 CPlayerCharacter::getDesiredVelocity(void)
{
	return m_vDesiredVelocity;
}

Vector3 CPlayerCharacter::getVelocity(void)
{
  return m_vVelocity;
}

float CPlayerCharacter::getMoveSpeed(void)
{
  return m_fMoveSpeed;
}

void CPlayerCharacter::dealDamage(float damage)
{
  m_fHealth -= damage;
  if(m_fHealth <= 0)
  {
    m_nLives--;
    m_fHealth = 100;
    if(m_nLives == 0)
      g_bPlayerDead = true;
    else
      g_bPlayerHit = true;
  }
  DEBUGPRINTF("Player Health before: %f\n", m_fHealth);

  m_fHealth -= damage;

  DEBUGPRINTF("Player Health after: %f\n", m_fHealth);
}

int CPlayerCharacter::getLives()
{
  return m_nLives;
}

void CPlayerCharacter::equip_weapon(CWeapon * weapon)
{
  m_pWeapon = weapon;
  m_pWeapon->equipped(this);
  g_cObjectManager.TakeObjectByName((CGameObject*)m_pWeapon);
  m_nSoundInstance = g_pSoundManager->play(EQUIP_SOUND);
  g_pSoundManager->volume(.7);
  if (g_bPlayerHit)
    g_pSoundManager->pitch(.4, -1, -1);
  else
    g_pSoundManager->pitch(1, -1, -1);
}

void CPlayerCharacter::unequip_weapon()
{
  m_pWeapon->m_vPos = m_vPos;
  m_pWeapon->unequipped();
  m_nSoundInstance = g_pSoundManager->play(UNEQUIP_SOUND);
  g_pSoundManager->volume(.7);
  if (g_bPlayerHit)
    g_pSoundManager->pitch(.4, -1, -1);
  else
    g_pSoundManager->pitch(1, -1, -1);
  if (m_pWeapon->m_nBullets <= 0 && !m_pWeapon->m_bMeleeWeapon)
    delete m_pWeapon;
  else
    g_cObjectManager.AddObjectByName(m_pWeapon->get_name(), (CGameObject*)m_pWeapon);
  m_pWeapon = nullptr;
}

CWeapon* CPlayerCharacter::get_weapon()
{
  return m_pWeapon;
}

void CPlayerCharacter::draw()
{
  if (m_pSprite == nullptr)return;
  if (m_bIsDead)return; //bail if already dead

  int t = m_nFrameInterval;
  if (m_bCycleSprite)
    t = (int)(t / (1.5f + fabs(m_vVelocity.Length())));

  if (m_pAnimation != nullptr) { //if there's an animation sequence
    //draw current frame
    m_pSprite->Draw(m_vPos, m_fOrientation, m_pAnimation[m_nCurrentFrame]);
    //advance to next frame
    if (m_vVelocity.Length() <= 0.001 && m_nCurrentFrame == 0)//if not moving dont advance past 0
	    m_nCurrentFrame = 0;
    else if (g_cTimer.elapsed(m_nLastFrameTime, t, true)) //if enough time passed and moving
      if (++m_nCurrentFrame >= m_nAnimationFrameCount && m_bCycleSprite) //increment and loop if necessary
        m_nCurrentFrame = 0;
  } //if
  else
	m_pSprite->Draw(m_vPos, m_fOrientation); //assume only one frame

  POINT curpos;
  HWND hwnd = GetActiveWindow(); //get active window *******NEEDS FIXING********* (maybe)
  RECT win_rec;
  GetCursorPos(&curpos);         //get screen cursor position
  ScreenToClient(hwnd, &curpos); //translate to client coordinates
  GetClientRect(hwnd, &win_rec); //get the current size of window
  //char buffer[100];
  //snprintf(buffer, sizeof(buffer), "%i %i\n", curpos.x, curpos.y);

  //OutputDebugString(buffer);
  
  //TODO change from center of the screen angles to player position angle
  
  if(g_bPlayerDead)
    m_fUpperAngle = 0.0f;
  else
    m_fUpperAngle = -atan2(curpos.y - win_rec.bottom/2, curpos.x - win_rec.right/2) + PI; //compute angle (negative)

  if(m_pWeapon)
    m_pWeapon->draw();
  
  m_pUpperSprite->Draw(Vector3(m_vPos.x, m_vPos.y, m_vPos.z - 2.1), m_fUpperAngle);
    
}

void CPlayerCharacter::move()
{

  const float SCALE = 32.0f; //to scale back motion

  const int time = g_cTimer.time(true); //current time
  int tdelta = time - m_nLastMoveTime; //time since last move
  if (tdelta < 0)
    tdelta = 0;
  const float tfactor = tdelta / SCALE; //scaled time factor

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

  //motion
  Vector3 moveDiff = m_vVelocity*tfactor; //position difference
  m_vPos += moveDiff; //move the player
  //check collision
  vector<Vector3> normals;
  if (g_cWallManager.CheckCollision(this, WALL_COLLISION, normals))
  {
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

  //move the gun with the player
  if (m_pWeapon)
  {
    float angle = m_fUpperAngle;
    Vector3 offset = Vector3(-cos(angle), -sin(angle), 0);
    offset.Normalize();
    m_pWeapon->set_pos(m_vPos + 40*offset, m_fUpperAngle);
  }

  //set the orrientation
  if (m_vVelocity.Length() != 0)
  { //range 90 > orientation > -90
	  m_fOrientation = atan2(m_vVelocity.y, m_vVelocity.x); //rotation around Z axis
	  m_fOrientation += PI;
  }

  //process weapon
  if(m_pWeapon && !g_bPlayerDead)
    m_pWeapon->process();

  m_nLastMoveTime = time;  //record time of move
}
