/// \file gamerenderer.h 
/// \brief Definition of the renderer class CGameRenderer.

#pragma once

#include "renderer.h"
#include "defines.h"
#include "Shader.h"

/// \brief The game renderer.
///
/// The game renderer class handles all the nasty Direct3D details associated
/// with game related rendering tasks, including drawing the game background.

class CGameRenderer: public CRenderer{
  private: 
    //Direct3D stuff for background floor and wall
    ID3D11Buffer* m_pBackgroundVB;  ///< Vertex buffer.
    ID3D11ShaderResourceView* m_pWallTexture; ///< Texture for wall.
    ID3D11ShaderResourceView* m_pFloorTexture; ///< Texture for floor.
    ID3D11ShaderResourceView* m_pWireframeTexture; ///< Texture for showing wireframe, all black.
    ID3D11Buffer* m_pConstantBuffer; ///< Constant buffer for shader.
    CShader* m_pShader; ///< Pointer to an instance of the shader class.

    BOOL  m_bCameraDefaultMode; ///< Camera in default mode.

    CSpriteSheet* m_cScreenText;
    CSpriteSheet* m_cScreenSelectedText;
    C3DSprite* m_pLivesSprite;

    int m_nRoundTextLifetime;
    int m_nLastRoundTexttime;

    int m_nTempRound;
 
  public:
    CGameRenderer(); ///< Constructor.
    ~CGameRenderer(); //Destructor

	float m_fCameraZDistance;   //camera's Z Distance From the player
  bool m_bRoundHit[500];

    void InitBackground(); ///< Initialize the background.
    void DrawBackground(float x); ///< Draw the background.
  
    void LoadTextures(); ///< Load textures for image storage.
    void Release(); ///< Release offscreen images.

    void DrawHUDText(char* text, Vector3 p);
    void DrawHUD(char* text);

    void RoundMechanics();

    //void DrawMenuSprite(C3DSprite * sprite, Vector3 s);
    //void DrawMenuText(char * text, Vector3 s);

    void ComposeFrame(); ///< Compose a frame of animation.
    void ProcessFrame(); ///< Process a frame of animation.

    void FlipCameraMode(); ///< Flip the camera mode.
}; //CGameRenderer 
