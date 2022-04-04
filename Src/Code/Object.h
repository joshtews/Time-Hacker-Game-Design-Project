/// \file object.h
/// \brief Interface for the game object class CGameObject.

#pragma once

#include "sprite.h"
#include "defines.h"
#include <iostream>

/// \brief The game object. 
///
/// Game objects are responsible for remembering information about 
/// themselves - including their image, location, and speed - and 
/// for moving and drawing themselves.

class CGameObject{ //class for a game object
  friend class CIntelligentObject;
  friend class CObjectManager;
  friend class CGameRenderer;
  friend class CSoundManager;
  friend class CProjectile;
  friend class CWeapon;
  //friend BOOL KeyboardHandler(WPARAM keystroke); //for keyboard control of objects
  friend bool ZCompare(const CGameObject* p0, const CGameObject* p1); //for depth sorting

  protected:
    POINT mouse_pos;
    int m_nLastMoveTime; ///< Last time moved.
    int m_nMinXSpeed; ///< Min horizontal speed.
    int m_nMaxXSpeed; ///< Max horizontal speed.
    int m_nMinYSpeed; ///< Min vertical speed.
    int m_nMaxYSpeed; ///< Max vertical speed.
	  float m_fAccelerationRate;  ///< Acceleration/turn rate

    

    C3DSprite* m_pSprite; ///< Pointer to sprite.
    
    int m_nBirthTime; ///< Time of creation.
    int m_nLifeTime; ///< Time that object lives.
    BOOL m_bVulnerable; ///< Vulnerable to bullets.
    BOOL m_bIntelligent; ///< TRUE for an intelligent object.
    BOOL m_bIsDead; ///< TRUE if the object is dead.
    int m_nSoundInstance;

    void LoadSettings(const char* name); //< Load object-dependent settings from XML element.

  public:
    CGameObject(ObjectType object, const char* name, const Vector3& s, const Vector3& v); ///< Constructor.
    ~CGameObject(); //< Destructor.

    virtual void draw(); ///< Draw at current location.
    virtual void move(); ///< Change location depending on time and speed  
	  virtual void onCollision(CGameObject*);
    virtual void onCollision(CGameObject* other, Vector3 pos, Vector3 normal);
  	Vector2 getVertex(int i); /// gets a Vertex of the rectangle
	/*points are as follows :
	   4---------1
	   |         |
	   |    0    |
	   |         |
	   3---------2
	*/

    int m_nFrameInterval;
    int * m_pAnimation;
    int m_nCurrentFrame;
    int m_nLastFrameTime;
    int m_nAnimationFrameCount;
    int m_nFrameCount;
    bool m_bCycleSprite;

    ObjectType m_nObjectType; ///< Object type.

    float m_fOrientation; ///< Orientation, angle to rotate about the Z axis.
    Vector3 m_vPos; ///< Current location.
    Vector3 m_vVelocity; ///< Current velocity.
    CollisionType m_eCollisionType; //Object's Collision Type
    int m_nWidth; ///< Width of object.
    int m_nHeight; ///< Height of object.


    const char* m_sName; ///< object name;
}; //CGameObject

