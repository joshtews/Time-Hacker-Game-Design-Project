/// \file SpriteMan.h
/// \brief Interface for the sprite manager class CSpriteManager.

#pragma once

#include "defines.h"
#include "sprite.h"

/// \brief The sprite manager. 
///
/// The sprite manager is responsible for managing sprites.

class CSpriteManager{
  private:
    C3DSprite* m_pSprite[NUM_OBJECT_TYPES]; ///< Sprite pointers.
    char m_pBuffer[MAX_PATH]; ///< File name buffer.
    C3DSprite* Load(ObjectType object,
      const char* file, const char* ext, int frames); ///< Load sprite.

  public:
    CSpriteManager(); ///< Constructor.
    ~CSpriteManager(); ///< Destructor.
    void Load(ObjectType object, char* name); ///< Load sprite.
    C3DSprite* GetSprite(ObjectType object); ///< Get sprite for object.
    void Release();  ///< Release all sprites.
}; //CSpriteManager
