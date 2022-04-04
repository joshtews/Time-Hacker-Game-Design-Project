#include "Menu.h"
#include "Sprite.h"
#include "SpriteMan.h"
#include "GameRenderer.h"
#include "debug.h"
#include <vector>

extern CGameRenderer GameRenderer;
extern CSpriteManager g_cSpriteManager; 
extern HWND g_HwndApp;
extern int g_nScreenHeight;

CMenu::CMenu()
{
  m_nNumOfSpriteComponents = 0;
  m_nNumOfTextComponents = 0;
}

CMenu::~CMenu()
{
}

void CMenu::insertSpriteComponent(ObjectType objtype, Vector3 s)
{
  //m_pSpriteComponents[m_nNumOfSpriteComponents].m_pSpriteComponent = g_cSpriteManager.GetSprite(objtype);
  //m_pSpriteComponents[m_nNumOfSpriteComponents].m_vPos = s;
  struct SpriteComponent temp;
  temp.m_pSpriteComponent = g_cSpriteManager.GetSprite(objtype);
  temp.m_vPos = s;
  m_pSpriteComponents.push_back(temp);
  m_nNumOfSpriteComponents++;
}

void CMenu::insertTextComponent(char * text, Vector3 s, RECT r)
{
  //m_pTextComponents[m_nNumOfTextComponents].m_sText = text;
  //m_pTextComponents[m_nNumOfTextComponents].m_vPos = s;
  struct TextComponent temp;
  temp.m_sText = text;
  temp.m_vPos = s;
  temp.m_rBox = r;
  m_pTextComponents.push_back(temp);
  m_nNumOfTextComponents++;
}

vector<struct SpriteComponent> CMenu::getSpriteComponents()
{
  return m_pSpriteComponents;
}

vector<struct TextComponent> CMenu::getTextComponents()
{
  return m_pTextComponents;
}

char * CMenu::checkCollision()
{
  POINT curpos;
  GetCursorPos(&curpos);         //get screen cursor position
  ScreenToClient(g_HwndApp, &curpos); //translate to client coordinates
  for(int i = 0; i < m_pTextComponents.size(); i++)
  {
    if(curpos.y < m_pTextComponents[i].m_rBox.bottom && curpos.y > m_pTextComponents[i].m_rBox.top
          && curpos.x > m_pTextComponents[i].m_rBox.left && curpos.x < m_pTextComponents[i].m_rBox.right)
    {
      return m_pTextComponents[i].m_sText;
    }
  }
  
  return "";
}