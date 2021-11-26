#include "Utilities.hlsl"
float2 FogData : register(c0);

#define FogStart FogData.x
#define FarClip FogData.y

static float fDepthAttenuation = 0.6;
static float fDepthAttenuationPower = 1.5;

struct PSInput
{
    float4 Position : POSITION0;
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
    float3 TexProj : TEXCOORD1;
    float2 MatDepth : TEXCOORD2;
};


sampler2D Sampler0 : register(s0);

float countDepthSpread(float sceneDepth, float pixelDepth, float depthSpread, float attenuationPower)
{
    if (pixelDepth > sceneDepth) 
        return pow(saturate((sceneDepth - (pixelDepth - depthSpread)) / depthSpread), attenuationPower);
    else
        return 1;
}

float4 main(PSInput PS, float2 vpos : VPOS) : COLOR0
{
    float2 sPixelSize = float2(1920.0, 1080.0);
    float2 txcoord = vpos / sPixelSize;
   
    float2 TexProj = PS.TexProj.xy / PS.TexProj.z;
    TexProj += sPixelSize.xy * 0.5;
    float pixelDepth = (PS.MatDepth.x / PS.MatDepth.y);
    
    float depth;
    float3 normal;
    float3 worldPosition;
    DecodeDepthNormal(TexProj, FogData.y, depth, normal);
	
    float depthMul = countDepthSpread(depth, pixelDepth, fDepthAttenuation, fDepthAttenuationPower);
    float4 finalColor = tex2D(Sampler0, PS.TexCoord);
    finalColor *= PS.Diffuse;
    finalColor.a *= depthMul;
	
    return saturate(finalColor);
}