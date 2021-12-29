#include "Utilities.hlsl"
row_major float4x4 ViewInverseMatrix : register(c0);
row_major float4x4 ProjectionMatrix : register(c4);
float4 FogData : register(c8);

struct VertexShaderInput
{
    float4 Position : POSITION0;
    float2 TexCoord : TEXCOORD0;
};

struct VertexShaderOutput
{
    float4 Position : POSITION0;
    float4 TexCoord : TEXCOORD0;
    float2 TexCoordHalfBuffer : TEXCOORD1;
};


sampler2D ssaoSampler : register(s0);

float2 BlurDirection : register(c9);
float4 main(float4 TexCoord : TEXCOORD0) : COLOR
{
    float depth;
    float4 depthNormal = TEXTURE2D_DEPTHNORMAL(TexCoord.xy);
    depth = DecodeFloatRG(depthNormal.zw, FogData.y);
    
	
    half divisor = 1;
    half occ = 0;
	
    clip(depth + 0.9999f);
#ifdef XBOX	
[branch]
#endif
    if (depth > 0)
    {
        occ = tex2D(ssaoSampler, TexCoord.zw).r;

#define NUM_SAMPLERS 2
	
#ifdef XBOX	
		[unroll(NUM_SAMPLERS*2 + 1)]
#endif	
        for (int i = -NUM_SAMPLERS; i <= NUM_SAMPLERS; i++)
        {
            float2 uvOcc = TexCoord.zw + BlurDirection * (i);
            half newOcc = tex2D(ssaoSampler, uvOcc).r;
            float2 uvScreen = TexCoord.xy + BlurDirection * (i);
            float4 depthNormal = TEXTURE2D_DEPTHNORMAL(uvScreen);
            float newDepth = DecodeFloatRG(depthNormal.zw, FogData.y) / FogData.y;
            half depthDif = abs(depth - newDepth) * FogData.y;
            half sz = max(0, 0.2f - depthDif) * 5;
            divisor += sz;
            occ += newOcc * sz;
        }
	
	
        occ /= divisor;

    }
    return 1 - occ;
}