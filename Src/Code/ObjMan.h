/// \file objman.h
/// \brief Interface for the object manager class CObjectManager.

#pragma once

#include <list>
#include <string>
#include <unordered_map>

#include "object.h"

/// \brief The object manager. 
///
/// The object manager is responsible for the care and feeding of
/// game objects. Objects can be named on creation so that they
/// can be accessed later - this is needed in particular for the player
/// object or objects.

class CObjectManager{
  private:
    list<CGameObject*> m_stlObjectList; ///< List of game objects.
    unordered_map<string, CGameObject*> m_stlNameToObject; ///< Map names to objects.
    unordered_map<string, ObjectType> m_stlNameToObjectType; ///< Map names to object types.
    
    int m_nLastGunFireTime; ///< Time gun was last fired.

    //creation functions
    CGameObject* createObject(const char* obj, const char* name, const Vector3& s); ///< Create new object by name.
    
    //distance functions
    float distance(CGameObject *g0, CGameObject *g1); ///< Distance between objects.

    //managing dead objects
    void cull(); ///< Cull dead objects
    void CreateNextIncarnation(CGameObject* object); ///< Replace object by next in series.

  public:
    CObjectManager(); ///< Constructor.
    ~CObjectManager(); ///< Destructor.

    int size(); //gets the number of items in the manager

    CGameObject* createObject(ObjectType obj, const char* name, const Vector3& s, const Vector3& v, const float orientation = 0); ///< Create new object.

    void move(); ///< Move all objects.
    void draw(); ///< Draw all objects.

    void GarbageCollect(); ///< Collect dead objects from object list.
    CGameObject* GetObjectByName(const char* name); ///< Get pointer to object by name.
    void InsertObjectType(const char* objname, ObjectType t); ///< Map name string to object type enumeration.
    ObjectType GetObjectType(const char* name); ///< Get object type corresponding to name string.
    void AddObjectByName(const char* name, CGameObject* p); ///< Add object back to manager after taken
    CGameObject* RemoveObjectByName(const char* name); ///< Delete object
    CGameObject* TakeObjectByName(CGameObject* p); ///< Take the object from manager and give to class
    void clean();
    //collision against the whole manager
    //collision detection
    BOOL CheckCollision(CObjectManager &manager, CollisionType collType = NPC_COLLISION, BOOL calcNormals = false); ///< Process all collisions against another object manager
    BOOL CheckCollision(CGameObject* p0, CollisionType collType = CollisionType::WALL_COLLISION); ///< Process collisions of all with one object.
    BOOL CheckCollision(CGameObject* p0, CollisionType collType, vector<Vector3> &normal, Vector3 *position = NULL); ///< Process collisions of all with one object.
    BOOL CheckCollisionSimple(CGameObject* p0, CollisionType collType = CollisionType::PICKUP_COLLISION); ///< Process simple collisions of all with one object.
    BOOL CheckCollisionSimple(CGameObject* p0, CollisionType collType, vector<Vector3> &normal); ///< Process simple collisions of all with one object.
    CGameObject* FindCollisionSimple(CGameObject* p0, CollisionType collType = CollisionType::WALL_COLLISION); ///< Process simple collisions of all with one object.
    void FireGun(char* name); ///< Fire a gun from named object.
}; //CObjectManager