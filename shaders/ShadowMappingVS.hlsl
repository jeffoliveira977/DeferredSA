row_major float4x4 World : register(c0);
row_major float4x4 View : register(c4);
row_major float4x4 Projection : register(c8);

void main(inout float4 position : POSITION0,
          in float2 iTexcoord   : TEXCOORD0,
          in float4 iColor      : TEXCOORD1,
          out float2 oTexcoord  : TEXCOORD0, 
          out float4 oColor     : TEXCOORD1, 
          out float3 depth : TEXCOORD2)
{
    position = mul(position, World);
    depth = position.xyz;
    position = mul(position, View);
    position = mul(position, Projection);
    
    oTexcoord = iTexcoord;
    oColor    = iColor;
    //depth     = position.zw; 
}