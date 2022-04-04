/// \file SpriteSheet.h 
/// \brief Definition of the sprite sheet class CSpriteSheet.

#pragma once

#include "Sprite.h"

/// \brief The sprite sheet.
///
/// A sprite sheet consists of a single frame containing all of the
/// sprite images. They are drawn from selecting a bounding rectangle
/// around the actual frame by adjusting the texture coordinates in
/// the vertex shader.

class CSpriteSheet: public C3DSprite{
  private:
    int m_nFrameWidth; ///< Width of the sprite frame.
    int m_nFrameHeight; ///< Height of the sprite frame.
  public:
    CSpriteSheet(int width, int height); ///< Constructor.
    Vector3 Draw(Vector3 p, int x, int y, int xoffset); ///< Draw a sprite.
    BOOL Load(char* filename); ///< Load the sprite sheet.
}; //CSpriteSheet