#include "Utilities.hlsl"
#include "Shadow.hlsl"
#include "PBR.hlsl"

#define USE_ADDITIONALGBUFFER	
#define AMBIENT_TERM_TYPE 1

row_major float4x4 ViewInverseMatrix : register(c0);
row_major float4x4 ProjectionMatrix : register(c4);

float3 SkyLightColor : register(c8);
float3 HorizonColor : register(c9);
float3 sunColor : register(c10);
float3 sunDir : register(c11);
float3 fogData : register(c12);

#define FogStart fogData.x
#define FarClip fogData.y
#define ShadowDNBalance fogData.z

ShadowData ShadowBuffer: register(c13);
sampler2D RadianceSampler : register(s4);
sampler2D ShadowSampler2[4] : register(s6);
row_major float4x4 ViewProjectionMatrix : register(c40);
sampler2D ShadowSampler : register(s5);

float4 main(float2 texCoord : TEXCOORD0, float2 vpos:VPOS) : COLOR
{
    float4 AlbedoColor = TEXTURE2D_ALBEDO(texCoord);

    clip(length(AlbedoColor) <= 0);

    float4 Shadow = tex2D(ShadowSampler, texCoord);
    
    float4 Parameters = TEXTURE2D_MATERIALPROPS(texCoord);
    float SpecIntensity = Parameters.x;
    float Roughness = 1 - Parameters.y;
    
    float4 color;
    
    float depth;
    float3 normal;
    DecodeDepthNormal(texCoord, fogData.y, depth, normal);
   // normal = normalize(normal);
   // normal = mul(normal, (float3x3) ViewInverseMatrix);
        
    float3 worldPosition;
    WorldPositionFromDepth(texCoord, depth, ProjectionMatrix, ViewInverseMatrix, worldPosition);
    
    float FarClip2 = 160.0;
    float FogStart2 = 0.0;
    float fogdist;
    fogdist = worldPosition.z;
    float fadefact = (FarClip2 - depth) / (FarClip2 - FogStart2);
    fadefact = saturate(1.0 - fadefact);
    
    Shadow = lerp(Shadow, 1.0, fadefact); 
    Shadow = lerp(1.0, Shadow, 0.7); 
    float3 lightDir = normalize(sunDir);
    float3 ViewDir = normalize(worldPosition.xyz - ViewInverseMatrix[3].xyz); // View direction vector

    float DiffuseTerm, SpecularTerm;
    CalculateDiffuseTerm_ViewDependent(normal, lightDir.xyz, -ViewDir, DiffuseTerm, Roughness);
    CalculateSpecularTerm(normal, lightDir.xyz, -ViewDir, Roughness, SpecularTerm);
    
    float3 Radiance = tex2D(RadianceSampler, texCoord);
    
    float2 Lighting = float2(DiffuseTerm, SpecularTerm * SpecIntensity) * (ShadowDNBalance <= 0.0 ? 1.0 : Shadow);
    color = float4(Lighting.x * sunColor.rgb + 0.6 + Radiance * saturate(1.0f - ShadowDNBalance + 0.2f), Lighting.y);
    
    return color;
}