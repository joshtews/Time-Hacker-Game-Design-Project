/// \file SpriteSheet.cpp
/// \brief Code for the sprite sheet class CSpriteSheet.

#include "sprite.h"
#include "gamerenderer.h"
#include "debug.h"

#include "SpriteSheet.h"

extern CGameRenderer GameRenderer;
extern int g_nScreenWidth;
extern BOOL g_bWireFrame;

/// \param width Width of sprite frame
/// \param height Height of sprite frame

CSpriteSheet::CSpriteSheet(int width, int height): C3DSprite(1){ //constructor
  m_nFrameWidth = width;
  m_nFrameHeight = height;
} //constructor

/// Load the sprite image into a texture from a given file
/// name and create a vertex buffer for the billboard image containing 4 
/// corner vertices spaced apart the appropriate width and height.
/// \param filename The name of the image file

BOOL CSpriteSheet::Load(char* filename){
  GameRenderer.LoadTexture(m_pTexture[0], filename, &m_nWidth, &m_nHeight);

  HRESULT hr = 0;
  if(m_pVertexBuffer == nullptr){ //create only when first frame is loaded  
    //load vertex buffer
    float w = m_nFrameWidth/2.0f;
    float h = m_nFrameHeight/2.0f;
  
    //vertex information, first triangle in clockwise order
    m_pVertexBufferData[0].p = Vector3(w, h, 0.0f);
    m_pVertexBufferData[0].tu = 1.0f; m_pVertexBufferData[0].tv = 0.0f;
  
    m_pVertexBufferData[1].p = Vector3(w, -h, 0.0f);
    m_pVertexBufferData[1].tu = 1.0f; m_pVertexBufferData[1].tv = 1.0f;
  
    m_pVertexBufferData[2].p = Vector3(-w, h, 0.0f);
    m_pVertexBufferData[2].tu = 0.0f; m_pVertexBufferData[2].tv = 0.0f;
  
    m_pVertexBufferData[3].p = Vector3(-w, -h, 0.0f);
    m_pVertexBufferData[3].tu = 0.0f; m_pVertexBufferData[3].tv = 1.0f;
  
    //create vertex buffer
    D3D11_SUBRESOURCE_DATA subresourceData;
    subresourceData.pSysMem = m_pVertexBufferData;
    subresourceData.SysMemPitch = 0;
    subresourceData.SysMemSlicePitch = 0;
    
    hr = GameRenderer.m_pDev2->CreateBuffer(&m_VertexBufferDesc, &subresourceData, &m_pVertexBuffer);
  } //if
  
  return SUCCEEDED(hr); //successful
} //Load

/// Draw the sprite image with its center at a given point in 3D space
/// unless m_bBottomOrigin is TRUE, in which case the bottom center
/// of the sprite is drawn at that point.
/// \param p Point in 3D space at which to draw the sprite
/// \param x X coordinate of first sprite frame in the row
/// \param y Y coordinate of first sprite frame in the row
/// \param xoffset Offset along row to desired frame, in number of frames
/// \return Position that next frame should be drawn

Vector3 CSpriteSheet::Draw(Vector3 p, int x, int y, int xoffset){
  Vector3 result = p + Vector3((float)m_nFrameWidth, 0.0f, 0.0f);
  x += xoffset*(m_nFrameWidth + 1);
  GameRenderer.SetWorldMatrix(p, 0.0f);

  m_pShader->SetShaders();

  GameRenderer.m_pDC2->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
  GameRenderer.m_pDC2->OMSetBlendState(m_pBlendState, nullptr, 0xffffffff);
  
  ConstantBuffer constantBufferData; 
  constantBufferData.wvp = GameRenderer.CalculateWorldViewProjectionMatrix();
  constantBufferData.u0 = (float)x/(m_nWidth - 1);
  constantBufferData.u1 = (float)(x + m_nFrameWidth)/(m_nWidth - 1);
  constantBufferData.v0 = (float)y/(m_nHeight - 1);
  constantBufferData.v1 = (float)(y + m_nFrameHeight)/(m_nHeight - 1);
  GameRenderer.m_pDC2->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &constantBufferData, 0, 0);
  GameRenderer.m_pDC2->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

  //set texture
  if(!g_bWireFrame && m_pTexture[0]) //if ok
    GameRenderer.m_pDC2->PSSetShaderResources(0, 1, &m_pTexture[0]); //set sprite texture

  //draw sprite (finally)
  UINT offset = 0;
  UINT nVertexBufferStride = sizeof(BILLBOARDVERTEX);
  GameRenderer.m_pDC2->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &nVertexBufferStride, &offset);
  GameRenderer.m_pDC2->Draw(4, 0);

  return result;
} //Draw