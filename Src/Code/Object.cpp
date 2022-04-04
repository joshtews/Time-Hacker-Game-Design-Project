/// \file object.cpp
/// \brief Code for the game object class CGameObject.

#include "object.h"

#include "defines.h" 
#include "timer.h" 
#include "debug.h"
#include "spriteman.h"
#include "Sound.h"


extern CTimer g_cTimer;
extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern XMLElement* g_xmlSettings;
extern CSpriteManager g_cSpriteManager;
extern CSoundManager* g_pSoundManager;

/// Initialize a game object. Gets object-dependent settings from g_xmlSettings
/// from the "object" tag that has the same "name" attribute as parameter name.
/// Assumes that the sprite manager has loaded the sprites already.
/// \param object Object type
/// \param name Object name in XML settings file object tag
/// \param s Initial location of object
/// \param v Initial velocity

CGameObject::CGameObject(ObjectType object, const char* name, const Vector3& s, const Vector3& v){ 
  //defaults
  m_nCurrentFrame = 0; 
  m_nLastFrameTime = g_cTimer.time();
  m_nFrameInterval = 30; 
  
  m_nLifeTime = -1; //negative means immortal
  m_bVulnerable = FALSE; 
  m_bIntelligent = FALSE;
  m_bIsDead = FALSE;

  m_eCollisionType = WALL_COLLISION;

  m_nMinXSpeed = -20; 
  m_nMaxXSpeed = 20;
  m_nMinYSpeed = -20; 
  m_nMaxYSpeed = 20;

  m_pAnimation = nullptr;
  m_nAnimationFrameCount = 0;
  m_bCycleSprite = TRUE;

  //common values
  m_nObjectType = object; //type of object
  m_sName = name;

  m_pSprite = g_cSpriteManager.GetSprite(object); //sprite pointer
  if(m_pSprite && m_eCollisionType != NPC_COLLISION){
    m_nFrameCount = m_pSprite->m_nFrameCount; //get frame count
    m_nHeight = m_pSprite->m_nHeight; //get object height from sprite
    m_nWidth = m_pSprite->m_nWidth; //get object width from sprite
  } //if

  if (m_pSprite && object == SLUGGER_OBJECT)
  {
    m_nWidth  = m_nWidth;
    m_nHeight = m_nHeight;
  }
  if (m_pSprite && object == CHICAGO_SLUGGER_OBJECT)
  {
    m_nWidth = m_nWidth;
    m_nHeight = m_nHeight;
  }
  if (m_pSprite && object == KNIFE_OBJECT)
  {
    m_nWidth = 25;
    m_nHeight = 25;
  }

  m_nLastMoveTime = g_cTimer.time(); //time
  m_nBirthTime = g_cTimer.time(); //time of creation
  m_vPos = s; //location
  m_vVelocity = v;  //velocity
  
  //object-dependent settings loaded from XML
  LoadSettings(name);

  m_nSoundInstance = -1;

} //constructor

CGameObject::~CGameObject(){
  delete[] m_pAnimation;
} //destructor

/// Draw the current sprite frame at the current position, then
/// compute which frame is to be drawn next time.

void CGameObject::draw(){
  if(m_pSprite == nullptr)return;
  if(m_bIsDead)return; //bail if already dead

  int t = m_nFrameInterval;
  if(m_bCycleSprite)
    t = (int)(t/(1.5f + fabs(m_vVelocity.Length())));

  if(m_pAnimation != nullptr){ //if there's an animation sequence
    //draw current frame
    m_pSprite->Draw(m_vPos, m_fOrientation, m_pAnimation[m_nCurrentFrame]);
    //advance to next frame
    if(g_cTimer.elapsed(m_nLastFrameTime, t)) //if enough time passed
      //increment and loop if necessary
      if(++m_nCurrentFrame >= m_nAnimationFrameCount && m_bCycleSprite) 
        m_nCurrentFrame = 0;
  } //if
  else 
    m_pSprite->Draw(m_vPos, m_fOrientation); //assume only one frame
} //draw

/// Load settings for object from g_xmlSettings.
/// \param name name of object as found in name tag of XML settings file

void CGameObject::LoadSettings(const char* name){
  if(g_xmlSettings){ //got "settings" tag
    //get "objects" tag
    XMLElement* objSettings =
      g_xmlSettings->FirstChildElement("objects"); //objects tag

    if(objSettings){ //got "objects" tag
      //set obj to the first "object" tag with the correct name
      XMLElement* obj = objSettings->FirstChildElement("object");
      while(obj && strcmp(name, obj->Attribute("name"))){
        obj = obj->NextSiblingElement("object");
      } //while

      if(obj){ //got "object" tag with right name
        //get object information from tag
        m_nMinXSpeed = obj->IntAttribute("minxspeed");
        m_nMaxXSpeed = obj->IntAttribute("maxxspeed");
        m_nMinYSpeed = obj->IntAttribute("minyspeed");
        m_nMaxYSpeed = obj->IntAttribute("maxyspeed");
        m_nFrameInterval = obj->IntAttribute("frameinterval");
        m_bVulnerable = obj->BoolAttribute("vulnerable");
        m_bCycleSprite = obj->BoolAttribute("cycle");
        m_nLifeTime = obj->IntAttribute("lifetime");   

        //parse animation sequence
        if(obj->Attribute("animation")){ //sequence present

          //get sequence length
          size_t length = strlen(obj->Attribute("animation"));
          m_nAnimationFrameCount = 1; //one more than number of commas
          for(size_t i = 0; i<length; i++) //for each character
          if(obj->Attribute("animation")[i] == ',')
            m_nAnimationFrameCount++; //count commas

          m_pAnimation = new int[m_nAnimationFrameCount]; //memory for animation sequence

          size_t i = 0; //character index
          int count = 0; //number of frame numbers input
          int num; //frame number
          char c = obj->Attribute("animation")[i]; //character in sequence string
          while(i < length){
            //get next frame number
            num = 0;
            while(i<length && c >= '0' && c <= '9'){
              num = num * 10 + c - '0';
              c = obj->Attribute("animation")[++i];
            }
            //process frame number
            c = obj->Attribute("animation")[++i]; //skip over comma
            m_pAnimation[count++] = num; //record frame number
          } //while
        } //if
      } //if
    } //if
  } //if
} //LoadSettings 
 
/// The distance that an object moves depends on its speed, 
/// and the amount of time since it last moved.

void CGameObject::move(){ //move object 
  const float SCALE = 32.0f; //to scale back motion

  const int time = g_cTimer.time(); //current time
  int tdelta = time - m_nLastMoveTime; //time since last move
  if (tdelta < 0)
    tdelta = 0;
  const float tfactor = tdelta/SCALE; //scaled time factor

  m_vPos += m_vVelocity*tfactor; //motion

  //set the orrientation
  if (m_vVelocity.Length() != 0)
  { //range 90 > orientation > -90
    
	  m_fOrientation = atan2(m_vVelocity.y, m_vVelocity.x); //rotation around Z axis
	  m_fOrientation += PI;
  }
  m_nLastMoveTime = time;  //record time of move
} //move

void CGameObject::onCollision(CGameObject *collidedObject)
{
}

void CGameObject::onCollision(CGameObject* other, Vector3 pos, Vector3 normal)
{
}

Vector2 CGameObject::getVertex(int i)
{
	/*points are as follows :
	4---------1
	|         |
	|    0    |
	|         |
	3---------2
	*/
	//truncate to 0,1,2,3,4
	min(i, 4);
	max(i, 0);
	//return the matching vertex
	switch (i)
	{
	  case 0: 
		  return Vector2(m_vPos.x, m_vPos.y);
		  break;
	  case 1: 
		  return Vector2( ((m_nWidth/2)*cos(m_fOrientation) - (m_nHeight/2)*sin(m_fOrientation)) + m_vPos.x,
		                  ((m_nWidth/2)*sin(m_fOrientation) + (m_nHeight/2)*cos(m_fOrientation)) + m_vPos.y);
		  break;
	  case 2:
		  return Vector2( ((m_nWidth / 2)*cos(m_fOrientation) - (-m_nHeight / 2)*sin(m_fOrientation)) + m_vPos.x,
			              ((m_nWidth / 2)*sin(m_fOrientation) + (-m_nHeight / 2)*cos(m_fOrientation)) + m_vPos.y);
		  break;
	  case 3:
		  return Vector2(((-m_nWidth / 2)*cos(m_fOrientation) - (-m_nHeight / 2)*sin(m_fOrientation)) + m_vPos.x,
			             ((-m_nWidth / 2)*sin(m_fOrientation) + (-m_nHeight / 2)*cos(m_fOrientation)) + m_vPos.y);
		  break;
	  case 4:
		  return Vector2(((-m_nWidth / 2)*cos(m_fOrientation) - (m_nHeight / 2)*sin(m_fOrientation)) + m_vPos.x,
			             ((-m_nWidth / 2)*sin(m_fOrientation) + (m_nHeight / 2)*cos(m_fOrientation)) + m_vPos.y);
		  break;
	}
	return Vector2();
}
