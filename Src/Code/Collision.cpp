
#include "Collision.h"

/// Given 2 object pointers, see whether the objects collide. 
/// If a collision is detected, replace the object hit
/// with the next in series (if one exists), and kill the object doing the
/// hitting (bullets don't go through objects in this game).
/// \param i iterator of the object to compare against


//collision detection with normal and position calculations
BOOL CollisionDetection(CGameObject* p0, CGameObject* p1, Vector3 *normal, Vector3 *position)
{
  /*
  p0 = first game object
  p1 = second game object
  normal = the normal of the surface on p1
  position = point of impact
  */

  //if the object is far away it is definitly not colliding
  if (((p0->m_vPos - p1->m_vPos).Length() > p1->m_nWidth * 2 &&
    (p0->m_vPos - p1->m_vPos).Length() > p1->m_nHeight * 2) &&
    ((p0->m_vPos - p1->m_vPos).Length() > p0->m_nWidth * 2 &&
    (p0->m_vPos - p1->m_vPos).Length() > p0->m_nHeight * 2))
    return false;

  //make vectors
  Vector2 normals[4];
  vector<Vector2> vecs_box1;
  vector<Vector2> vecs_box2;

  //set vertex positions
  for (int i = 1; i < 5; i++)
  {
    vecs_box1.push_back(p0->getVertex(i));
    vecs_box2.push_back(p1->getVertex(i));
  }

  //because everything is a rectangle we can get normals by using
  //the complementary side to the one we want
  normals[0] = vecs_box1[0] - vecs_box1[3];
  normals[1] = vecs_box1[2] - vecs_box1[3];
  normals[2] = vecs_box2[0] - vecs_box2[3];
  normals[3] = vecs_box2[2] - vecs_box2[3];

  //loop through each normal
  for (int i = 0; i < 4; i++)
  {
    //setting min-max
    float minProjBox1 = vecs_box1[0].Dot(normals[i]);
    int minVertexBox1 = 1;
    float maxProjBox1 = vecs_box1[0].Dot(normals[i]);
    int maxVertexBox1 = 1;

    int size = (int)vecs_box1.size();
    for (int j = 1; j < size; j++)
    {
      float currProj1 = vecs_box1[j].Dot(normals[i]);

      if (minProjBox1 > currProj1)
      {
        minProjBox1 = currProj1;
        minVertexBox1 = j;
      }

      if (currProj1 > maxProjBox1)
      {
        maxProjBox1 = currProj1;
        maxVertexBox1 = j;
      }
    }

    //setting min-max
    float minProjBox2 = vecs_box2[0].Dot(normals[i]);
    int minVertexBox2 = 1;
    float maxProjBox2 = vecs_box2[0].Dot(normals[i]);
    int maxVertexBox2 = 1;

    size = (int)vecs_box2.size();
    for (int j = 1; j < size; j++)
    {
      float currProj2 = vecs_box2[j].Dot(normals[i]);

      if (minProjBox2 > currProj2)
      {
        minProjBox2 = currProj2;
        minVertexBox2 = j;
      }

      if (currProj2 > maxProjBox2)
      {
        maxProjBox2 = currProj2;
        maxVertexBox2 = j;
      }
    }

    //check overlap
    if (maxProjBox2 < minProjBox1 || maxProjBox1 < minProjBox2)
    {
      return false;
    }
  }
  //end now if no normal is requested
  if (normal == NULL)
    return true;

  /*
  [0] = 4-----1
  [1] = 1-----2
  [2] = 2-----3
  [3] = 3-----4
  */

  //if the player is between the 2 vectors of the side we are checking
  int side = -1;

  //check 4----1 side first
  Vector3 d = vecs_box2[3] - p1->m_vPos;
  Vector3 e = vecs_box2[0] - p1->m_vPos;
  Vector3 f = p0->m_vPos - p1->m_vPos;
  if (((d.x*e.y) - (e.x*d.y)) * ((d.x*f.y) - (f.x*d.y)) >= 0 &&
    ((f.x*e.y) - (e.x*f.y)) * ((f.x*d.y) - (d.x*f.y)) >= 0)
  {
    side = 0;
  }

  //loop through the rest of the sides
  for (int i = 1; i < 4 && side < 0; i++)
  {
    d = vecs_box2[i-1] - p1->m_vPos;
    e = vecs_box2[i] - p1->m_vPos;
    f = p0->m_vPos - p1->m_vPos;
    //if the player is between the 2 vectors of the side we are checking
    if (((d.x*e.y) - (e.x*d.y)) * ((d.x*f.y) - (f.x*d.y)) >= 0 &&
      ((f.x*e.y) - (e.x*f.y)) * ((f.x*d.y) - (d.x*f.y)) >= 0)
    {
      side = i;
    }
  }

  //idk why but this fixes it
  if (side < 0)
    return true;
  side++;
  if (side > 3)
    side = 0;

  //calculate normal and position
  float a;
  float b;
  float c;
  Vector2 slope;

  //get initial values
  if (side == 0)
  { //4----0 side
    slope = vecs_box2[0] - vecs_box2[3];
    b = slope.x;
    a = -slope.y;
    c = -slope.x*(vecs_box2[0].y - (slope.y / slope.x)*vecs_box2[0].x);
  }
  else
  { //rest of the sides
    //get slope as Vector2(deltaX, deltaY)
    slope = vecs_box2[side] - vecs_box2[side-1];
    b = slope.x;
    a = -slope.y;
    c = -slope.x*(vecs_box2[side].y - (slope.y / slope.x)*vecs_box2[side].x);
  }

  //set position of closest side
  if (position != NULL)
  {
    position->x = (b * (b * p0->m_vPos.x - a * p0->m_vPos.y) - a * c) /
                  (a * a + b * b);
    position->y = (a * (-b * p0->m_vPos.x + a * p0->m_vPos.y) - b * c) /
                  (a * a + b * b);
  }

  //set normal of the closest side
  normal->x = a;
  normal->y = b;
  normal->Normalize();
  

  return true;
} //CollisionDetection

//collision detection without normal and position calculation
BOOL CollisionDetection(CGameObject* p0, CGameObject* p1)
{
  /*
  p0 = first game object
  p1 = second game object
  normal = the normal of the surface on p1
  position = point of impact
  */

  //if the object is far away it is definitly not colliding
  if (((p0->m_vPos - p1->m_vPos).Length() > p1->m_nWidth * 2 &&
    (p0->m_vPos - p1->m_vPos).Length() > p1->m_nHeight * 2) && 
    ((p0->m_vPos - p1->m_vPos).Length() > p0->m_nWidth * 2 &&
    (p0->m_vPos - p1->m_vPos).Length() > p0->m_nHeight * 2) && 
      (p0->m_vPos - p1->m_vPos).Length() > 50)
    return false;

  //make vectors
  Vector2 normals[4];
  vector<Vector2> vecs_box1;
  vector<Vector2> vecs_box2;

  //set vertex positions
  for (int i = 1; i < 5; i++)
  {
    vecs_box1.push_back(p0->getVertex(i));
    vecs_box2.push_back(p1->getVertex(i));
  }

  //because everything is a rectangle we can get normals by using
  //the complementary side to the one we want
  normals[0] = vecs_box1[0] - vecs_box1[3];
  normals[1] = vecs_box1[2] - vecs_box1[3];
  normals[2] = vecs_box2[0] - vecs_box2[3];
  normals[3] = vecs_box2[2] - vecs_box2[3];

  //loop through each normal
  for (int i = 0; i < 4; i++)
  {
    //setting min-max
    float minProjBox1 = vecs_box1[0].Dot(normals[i]);
    int minVertexBox1 = 1;
    float maxProjBox1 = vecs_box1[0].Dot(normals[i]);
    int maxVertexBox1 = 1;

    int size = (int)vecs_box1.size();
    for (int j = 1; j < size; j++)
    {
      float currProj1 = vecs_box1[j].Dot(normals[i]);

      if (minProjBox1 > currProj1)
      {
        minProjBox1 = currProj1;
        minVertexBox1 = j;
      }

      if (currProj1 > maxProjBox1)
      {
        maxProjBox1 = currProj1;
        maxVertexBox1 = j;
      }
    }

    //setting min-max
    float minProjBox2 = vecs_box2[0].Dot(normals[i]);
    int minVertexBox2 = 1;
    float maxProjBox2 = vecs_box2[0].Dot(normals[i]);
    int maxVertexBox2 = 1;

    size = (int)vecs_box2.size();
    for (int j = 1; j < size; j++)
    {
      float currProj2 = vecs_box2[j].Dot(normals[i]);

      if (minProjBox2 > currProj2)
      {
        minProjBox2 = currProj2;
        minVertexBox2 = j;
      }

      if (currProj2 > maxProjBox2)
      {
        maxProjBox2 = currProj2;
        maxVertexBox2 = j;
      }
    }

    //check overlap
    if (maxProjBox2 < minProjBox1 || maxProjBox1 < minProjBox2)
    {
      return false;
    }
  }
  return true;
} //CollisionDetection

//collision detection using radius based collision
BOOL CollisionDetectionSimple(CGameObject* p0, CGameObject* p1)
{      //object's distance            <=            p1's largest side size
  if ((p0->m_vPos - p1->m_vPos).Length() <= max(p1->m_nHeight, p1->m_nWidth))
    return true;
  return false;
}

BOOL CollisionDetectionSimple(CGameObject* p0, CGameObject* p1, Vector3& normal)
{      //object's distance            <=            p1's largest side size
  if ((p0->m_vPos - p1->m_vPos).Length() <= (p1->m_nHeight + p1->m_nWidth)/2 )
  {
    (p0->m_vPos - p1->m_vPos).Normalize(normal);
    return true;
  }
  return false;
}