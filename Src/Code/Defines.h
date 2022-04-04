/// \file defines.h
/// \brief Essential defines.

#pragma once

#include <d3d11_2.h>
#include <dxgi1_3.h>
#include <DirectXMath.h>

#include "SimpleMath.h"
#include "tinyxml2.h"

#define PI 3.14159265358979323846

using namespace DirectX;
using namespace SimpleMath;
using namespace tinyxml2;
using namespace std;

/// Safe release of a pointer to a Windows COM object. If
/// the pointer is not null, then release what it is pointing
/// to and set it to null.

//Beware the extra parentheses in this #define. They are there
//for a reason. 

#define SAFE_RELEASE(p) if(p){(p)->Release(); (p) = nullptr;}

/// Safe delete of a pointer. Note that it is OK to delete a null
/// pointer. Note the brackets and braces.

#define SAFE_DELETE(p) {delete (p); (p) = nullptr;}

/// \brief Billboard vertex structure. 
///
/// Custom vertex format for representing a vanilla billboard object. Consists 
/// of position, and texture coordinates.

struct BILLBOARDVERTEX{ 
  Vector3 p; ///< Position.
  float tu; ///< Texture U coordinate.
  float tv; ///< Texture V coordinate.
}; //BILLBOARDVERTEX

/// Constant buffer for use by shaders.

struct ConstantBuffer{
  XMFLOAT4X4 wvp; ///< World View Projection matrix
  float u0, u1, v0, v1;
}; //ConstantBuffer

/// Game object types.
/// Types of game object that can appear in the game. Note: NUM_OBJECT_TYPES 
/// must be last.

enum ObjectType{
  PLAYER_UPPER, PLAYER_OBJECT, WEAPON_OBJECT,
  BULLET_OBJECT, PISTOL_OBJECT, PISTOL_EQUIPPED, WALL_OBJECT, 
  SHOTGUN_OBJECT, SHOTGUN_EQUIPPED,
  FLOORING, HUD_OBJECT, LMG_OBJECT, LMG_EQUIPPED, NPC_OBJECT, ENEMYTEST_UPPER,
  ENEMYTEST_OBJECT, SLUGGER_OBJECT, SLUGGER_EQUIPPED, KNIFE_OBJECT, KNIFE_EQUIPPED,
  ENEMYTEST_DEAD, TITLECARD, LIVES_SPRITE, CHICAGO_SLUGGER_OBJECT,
  CHICAGO_SLUGGER_EQUIPPED, NUM_OBJECT_TYPES //MUST be the last one
}; //ObjectType

enum CollisionType {
  NO_COLLISION, PICKUP_COLLISION, PROJECTILE_COLLISION, NPC_COLLISION, WALL_COLLISION, ALL_COLLISION
};

enum NPCState {
  ROAMING, ALERT, IDLE, DEAD
};