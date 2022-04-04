#pragma once
#include "Sprite.h"
#include "SpriteMan.h"
#include <vector>

struct SpriteComponent
{
  C3DSprite * m_pSpriteComponent = nullptr;
  Vector3 m_vPos;
};

struct TextComponent
{
  char * m_sText;
  Vector3 m_vPos;
  RECT m_rBox;
};

class CMenu
{
  public:
  CMenu();
  ~CMenu();

  void insertSpriteComponent(ObjectType objtype, Vector3 s);
  void insertTextComponent(char * text, Vector3 s, RECT r);
  vector<struct SpriteComponent> getSpriteComponents();
  vector<struct TextComponent> getTextComponents();
  char * checkCollision();
  
  vector<struct SpriteComponent> m_pSpriteComponents;
  vector<struct TextComponent> m_pTextComponents;
  
  private:
  int m_nNumOfSpriteComponents;
  int m_nNumOfTextComponents;
};