/// \file SpriteMan.cpp
/// \brief Code for the sprite manager class CSpriteManager.

#include <stdio.h>

#include "SpriteMan.h"
#include "debug.h"
#include "Defines.h"

extern XMLElement* g_xmlSettings; //global XML settings

CSpriteManager::CSpriteManager(){ //constructor
  for(int i = 0; i<NUM_OBJECT_TYPES; i++)
    m_pSprite[i] = nullptr;
} //constructor

CSpriteManager::~CSpriteManager(){ //destructor
  for(int i = 0; i<NUM_OBJECT_TYPES; i++)
    delete m_pSprite[i];
} //destructor

/// Given a file name and extension such as "foo" and "bmp", read
/// in sprite frames from "foo1.bmp", "foo2.bmp," etc.
/// \param object Object type
/// \param file File name prefix
/// \param ext File name extension
/// \param frames Number of frames
/// \return TRUE if load succeeded

C3DSprite* CSpriteManager::Load(ObjectType object,
  const char* file, const char* ext, int frames)
{
  BOOL success = TRUE; //are reading sucessfully
  m_pSprite[object] = new C3DSprite(frames); //get space in array for new sprite

  for(int i = 0; i<frames && success; i++){ //for each frame
    sprintf_s(m_pBuffer, MAX_PATH, "%s%d.%s", file, i, ext); //assemble file name
    DEBUGPRINTF("\n");
    DEBUGPRINTF(m_pBuffer);
    DEBUGPRINTF("\n"); 
    success = m_pSprite[object]->Load(m_pBuffer, i); //load from that file name
    if(!success)
      DEBUGPRINTF("FAILURE TO LOAD IMAGE!\n");
  } //for

  return m_pSprite[object]; // return success, obviously some work needs to be done here
} //Load

/// Load information about the sprite from global variable g_xmlSettings, then
/// load the sprite images as per that information. Abort if something goes wrong.
/// \param object Object type
/// \param name Object name in XML file

void CSpriteManager::Load(ObjectType object, char* name){
  int frames = 0; //number of frames in sprite
  C3DSprite* sprite = nullptr;

  if(g_xmlSettings){ //got "settings" tag
    //get "sprites" tag
    XMLElement* spriteSettings = g_xmlSettings->FirstChildElement("sprites"); //sprites tag
    if(spriteSettings){ //got "sprites" tag
      //load "sprite" tag with correct name attribute
      XMLElement* spr; //sprite element
      //set spr to the first "sprite" tag with the correct name
      spr = spriteSettings->FirstChildElement("sprite");
      while(spr && strcmp(name, spr->Attribute("name"))){
        spr = spr->NextSiblingElement("sprite");
      } //while
      if(spr){ //got "sprite" tag with right name
        //get sprite information from tag
        frames = spr->IntAttribute("frames");
        //now load the sprite from the information loaded
        sprite = Load(object, spr->Attribute("file"), spr->Attribute("ext"), frames);
      } //if
    } //if
  } //if

  if(sprite == nullptr)
    DEBUGPRINTF("Cannot load sprite \"%s\".\n", name);
} //Load

C3DSprite* CSpriteManager::GetSprite(ObjectType object){
  return m_pSprite[object];
} //GetSprite

/// Release all textures from all sprites.

void CSpriteManager::Release(){
  for(int i = 0; i<NUM_OBJECT_TYPES; i++)
    SAFE_RELEASE(m_pSprite[i]);
} //Release