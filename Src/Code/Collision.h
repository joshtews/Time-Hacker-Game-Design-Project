#pragma once
#include <list>
#include <string>
#include <unordered_map>

#include "object.h"
#include "objman.h"
#include "debug.h"
#include "defines.h"
#include "timer.h"
#include "player.h"
#include "Weapon.h"
#include "SubWeapons.h"

///< Complex collisions of 2 objects. gives 
BOOL CollisionDetection(CGameObject* p0, CGameObject* p1);
BOOL CollisionDetection(CGameObject* p0, CGameObject* p1, Vector3* normalOfP1, Vector3* pos);
BOOL CollisionDetectionSimple(CGameObject* p0, CGameObject* p1);
BOOL CollisionDetectionSimple(CGameObject* p0, CGameObject* p1, Vector3& normal);