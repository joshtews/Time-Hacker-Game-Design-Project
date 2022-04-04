/// \file gamerenderer.cpp
/// \brief Direct3D rendering tasks for the game.
/// DirectX stuff that won't change much is hidden away in this file
/// so you won't have to keep looking at it.

#include <algorithm>

#include "gamerenderer.h"
#include "defines.h" 
#include "abort.h"
#include "imagefilenamelist.h"
#include "debug.h"
#include "sprite.h"
#include "object.h"
#include "objman.h"
#include "spriteman.h"
#include "SpriteSheet.h"
#include "Weapon.h"
#include "player.h"
#include "Menu.h"
#include "EnemyTestCharacter.h"
#include "Random.h"
#include "Timer.h"
#include "Sound.h"

extern int g_nScreenWidth;
extern int g_nScreenHeight;
extern BOOL g_bWireFrame;
extern HWND g_HwndApp;
extern CImageFileNameList g_cImageFileName;
extern CObjectManager g_cObjectManager;
extern CObjectManager g_cWallManager;
extern CObjectManager g_cProjectileManager;
extern CObjectManager g_cNPCManager;
extern CSpriteManager g_cSpriteManager;
extern CObjectManager g_cDeadManager;
extern CPlayerCharacter* g_cPlayer;
extern CObjectManager g_cDeadManager;
extern bool g_bPlayerDead;
extern CMenu g_cMainMenu;
extern CTimer g_cTimer;
extern bool g_bAtMainMenu;
extern int g_nNPCsKilled;
extern CRandom g_cRandom;
extern bool g_bPlayerHit;
extern CSoundManager * g_pSoundManager;
extern float g_fPitch;
extern ObjectType g_nWeapons[3];
extern char * g_nWeaponNames[3];
extern Vector3 g_vSpawnLocations[8];


CGameRenderer::CGameRenderer(): m_bCameraDefaultMode(TRUE){
  m_cScreenText = nullptr;
	m_fCameraZDistance = -1000;
  m_nRoundTextLifetime = 5000;
  m_nLastRoundTexttime = 0;
  for (int i = 0; i < 10; i++)
    m_bRoundHit[i] = true;
} //constructor

CGameRenderer::~CGameRenderer()
{
  delete m_cScreenText;
}
 

/// Initialize the vertex and constant buffers for the background, that is, the
/// ground and the sky.

void CGameRenderer::InitBackground(){
  HRESULT hr;

  //load vertex buffer
  float w = 2.0f*g_nScreenWidth;
  float h = 2.0f*g_nScreenHeight;
  
  //vertex information, first triangle in clockwise order
  BILLBOARDVERTEX pVertexBufferData[6]; 
  pVertexBufferData[0].p = Vector3(w, 0, 0);
  pVertexBufferData[0].tu = 1.0f; pVertexBufferData[0].tv = 0.0f;

  pVertexBufferData[1].p = Vector3(0, 0, 0);
  pVertexBufferData[1].tu = 0.0f; pVertexBufferData[1].tv = 0.0f;

  pVertexBufferData[2].p = Vector3(w, 0, 1500);
  pVertexBufferData[2].tu = 1.0f; pVertexBufferData[2].tv = 1.0f;

  pVertexBufferData[3].p = Vector3(0, 0, 1500);
  pVertexBufferData[3].tu = 0.0f; pVertexBufferData[3].tv = 1.0f;

  pVertexBufferData[4].p = Vector3(w, h, 1500);
  pVertexBufferData[4].tu = 1.0f; pVertexBufferData[4].tv = 0.0f;

  pVertexBufferData[5].p = Vector3(0, h, 1500);
  pVertexBufferData[5].tu = 0.0f; pVertexBufferData[5].tv = 0.0f;
  
  //create vertex buffer for background
  m_pShader = new CShader(2);
    
  m_pShader->AddInputElementDesc(0, DXGI_FORMAT_R32G32B32_FLOAT, "POSITION");
  m_pShader->AddInputElementDesc(12, DXGI_FORMAT_R32G32_FLOAT,  "TEXCOORD");
  m_pShader->VSCreateAndCompile(L"VertexShader.hlsl", "main");
  m_pShader->PSCreateAndCompile(L"PixelShader.hlsl", "main");
    
  // Create constant buffers.
  D3D11_BUFFER_DESC constantBufferDesc = { 0 };
  constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
  constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  constantBufferDesc.CPUAccessFlags = 0;
  constantBufferDesc.MiscFlags = 0;
  constantBufferDesc.StructureByteStride = 0;
    
  m_pDev2->CreateBuffer(&constantBufferDesc, nullptr, &m_pConstantBuffer);
    
  D3D11_BUFFER_DESC VertexBufferDesc;
  VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  VertexBufferDesc.ByteWidth = sizeof(BILLBOARDVERTEX)* 6;
  VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  VertexBufferDesc.CPUAccessFlags = 0;
  VertexBufferDesc.MiscFlags = 0;
  VertexBufferDesc.StructureByteStride = 0;
    
  D3D11_SUBRESOURCE_DATA subresourceData;
  subresourceData.pSysMem = pVertexBufferData;
  subresourceData.SysMemPitch = 0;
  subresourceData.SysMemSlicePitch = 0;
    
  hr = m_pDev2->CreateBuffer(&VertexBufferDesc, &subresourceData, &m_pBackgroundVB);
} //InitBackground

/// Draw the game background.
/// \param x Camera x offset

void CGameRenderer::DrawBackground(float x){
  const float delta = 2.0f * g_nScreenWidth;
  float fQuantizeX = delta * (int)(x / delta - 1.0f) + g_nScreenWidth; //Quantized x coordinate

  UINT nVertexBufferOffset = 0;
  
  UINT nVertexBufferStride = sizeof(BILLBOARDVERTEX);
  m_pDC2->IASetVertexBuffers(0, 1, &m_pBackgroundVB, &nVertexBufferStride, &nVertexBufferOffset);
  m_pDC2->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
  m_pShader->SetShaders();

  ////draw floor
  //if(g_bWireFrame)
  //  m_pDC2->PSSetShaderResources(0, 1, &m_pWireframeTexture); //set wireframe texture
  //else
  //  m_pDC2->PSSetShaderResources(0, 1, &m_pFloorTexture); //set floor texture
  //
  SetWorldMatrix(Vector3(fQuantizeX, 0, 0));
  
  ConstantBuffer constantBufferData; ///< Constant buffer data for shader.

  SetWorldMatrix(Vector3(fQuantizeX - delta, 0, 0));

  constantBufferData.wvp = CalculateWorldViewProjectionMatrix();
  m_pDC2->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &constantBufferData, 0, 0);
  m_pDC2->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
  m_pDC2->Draw(4, 0);

  /*
  //draw backdrop
  if(!g_bWireFrame)
    m_pDC2->PSSetShaderResources(0, 1, &m_pWallTexture);

  SetWorldMatrix(Vector3(fQuantizeX, 0, -600));

  constantBufferData.wvp = CalculateWorldViewProjectionMatrix();
  m_pDC2->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &constantBufferData, 0, 0);
  m_pDC2->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
  m_pDC2->Draw(4, 2);

  SetWorldMatrix(Vector3(fQuantizeX - delta, 0, -600));

  constantBufferData.wvp = CalculateWorldViewProjectionMatrix();
  m_pDC2->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &constantBufferData, 0, 0);
  m_pDC2->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
  m_pDC2->Draw(4, 2);
  */
} //DrawBackground
 
/// Load the background and sprite textures.

void CGameRenderer::LoadTextures(){ 
  ////background
  //LoadTexture(m_pWallTexture, g_cImageFileName[0]);
  //LoadTexture(m_pFloorTexture, g_cImageFileName[1]);
  LoadTexture(m_pWireframeTexture, g_cImageFileName[0]); //black for wireframe

  //sprites
  g_cSpriteManager.Load(PLAYER_OBJECT, "player");
  g_cSpriteManager.Load(PLAYER_UPPER, "playerupper");
  g_cSpriteManager.Load(ENEMYTEST_UPPER, "enemytestupper");
  g_cSpriteManager.Load(ENEMYTEST_DEAD, "enemytestdead");
  g_cSpriteManager.Load(ENEMYTEST_OBJECT, "enemytest");
  g_cSpriteManager.Load(BULLET_OBJECT,"bullet");
  g_cSpriteManager.Load(WALL_OBJECT, "testwall");
  g_cSpriteManager.Load(PISTOL_OBJECT, "pistol");
  g_cSpriteManager.Load(SHOTGUN_OBJECT, "shotgun");
  g_cSpriteManager.Load(SHOTGUN_EQUIPPED, "shotgunequipped");
  g_cSpriteManager.Load(PISTOL_EQUIPPED, "pistolequipped");
  g_cSpriteManager.Load(FLOORING, "flooring");
  g_cSpriteManager.Load(HUD_OBJECT, "hud");
  g_cSpriteManager.Load(LMG_OBJECT, "lmg");
  g_cSpriteManager.Load(LMG_EQUIPPED, "lmgequipped");
  g_cSpriteManager.Load(SLUGGER_OBJECT, "slugger");
  g_cSpriteManager.Load(SLUGGER_EQUIPPED, "sluggerequipped");
  g_cSpriteManager.Load(KNIFE_OBJECT, "knife");
  g_cSpriteManager.Load(KNIFE_EQUIPPED, "knifeequipped");
  g_cSpriteManager.Load(TITLECARD, "titlecard");
  g_cSpriteManager.Load(LIVES_SPRITE, "livessprite");
  g_cSpriteManager.Load(CHICAGO_SLUGGER_OBJECT, "chicagoslugger");
  g_cSpriteManager.Load(CHICAGO_SLUGGER_EQUIPPED, "chicagosluggerequipped");

  m_cScreenText = new CSpriteSheet(21, 37);
  m_cScreenText->Load("Images\\Text.png");
  m_cScreenSelectedText = new CSpriteSheet(21, 37);
  m_cScreenSelectedText->Load("Images\\Text_old.png");
  m_pLivesSprite = g_cSpriteManager.GetSprite(LIVES_SPRITE);
} //LoadTextures

/// All textures used in the game are released - the release function is kind
/// of like a destructor for DirectX entities, which are COM objects.

void CGameRenderer::Release(){ 
  g_cSpriteManager.Release();
/*
  SAFE_RELEASE(m_pWallTexture);
  SAFE_RELEASE(m_pFloorTexture);*/
  SAFE_RELEASE(m_pWireframeTexture);
  SAFE_RELEASE(m_pBackgroundVB);
  SAFE_RELEASE(m_cScreenText);  

  SAFE_DELETE(m_pShader);
  
  CRenderer::Release();
} //Release

//Draw HUD Text
void CGameRenderer::DrawHUDText(char* text, Vector3 p) {
  if(g_bAtMainMenu)
  {
    POINT curpos;
    GetCursorPos(&curpos);         //get screen cursor position
    ScreenToClient(g_HwndApp, &curpos); //translate to client coordinates
    vector<struct TextComponent> tempMenuText = g_cMainMenu.getTextComponents();
    if(text == "HACK TIME" && curpos.y < tempMenuText[0].m_rBox.bottom && curpos.y > tempMenuText[0].m_rBox.top
          && curpos.x > tempMenuText[0].m_rBox.left && curpos.x < tempMenuText[0].m_rBox.right)
    {
      for (unsigned int i = 0; i<strlen(text); i++) {
        char c = text[i];
        if (c >= 'A' && c <= 'Z')
          p = m_cScreenText->Draw(p, 1, 48, c - 'A');
        else if (c >= 'a' && c <= 'z')
          p = m_cScreenText->Draw(p, 1, 95, c - 'a');
        else if (c >= '0' && c <= '9')
          p = m_cScreenText->Draw(p, 1, 1, c - '0');
        else p = m_cScreenText->Draw(p, 1, 1, 10); //blank
      }
    }
    else if(text == "EXIT" && curpos.y < tempMenuText[1].m_rBox.bottom && curpos.y > tempMenuText[1].m_rBox.top
      && curpos.x > tempMenuText[1].m_rBox.left && curpos.x < tempMenuText[1].m_rBox.right)
    {
      for (unsigned int i = 0; i<strlen(text); i++) {
        char c = text[i];
        if (c >= 'A' && c <= 'Z')
          p = m_cScreenText->Draw(p, 1, 48, c - 'A');
        else if (c >= 'a' && c <= 'z')
          p = m_cScreenText->Draw(p, 1, 95, c - 'a');
        else if (c >= '0' && c <= '9')
          p = m_cScreenText->Draw(p, 1, 1, c - '0');
        else p = m_cScreenText->Draw(p, 1, 1, 10); //blank
      }
    }
    else
    {
      for (unsigned int i = 0; i<strlen(text); i++) {
        char c = text[i];
        if (c >= 'A' && c <= 'Z')
          p = m_cScreenSelectedText->Draw(p, 1, 48, c - 'A');
        else if (c >= 'a' && c <= 'z')
          p = m_cScreenSelectedText->Draw(p, 1, 95, c - 'a');
        else if (c >= '0' && c <= '9')
          p = m_cScreenSelectedText->Draw(p, 1, 1, c - '0');
        else p = m_cScreenText->Draw(p, 1, 1, 10); //blank
      }
    }
  }
  else
  {
    for (unsigned int i = 0; i<strlen(text); i++) {
      char c = text[i];
      if (c >= 'A' && c <= 'Z')
        p = m_cScreenText->Draw(p, 1, 48, c - 'A');
      else if (c >= 'a' && c <= 'z')
        p = m_cScreenText->Draw(p, 1, 95, c - 'a');
      else if (c >= '0' && c <= '9')
        p = m_cScreenText->Draw(p, 1, 1, c - '0');
      else p = m_cScreenText->Draw(p, 1, 1, 10); //blank
    }
  }
} //DrawHUDText

//RoundMechanics
//Check every frame (if we're not paused) to see round rewards and spawn NPCs

void CGameRenderer::RoundMechanics()
{
  int rand2;
  int rand3;
  //Spawn enemies proportional to x^1/2
  float NPCmax = pow((float)g_nNPCsKilled * 1.3f, 0.5f) + 1.0f;
  if(g_cNPCManager.size() < NPCmax)      
  {
    //get random vector along outer circle to spawn enemies (r = 3000)
    const float rand = ((float)g_cRandom.number(0, 360) / 360) * 2*PI;
    float r = 1500.0f;
    float x = r * cos(rand);
    float y = r * sin(rand);
    int round = 0;
    for (int i = 0; i < 10 && !m_bRoundHit[i + 1]; i++)
      round = i;
    round++;
    if (round >= 2 && g_cRandom.number(1, 4) >= 3)
      g_cNPCManager.AddObjectByName("enemytest", new CEnemyTestCharacter("enemytest",
        Vector3(x, y, 0), g_cRandom.number(5, 6 + round), new CSlugger(Vector3(x, y, 0), Vector3(0, 0, 0))));
    else
      g_cNPCManager.AddObjectByName("enemytest", new CEnemyTestCharacter("enemytest",
      Vector3(x, y, 0), g_cRandom.number(5, 9 + round), new CKnife(Vector3(x, y, 0), Vector3(0,0,0))));
  }
 
  if(g_bPlayerHit)
  {
    if (!g_bAtMainMenu && !g_cTimer.m_bPaused)
    {
      g_cTimer.pause();
      g_pSoundManager->play(SLOW_DOWN_SOUND);
      g_pSoundManager->volume(2.0);
      g_pSoundManager->pitch(.4, -1, THEME_MUSIC_SOUND);
      g_pSoundManager->volume(.6, -1, THEME_MUSIC_SOUND);
      g_fPitch = .4;
    }
    else if (g_cTimer.getTrueTime() > (g_cTimer.m_nPauseStart + 7000) && !g_bAtMainMenu && !g_bPlayerDead)
    {
      g_cTimer.resume();
      g_bPlayerHit = false;
      g_pSoundManager->play(SPEED_UP_SOUND);
      g_pSoundManager->volume(2.0);
      g_pSoundManager->pitch(1, -1, THEME_MUSIC_SOUND);
      g_pSoundManager->volume(1, -1, THEME_MUSIC_SOUND);
      g_fPitch = 1;
    }
  }

  if(g_bPlayerDead)
  {
    g_cPlayer->m_vVelocity = Vector3(0.0, 0.0, 0.0);
    char killstext[256];
    sprintf_s(killstext, "%d KILLS", g_nNPCsKilled);
    DrawHUDText(killstext, Vector3(g_nScreenWidth / 2.3f, g_nScreenHeight / 1.1f, 1000.0f));
    DrawHUDText("PRESS ENTER TO RESTART", Vector3(g_nScreenWidth / 3.6f, g_nScreenHeight / 1.5f, 1000.0f));
  }
  

  //Tell the player when rounds are completed and reward them
  if(!g_bPlayerDead)
  {
  if(g_nNPCsKilled >= 10 && g_nNPCsKilled < 50)
  {
    if(m_bRoundHit[0])
    {
      m_nLastRoundTexttime = g_cTimer.time();
      m_bRoundHit[0] = false;
      g_cDeadManager.clean();
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
    }
    if(g_cTimer.time() <= m_nLastRoundTexttime + m_nRoundTextLifetime)
    {
      DrawHUDText("ROUND 1 COMPLETE!", Vector3(g_nScreenWidth / 2.8f, g_nScreenHeight - (g_nScreenHeight / 5.0f), 1000.0f));
    }
  }
  else if (g_nNPCsKilled >= 50  && g_nNPCsKilled < 100)
  {
    if (m_bRoundHit[1])
    {
      m_nLastRoundTexttime = g_cTimer.time();
      m_bRoundHit[1] = false;
      g_cDeadManager.clean();
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
    }
    if (g_cTimer.time() <= m_nLastRoundTexttime + m_nRoundTextLifetime)
    {
      DrawHUDText("ROUND 2 COMPLETE!", Vector3(g_nScreenWidth / 2.8f, g_nScreenHeight - (g_nScreenHeight / 5.0f), 1000.0f));
    }
  }
  else if (g_nNPCsKilled >= 100 && g_nNPCsKilled < 150)
  {
    if (m_bRoundHit[2])
    {
      m_nLastRoundTexttime = g_cTimer.time();
      m_bRoundHit[2] = false;
      g_cDeadManager.clean();
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
    }
    if (g_cTimer.time() <= m_nLastRoundTexttime + m_nRoundTextLifetime)
    {
      DrawHUDText("ROUND 3 COMPLETE!", Vector3(g_nScreenWidth / 2.8f, g_nScreenHeight - (g_nScreenHeight / 5.0f), 1000.0f));
    }
  }
  else if (g_nNPCsKilled >= 150 && g_nNPCsKilled < 200)
  {
    if (m_bRoundHit[3])
    {
      m_nLastRoundTexttime = g_cTimer.time();
      m_bRoundHit[3] = false;
      g_cDeadManager.clean();
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f); rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
    }
    if (g_cTimer.time() <= m_nLastRoundTexttime + m_nRoundTextLifetime)
    {
      DrawHUDText("ROUND 4 COMPLETE!", Vector3(g_nScreenWidth / 2.8f, g_nScreenHeight - (g_nScreenHeight / 5.0f), 1000.0f));
    }
  }
  else if (g_nNPCsKilled >= 200 && g_nNPCsKilled < 250)
  {
    if (m_bRoundHit[4])
    {
      m_nLastRoundTexttime = g_cTimer.time();
      m_bRoundHit[4] = false;
      g_cDeadManager.clean();
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f); rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
    }
    if (g_cTimer.time() <= m_nLastRoundTexttime + m_nRoundTextLifetime)
    {
      DrawHUDText("ROUND 5 COMPLETE!", Vector3(g_nScreenWidth / 2.8f, g_nScreenHeight - (g_nScreenHeight / 5.0f), 1000.0f));
    }
  }
  else if (g_nNPCsKilled >= 400 && g_nNPCsKilled < 500)
  {
    if (m_bRoundHit[5])
    {
      m_nLastRoundTexttime = g_cTimer.time();
      m_bRoundHit[5] = false;
      g_cDeadManager.clean();
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f); rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
    }
    if (g_cTimer.time() <= m_nLastRoundTexttime + m_nRoundTextLifetime)
    {
      DrawHUDText("ROUND 6 COMPLETE!", Vector3(g_nScreenWidth / 2.8f, g_nScreenHeight - (g_nScreenHeight / 5.0f), 1000.0f));
    }
  }
  else if (g_nNPCsKilled >= 500 && g_nNPCsKilled < 600)
  {
    if (m_bRoundHit[6])
    {
      m_nLastRoundTexttime = g_cTimer.time();
      m_bRoundHit[6] = false;
      g_cDeadManager.clean();
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f); 
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
    }
    if (g_cTimer.time() <= m_nLastRoundTexttime + m_nRoundTextLifetime)
    {
      DrawHUDText("ROUND 7 COMPLETE!", Vector3(g_nScreenWidth / 2.8f, g_nScreenHeight - (g_nScreenHeight / 5.0f), 1000.0f));
    }
  }
  else if (g_nNPCsKilled >= 600 && g_nNPCsKilled < 750)
  {
    if (m_bRoundHit[7])
    {
      m_nLastRoundTexttime = g_cTimer.time();
      m_bRoundHit[7] = false;
      g_cDeadManager.clean();
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
    }
    if (g_cTimer.time() <= m_nLastRoundTexttime + m_nRoundTextLifetime)
    {
      DrawHUDText("ROUND 8 COMPLETE!", Vector3(g_nScreenWidth / 2.8f, g_nScreenHeight - (g_nScreenHeight / 5.0f), 1000.0f));
    }
  }
  else if (g_nNPCsKilled >= 750 && g_nNPCsKilled < 1000)
  {
    if (m_bRoundHit[8])
    {
      m_nLastRoundTexttime = g_cTimer.time();
      m_bRoundHit[8] = false;
      g_cDeadManager.clean();
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f); 
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
    }
    if (g_cTimer.time() <= m_nLastRoundTexttime + m_nRoundTextLifetime)
    {
      DrawHUDText("ROUND 9 COMPLETE!", Vector3(g_nScreenWidth / 2.8f, g_nScreenHeight - (g_nScreenHeight / 5.0f), 1000.0f));
    }
  }
  else if (g_nNPCsKilled >= 1000)
  {
    if (m_bRoundHit[9])
    {
      m_nLastRoundTexttime = g_cTimer.time();
      m_bRoundHit[9] = false;
      g_cDeadManager.clean();
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
      rand2 = g_cRandom.number(0, 7);
      rand3 = g_cRandom.number(0, 2);
      g_cObjectManager.createObject(g_nWeapons[rand3], g_nWeaponNames[rand3], g_vSpawnLocations[rand2], Vector3(0, 0, 0), 0.0f);
    }
    if (g_cTimer.time() <= m_nLastRoundTexttime + m_nRoundTextLifetime)
    {
      DrawHUDText("YOU WIN!...BUT OUR PRINCESS IS IN ANOTHER CASTLE!", Vector3(g_nScreenWidth / 2.8f, g_nScreenHeight - (g_nScreenHeight / 5.0f), 1000.0f));
    }
  }
  }
}

//Draw HUD
void CGameRenderer::DrawHUD(char* text) {
  float w = g_nScreenWidth / 2.0f;
  float h = g_nScreenHeight / 2.0f;

  //switch to orthographic projection
  XMMATRIX tempProj = m_matProj;
  m_matProj = XMMatrixOrthographicOffCenterLH(-w, w, -h, h, 1.0f, 10000.0f);
  m_matWorld = XMMatrixIdentity();
  m_matView = XMMatrixLookAtLH(Vector3(w, h, 0), Vector3(w, h, 1000.0f), Vector3(0, 1, 0));

  ///clear the depth buffer
  m_pDC2->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  //draw the HUD background sprite
  //C3DSprite* hud = g_cSpriteManager.GetSprite(HUD_OBJECT);
  //Vector3 p = Vector3(0.0f, g_nScreenHeight / 6.0f, 1000.0f);
  //hud->Draw(p, 0.0f, 0);

  vector<struct SpriteComponent> tempMenuSprites = g_cMainMenu.getSpriteComponents();
  vector<struct TextComponent> tempMenuText = g_cMainMenu.getTextComponents();
  //draw menu sprites and text
  if(g_bAtMainMenu)
  {
    for (int i = 0; i < tempMenuSprites.size(); i++)
    {
      tempMenuSprites[i].m_pSpriteComponent->Draw(tempMenuSprites[i].m_vPos, 0.0f);
    }
    
    for (int i = 0; i < tempMenuText.size(); i++)
    {
      DrawHUDText(tempMenuText[i].m_sText, tempMenuText[i].m_vPos);
    }
  }
  else
  {
    DrawHUDText(text, Vector3(g_nScreenWidth - (g_nScreenWidth / 5.0f), g_nScreenHeight / 7.0, 1000.0f));
    //check game state and update
    RoundMechanics();
    char buffer[256];
    sprintf_s(buffer, "x%d", g_cPlayer->getLives());
    DrawHUDText(buffer, Vector3(g_nScreenWidth - (g_nScreenWidth / 6.0f), g_nScreenHeight - (g_nScreenHeight / 9.0), 1000.0f));
    m_pLivesSprite->Draw(Vector3(300.0f, 700.0f, 999.0f), 0.0f);
  }
  //back to perspective projection 
  m_matProj = tempProj;
} //DrawHUD

/// Move all objects, then draw them.
/// \return TRUE if it succeeded

void CGameRenderer::ComposeFrame(){
  if(!g_bAtMainMenu)
  {
    //move the player
    if (g_cPlayer)
      g_cPlayer->move();
    
    if (!g_cTimer.m_bPaused)
    {
      //move everything
      g_cProjectileManager.move(); //move projectiles
      //calculate collisions for projectiles
      g_cProjectileManager.CheckCollision(g_cWallManager, WALL_COLLISION, false);
      g_cProjectileManager.CheckCollision(g_cNPCManager, NPC_COLLISION);

      //check player against bullets
      g_cProjectileManager.CheckCollision(g_cPlayer, PROJECTILE_COLLISION);

      //move rest
      g_cWallManager.move();       //move walls
      g_cNPCManager.move();        //move NPCs
      g_cObjectManager.move();     //move objects

      //g_cMainMenu.Draw(); //draw menu
    }
    g_cNPCManager.GarbageCollect();

    //set camera location
    CGameObject* p = g_cPlayer;
    float x = p->m_vPos.x, y = p->m_vPos.y; //player's current location
    /*y = min(y, g_nScreenHeight/2.0f);
    y = max(y, g_nScreenHeight/4.0f);*/

    Vector3 pos, lookatpt;

    if(m_bCameraDefaultMode){
      pos = Vector3(x + g_nScreenWidth/2, y, p->m_vPos.z + m_fCameraZDistance);
      lookatpt = Vector3(x + g_nScreenWidth/2, y, 1000);
    } //if
    else{
      pos = Vector3(x - 2.5f*g_nScreenWidth, 1000, -3000);
      lookatpt = Vector3(x - g_nScreenWidth, 700, 0);
    } //else
  
    SetViewMatrix(pos, lookatpt);

    //prepare to draw
    m_pDC2->OMSetRenderTargets(1, &m_pRTV, m_pDSV);
    //float clearColor[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    float clearColor[] = { ((cos((((2*PI) * (float)g_cTimer.time()) / 250)) + 1) / 4) + .5, 0.0f, ((cos((((2 * PI) * (float)g_cTimer.time()) / 500)) + 1) / 4) + .5, 0.0f };
    m_pDC2->ClearRenderTargetView(m_pRTV, clearColor);
    m_pDC2->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    //draw
    //DrawBackground(x + g_nScreenWidth/2.0f); //draw background
    g_cWallManager.draw();
    g_cObjectManager.draw(); //draw objects
    g_cProjectileManager.draw();
    g_cDeadManager.draw();
    g_cNPCManager.draw();
    if (g_cPlayer)
      g_cPlayer->draw();

    //if we have a weapon equipped, get bullet count and draw to screen
    CWeapon * Weapon = g_cPlayer->get_weapon();
    char buffer0[256], buffer1[256];

    if(Weapon)
    {
      if(!Weapon->m_bMeleeWeapon)
        sprintf_s(buffer0, "%d", Weapon->getBullets());
      else
        sprintf_s(buffer0, "");
    }
    else
      sprintf_s(buffer0, "");

    DrawHUD(buffer0);
  }
  else
  {
    DrawHUD("");
  }
} //ComposeFrame
 
/// Compose a frame of animation and present it to the video card.

void CGameRenderer::ProcessFrame(){ 
  ComposeFrame(); //compose a frame of animation
  m_pSwapChain2->Present(0, 0); //present it
} //ProcessFrame

/// Toggle between eagle-eye camera (camera pulled back far enough to see
/// backdrop) and the normal game camera.

void CGameRenderer::FlipCameraMode(){
  m_bCameraDefaultMode = !m_bCameraDefaultMode; 
} //FlipCameraMode
