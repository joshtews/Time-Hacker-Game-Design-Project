/// \file objman.cpp
/// \brief Code for the object manager class CObjectManager.

#include "objman.h"
#include "debug.h"
#include "defines.h"
#include "timer.h"
#include "player.h"
#include "Weapon.h"
#include "SubWeapons.h"
#include "Collision.h"

extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern CTimer g_cTimer; 
extern CObjectManager g_cDeadManager;

/// Comparison for depth sorting game objects.
/// To compare two game objects, simply compare their Z coordinates.
/// \param p0 Pointer to game object 0.
/// \param p1 Pointer to game object 1.
/// \return true If object 0 is behind object 1.

bool ZCompare(const CGameObject* p0, const CGameObject* p1){
  return p0->m_vPos.z > p1->m_vPos.z;
} //ZCompare

CObjectManager::CObjectManager(){ 
  m_stlObjectList.clear();
  m_stlNameToObject.clear();
  m_stlNameToObjectType.clear();
  m_nLastGunFireTime = 0;
} //constructor

CObjectManager::~CObjectManager(){ 
	for (auto i = m_stlObjectList.begin(); i != m_stlObjectList.end(); i++)
	{
	  if(*i)
		delete *i;
	}
} //destructor

int CObjectManager::size()
{
  int total = 0;
  for (auto j = m_stlObjectList.begin(); j != m_stlObjectList.end(); j++)
    total++;
  return total;
}

/// Insert a map from an object name string to an object type enumeration.
/// \param name Name of an object type
/// \param t Enumerated object type corresponding to that name.

void CObjectManager::InsertObjectType(const char* name, ObjectType t){
  m_stlNameToObjectType.insert(pair<string, ObjectType>(name, t)); 
} //InsertObjectType

/// Get the ObjectType corresponding to a type name string. Returns NUM_OBJECT_TYPES
/// if the name is not in m_stlNameToObjectType.
/// \param name Name of an object type
/// \return Enumerated object type corresponding to that name.

ObjectType CObjectManager::GetObjectType(const char* name){
  unordered_map<string, ObjectType>::iterator i = 
    m_stlNameToObjectType.find(name);
  if(i == m_stlNameToObjectType.end()) //if name not in map
    return NUM_OBJECT_TYPES; //error return
  else return i->second; //return object type
} //GetObjectType

/// Create a new instance of a game object.
/// \param obj The type of the new object
/// \param name The name of object as found in name tag of XML settings file
/// \param s Location.
/// \param v Velocity.
/// \return Pointer to object created.

CGameObject* CObjectManager::createObject(ObjectType obj, const char* name, const Vector3& s, const Vector3& v, const float orientation){
  CGameObject* p;

  if (obj == PISTOL_OBJECT)
  {
	  p = new CPistol(s, v);
	  p->m_eCollisionType = PICKUP_COLLISION;
  }
  else if (obj == SHOTGUN_OBJECT)
  { 
    p = new CShotgun(s, v);
    p->m_eCollisionType = PICKUP_COLLISION;
  }
  else if (obj == LMG_OBJECT)
  {
    p = new CLmg(s, v);
    p->m_eCollisionType = PICKUP_COLLISION;
  }
  else if (obj == SLUGGER_OBJECT)
  {
    p = new CSlugger(s, v);
    p->m_eCollisionType = PICKUP_COLLISION;
  }
  else if (obj == KNIFE_OBJECT)
  {
    p = new CKnife(s, v);
    p->m_eCollisionType = PICKUP_COLLISION;
  }
  else if (obj == CHICAGO_SLUGGER_OBJECT)
  {
    p = new CChicagoSlugger(s, v);
    p->m_eCollisionType = PICKUP_COLLISION;
  }
  else p = new CGameObject(obj, name, s, v);    
  
  m_stlObjectList.push_front(p); //insert in object list

  if(m_stlNameToObject.find(name) == m_stlNameToObject.end()) //if name not in map
    m_stlNameToObject.insert(pair<string, CGameObject*>(name, p)); //put it there

  if (name == "flooring")
    p->m_eCollisionType = NO_COLLISION;

  p->m_fOrientation = orientation;
  return p;
} //createObject

void CObjectManager::AddObjectByName(const char* name, CGameObject* p) {
  m_stlObjectList.push_front(p);
  m_stlNameToObject.insert(pair<string, CGameObject*>(name, p));
}

/// Create a new instance of a game object with velocity zero.
/// \param objname The name of the new object's type
/// \param name The name of object as found in name tag of XML settings file
/// \param s Location.
/// \return Pointer to object created.

CGameObject* CObjectManager::createObject(const char* objname, const char* name, const Vector3& s){
  ObjectType obj = GetObjectType(objname);
  return createObject(obj, name, s, Vector3(0.0f));
} //createObject

/// Move all game objects, while making sure that they wrap around the world correctly.

void CObjectManager::move(){

  //move nonplayer objects
  for(auto i=m_stlObjectList.begin(); i!=m_stlObjectList.end(); i++){ //for each object
    CGameObject* curObject = *i; //current object
    curObject->move(); //move it
  } //for
  
  cull(); //cull old objects
  GarbageCollect(); //bring out yer dead!
} //move

/// Draw the objects from the object list and the player object. Care
/// must be taken to draw them from back to front.

void CObjectManager::draw(){
  m_stlObjectList.sort(ZCompare); //depth sort
  int temp = 0;
  for (auto i = m_stlObjectList.begin(); i != m_stlObjectList.end(); i++) { //for each object
    if ((*i)->m_nObjectType == NPC_OBJECT)
    {
      //DEBUGPRINTF("%d\n", ++temp);
      ++temp;
    } 
	  (*i)->draw();
	  /*if ((*i)->m_nObjectType == TESTWALL)
	  {
		  DEBUGPRINTF("0: %f %f \n", (*i)->getVertex(0).x, (*i)->getVertex(0).y);
		  DEBUGPRINTF("1: %f %f \n", (*i)->getVertex(1).x, (*i)->getVertex(1).y);
		  DEBUGPRINTF("2: %f %f \n", (*i)->getVertex(2).x, (*i)->getVertex(2).y);
		  DEBUGPRINTF("3: %f %f \n", (*i)->getVertex(3).x, (*i)->getVertex(3).y);
		  DEBUGPRINTF("4: %f %f \n", (*i)->getVertex(4).x, (*i)->getVertex(4).y);
	  }*/
  }
} //draw

/// Get a pointer to an object by name, nullptr if it doesn't exist.
/// \param name Name of object.
/// \return Pointer to object created with that name, if it exists.

CGameObject* CObjectManager::GetObjectByName(const char* name){ 
  unordered_map<string, CGameObject*>::iterator 
    current = m_stlNameToObject.find((string)name);
  if(current != m_stlNameToObject.end())
    return current->second;
  else return nullptr;
} //GetObjectByName

CGameObject* CObjectManager::RemoveObjectByName(const char* name) {
  unordered_map<string, CGameObject*>::iterator
    current = m_stlNameToObject.find((string)name);
  if (current != m_stlNameToObject.end())
    m_stlNameToObject.erase(name);
  else return nullptr;
} //GetObjectByName

CGameObject* CObjectManager::TakeObjectByName(CGameObject* p) {
  //CGameObject* p;
  //unordered_map<string, CGameObject*>::iterator
  //  current = m_stlNameToObject.find((string)name);
  //if (current != m_stlNameToObject.end())
  //{
  //  p = current->second;
  //  //current->second = nullptr;
  //  m_stlNameToObject.erase("gun");
  //  m_stlObjectList.remove(p);
  //  return p;
  //}
  //else return nullptr;
  
  for (auto j = m_stlObjectList.begin(); j != m_stlObjectList.end(); j++)
  {
    if (p == (*j))            
    {
      m_stlObjectList.remove((*j));
      return (*j);
    }
  }
  return nullptr;
} //GetObjectByName

/// Distance between objects.
/// \param pointer to first object 
/// \param pointer to second object
/// \return distance between the two objects

float CObjectManager::distance(CGameObject *g0, CGameObject *g1){ 
  if(g0 == nullptr || g1 == nullptr)return -1; //bail if bad pointer
  const float fWorldWidth = 2.0f * (float)g_nScreenWidth; //world width
  float x = (float)fabs(g0->m_vPos.x - g1->m_vPos.x); //x distance
  float y = (float)fabs(g0->m_vPos.y - g1->m_vPos.y); //y distance
  if(x > fWorldWidth) x -= (float)fWorldWidth; //compensate for wrap-around world
  return sqrtf(x*x + y*y);
} //distance

/// Fire a bullet from the plane's gun.
/// \param name Name of the object that is to fire the gun.

void CObjectManager::FireGun(char* name){   
  std::unordered_map<std::string, CGameObject*>::iterator planeIterator = 
    m_stlNameToObject.find((std::string)"plane");
  if(planeIterator == m_stlNameToObject.end())return; //this should of course never happen
  const CGameObject* planeObject = planeIterator->second;

  if(g_cTimer.elapsed(m_nLastGunFireTime, 200)){ //slow down firing rate
    const float fAngle = planeObject->m_fOrientation;
    const float fSine = sin(fAngle);
    const float fCosine = cos(fAngle);

    //enter the number of pixels from center of plane to gun
    const float fGunDx = -48.0f; 
    const float fGunDy = -17.0f;

    //initial bullet position
	const Vector3 s = planeObject->m_vPos;
      Vector3(fGunDx*fCosine - fGunDy*fSine, fGunDx*fSine - fGunDy*fCosine, 0); 

    //velocity of bullet
    const float BULLETSPEED = 10.0f;
    const Vector3 v = BULLETSPEED * Vector3(-fCosine, -fSine, 0) +
      planeObject->m_vVelocity;

    createObject(BULLET_OBJECT, "bullet", s, v); //create bullet
  } //if
} //FireGun

/// Cull old objects.
/// Run through the objects in the object list and compare their age to
/// their life span. Kill any that are too old. Immortal objects are
/// flagged with a negative life span, so ignore those.

void CObjectManager::cull(){ 
  for(auto i=m_stlObjectList.begin(); i!=m_stlObjectList.end(); i++){
    CGameObject* object = *i; //current object

    //died of old age
    if(object->m_nLifeTime > 0 && //if mortal and ...
    (g_cTimer.time() - object->m_nBirthTime > object->m_nLifeTime)) //...old...
      object->m_bIsDead = TRUE; //slay it

    //one shot animation 
    if(object->m_nFrameCount > 1 && !object->m_bCycleSprite && //if plays one time...
      object->m_nCurrentFrame >= object->m_nAnimationFrameCount){ //and played once already...
        object->m_bIsDead = TRUE; //slay it
        CreateNextIncarnation(object); //create next in the animation sequence
    } //if
  } //for
} //cull

/// Create the object next in the appropriate series (object, exploding
/// object, dead object). If there's no "next" object, do nothing.
/// \param object Pointer to the object to be replaced

void CObjectManager::CreateNextIncarnation(CGameObject* object){
//  if(object->m_nObjectType == CROW_OBJECT)
//    createObject(EXPLODINGCROW_OBJECT, "explodingcrow", object->m_vPos,
//      object->m_vVelocity); //create new one
//  else if(object->m_nObjectType == EXPLODINGCROW_OBJECT)
//    createObject(DEADCROW_OBJECT, "deadcrow", object->m_vPos,
//      object->m_vVelocity); //create new one
} //CreateNextIncarnation

/// Master collision detection function.
/// Compare every object against every other object for collision. Only
/// bullets can collide right now.

BOOL CObjectManager::CheckCollision(CObjectManager &manager, CollisionType collType, BOOL calcNormals){ 
  Vector3 positionTemp;
  vector<Vector3> normalTemp;
  bool Collided = false;
  for(auto i=m_stlObjectList.begin(); i!=m_stlObjectList.end(); i++)
    if(calcNormals)
      Collided = manager.CheckCollision(*i, collType, normalTemp, &positionTemp); //check every object for collision*/
    else
      Collided = manager.CheckCollision(*i,collType); //check every object for collision*/
	return Collided;
} //CollisionDetection

/// Given an object pointer, compare that object against every other 
/// object for collision. If a collision is detected, replace the object hit
/// with the next in series (if one exists), and kill the object doing the
/// hitting (bullets don't go through objects in this game).
/// \param p Pointer to the object to be compared against.

BOOL CObjectManager::CheckCollision(CGameObject* p, CollisionType collType)
{ 
  bool Collided = false;
  for (auto j = m_stlObjectList.begin(); j != m_stlObjectList.end(); j++)
  {
  /*  if ((*j)->m_eCollisionType == NPC_COLLISION)
      DEBUGPRINTF("CHECKING NPC");*/
    if (p && (*j) && //they exist
       p != (*j) &&                          //not the object for checking
      (*j)->m_eCollisionType >= collType && //has the correct collision type
      CollisionDetection(p, *j))            //check the collision
    {
      p->onCollision(*j);
      Collided = true;
    }
  }
	return Collided;
} //CollisionDetection

BOOL CObjectManager::CheckCollision(CGameObject* p, CollisionType collType, vector<Vector3> &normals, Vector3 *position)
{
  BOOL collide = false;
  for (auto j = m_stlObjectList.begin(); j != m_stlObjectList.end(); j++)
  {
    Vector3 normal = Vector3(0, 0, 0);
    if (p != (*j) &&                          //not the object for checking
      (*j)->m_eCollisionType >= collType && //has the correct collision type
      CollisionDetection(p, *j, &normal, position))            //check the collision
    {
      normals.push_back(normal);
      if(position != NULL)
        p->onCollision(*j, *position, normal);
      collide = true;
    }
  }
  return collide;
} //CollisionDetection

BOOL CObjectManager::CheckCollisionSimple(CGameObject * p, CollisionType collType)
{
  for (auto j = m_stlObjectList.begin(); j != m_stlObjectList.end(); j++)
  {
    if (p != (*j) &&                          //not the object for checking
      (*j)->m_eCollisionType >= collType && //has the correct collision type
      CollisionDetectionSimple(p, *j))            //check the collision
    {
      p->onCollision(*j);
      return true;
    }
  }
  return false;
}

BOOL CObjectManager::CheckCollisionSimple(CGameObject * p, CollisionType collType, vector<Vector3>& normals)
{
  BOOL collided = false;
  Vector3 normal = Vector3(0, 0, 0);

  for (auto j = m_stlObjectList.begin(); j != m_stlObjectList.end(); j++)
  {
    if (p != (*j) &&                          //not the object for checking
      (*j)->m_eCollisionType >= collType && //has the correct collision type
      CollisionDetectionSimple(p, *j, normal))            //check the collision
    {
      normals.push_back(normal);
      p->onCollision(*j);
      collided = true;
    }
  }
  return collided;
}

CGameObject* CObjectManager::FindCollisionSimple(CGameObject * p, CollisionType collType)
{
  for (auto j = m_stlObjectList.begin(); j != m_stlObjectList.end(); j++)
  {
    if (p != (*j) &&                          //not the object for checking
      (*j)->m_eCollisionType == collType && //has the correct collision type
      CollisionDetectionSimple(p, *j))            //check the collision
    {
      p->onCollision(*j);
      return (*j);
    }
  }
  return NULL;
}

void CObjectManager::clean()
{
  for (auto j = m_stlObjectList.begin(); j != m_stlObjectList.end(); j++)
  {
    m_stlObjectList.remove(*j);
  }
}

/// Collect garbage, that is, remove dead objects from the object list.

void CObjectManager::GarbageCollect(){
  auto i = m_stlObjectList.begin();
  while(i != m_stlObjectList.end()){
    CGameObject* p = *i; //save pointer to object temporarily
    if(p->m_bIsDead){
      i = m_stlObjectList.erase(i); //remove pointer from list
      //move to dead manager if an NPC, else delete
      if (p->m_nObjectType == NPC_OBJECT)
      {
        g_cDeadManager.AddObjectByName(p->m_sName, p);
        //fixes transparency issue...maybe
        p->m_vPos.z += -.00001 * g_cDeadManager.size();
        p->m_bIsDead = false;
        //DEBUGPRINTF("DEADGUY\n");
      }
      else
        delete p; //delete object
    } //if
    if(i != m_stlObjectList.end())
      ++i; //next object
  } //while
} //GarbageCollect