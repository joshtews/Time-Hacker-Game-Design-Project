/// \file sprite.cpp
/// \brief Code for the base sprite class C3DSprite.

#include "sprite.h"
#include "gamerenderer.h"
#include "debug.h"

extern CGameRenderer GameRenderer;
extern int g_nScreenWidth;
extern BOOL g_bWireFrame;

C3DSprite::C3DSprite(int framecount){ //constructor
  HRESULT hr = 0; //D3D result

  m_nFrameCount = framecount; //number of frames
  m_pTexture = new ID3D11ShaderResourceView*[framecount]; //texture array
  for(int i=0; i<framecount; i++) 
    m_pTexture[i] = nullptr; //null it out

  m_pVertexBuffer = nullptr; //vertex buffer

  m_pVertexBufferData = new BILLBOARDVERTEX[4];

  m_pShader = new CShader(2);

  m_pShader->AddInputElementDesc(0, DXGI_FORMAT_R32G32B32_FLOAT, "POSITION");
  m_pShader->AddInputElementDesc(12, DXGI_FORMAT_R32G32_FLOAT, "TEXCOORD");
  m_pShader->VSCreateAndCompile(L"VertexShader.hlsl", "main");
  m_pShader->PSCreateAndCompile(L"PixelShader.hlsl", "main");

  D3D11_BLEND_DESC1 blendDesc;
  blendDesc.AlphaToCoverageEnable = FALSE;
  blendDesc.IndependentBlendEnable = FALSE;
  blendDesc.RenderTarget[0].BlendEnable = TRUE;
  blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  blendDesc.RenderTarget[0].LogicOp = D3D11_LOGIC_OP_CLEAR;
  blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
  blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
  blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;

  hr = GameRenderer.m_pDev2->CreateBlendState1(&blendDesc, &m_pBlendState);

  // Create constant buffers.
  D3D11_BUFFER_DESC constantBufferDesc = { 0 };
  constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
  constantBufferDesc.CPUAccessFlags = 0;
  constantBufferDesc.MiscFlags = 0;
  constantBufferDesc.StructureByteStride = 0;
  constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

  GameRenderer.m_pDev2->CreateBuffer(&constantBufferDesc, nullptr, &m_pConstantBuffer);

  m_VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  m_VertexBufferDesc.ByteWidth = sizeof(BILLBOARDVERTEX)* 4;
  m_VertexBufferDesc.CPUAccessFlags = 0;
  m_VertexBufferDesc.MiscFlags = 0;
  m_VertexBufferDesc.StructureByteStride = 0;
  m_VertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
} //constructor

C3DSprite::~C3DSprite(){ //destructor
  delete [] m_pTexture;
  delete [] m_pVertexBufferData;
  delete m_pShader;
} //destructor

/// Load the sprite image into a texture from a given file
/// name and create a vertex buffer for the billboard image containing 4 
/// corner vertices spaced apart the appropriate width and height.
/// \param filename The name of the image file
/// \param frame Frame number

BOOL C3DSprite::Load(char* filename, int frame){
  GameRenderer.LoadTexture(m_pTexture[frame], filename, &m_nWidth, &m_nHeight);

  HRESULT hr = 0;
  if(m_pVertexBuffer == nullptr){ //create only when first frame is loaded  
    //load vertex buffer
    float w = m_nWidth/2.0f;
    float h = m_nHeight/2.0f;
  
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
/// \param angle Angle to rotate sprite
/// \param frame Frame number

void C3DSprite::Draw(Vector3 p, float angle, int frame){
  p.x += g_nScreenWidth/2.0f;
  GameRenderer.SetWorldMatrix(Vector3(p.x, p.y, p.z), angle);

  m_pShader->SetShaders();

  GameRenderer.m_pDC2->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
  GameRenderer.m_pDC2->OMSetBlendState(m_pBlendState, nullptr, 0xffffffff);
  
  ConstantBuffer constantBufferData; 
  constantBufferData.wvp = GameRenderer.CalculateWorldViewProjectionMatrix();
  constantBufferData.u0 = 0.0f;
  constantBufferData.u1 = 1.0f;
  constantBufferData.v0 = 0.0f;
  constantBufferData.v1 = 1.0f;
  GameRenderer.m_pDC2->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &constantBufferData, 0, 0);
  GameRenderer.m_pDC2->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

  //set texture
  if(!g_bWireFrame && frame < m_nFrameCount && m_pTexture[frame]) //if ok
    GameRenderer.m_pDC2->PSSetShaderResources(0, 1, &m_pTexture[frame]); //set sprite texture

  //draw sprite (finally)
  UINT offset = 0;
  UINT nVertexBufferStride = sizeof(BILLBOARDVERTEX);
  GameRenderer.m_pDC2->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &nVertexBufferStride, &offset);
  GameRenderer.m_pDC2->Draw(4, 0);
} //Draw

/// Release the sprite vertex buffer, blend state, and textures.

void C3DSprite::Release(){
  SAFE_RELEASE(m_pVertexBuffer); //release vertex buffer
  SAFE_RELEASE(m_pBlendState); //release blend state

  for(int i=0; i<m_nFrameCount; i++) //for each frame
    SAFE_RELEASE(m_pTexture[i]); //release texture
} //Release