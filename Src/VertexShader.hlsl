cbuffer cbPerObject: register(b0){
  float4x4 gWVP;
  float u0, u1, v0, v1;
}; //cbPerObject

void main(float3 iPosL: POSITION, float2 texC: TEXCOORD,
  out float4 oPosH: SV_POSITION, out float2 texo: TEXCOORD)
{
  oPosH = mul(float4(iPosL, 1.0f), gWVP); //transform to homogeneous clip space
  if(texC.x == 0.0f)texo.x = u0;
  else if(texC.x == 1.0f)texo.x = u1;
  else texo.x = texC.x;

  if(texC.y == 0.0f)texo.y = v0;
  else if(texC.y == 1.0f)texo.y = v1;
  else texo.y = texC.y;
  //texo = texC; //just pass vertex color into the pixel shader
} //main