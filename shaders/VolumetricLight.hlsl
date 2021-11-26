#include "Utilities.hlsl"
#include "Shadow.hlsl"

row_major float4x4 ViewInverseMatrix : register(c0);
row_major float4x4 ProjectionMatrix : register(c4);

float3 SkyLightColor : register(c8);
float3 HorizonColor : register(c9);
float3 SunColor : register(c10);
float3 SunDir : register(c11);
float3 FogData : register(c12);
float3 VolumetricLightParams : register(c13);
ShadowData ShadowBuffer : register(c14);

#define RaymarchingDistance VolumetricLightParams.x
#define SunlightBlendOffset VolumetricLightParams.y
#define SunlightIntensity VolumetricLightParams.z

#define FogStart FogData.x
#define FarClip FogData.y

sampler2D ShadowSampler[4] : register(s4);

static const float g_fAtmosphereHeight = 8000.0f;
static const float g_fThickAtmosphereHeight = 1200.0f;

#ifndef SUNLIGHT_RM_STEPS
#define SUNLIGHT_RM_STEPS 16
#endif

float Bayer4x4(float2 index)
{
    const float4x4 Bayer = { float4(1, 13, 4, 16), float4(9, 5, 12, 8), float4(3, 15, 2, 14), float4(11, 7, 10, 6) };
    return Bayer[index.x % 4][index.y % 4] * 0.0625;
}

/*!
	Rayleigh density at given height
*/
inline float GetRayleighDensity(float Height)
{
    return exp(-Height / g_fAtmosphereHeight);
}

/*!
	Mie density at given height
*/
inline float GetMieDensity(float Height)
{
    return exp(-Height / g_fThickAtmosphereHeight);
}
/*!
    Calculates sun-light volumetric scattering
    TODO: improve this alghorithm quality and performance, by introducing
   jittering or maybe something more
*/

/*!
    Calculates sun-light volumetric scattering.
*/
float3 ComputeSunRaysWithLighting(float2 position, float3 LightDir,
                                   float3 ViewPos, float3 WorldPos,
                                   float Length, ShadowData ShadowBuffer)
{
    const int SunRaySampleCount = SUNLIGHT_RM_STEPS;
    float3 ViewDir = normalize(WorldPos - ViewPos);
    float StepLength = Length / (float) SunRaySampleCount;

    float3 Step = ViewDir * StepLength;
    float Jitter = Bayer4x4(position.xy);
    
    float3 CurrentPos = ViewPos + Step * Jitter;
    float3 ResultColor = 0.0;

    float SunBlend = saturate(dot(ViewDir, LightDir) + SunlightBlendOffset);
    SunBlend = SunBlend * SunBlend * 2.0;

    float3 SunRayContrib = SunBlend * SunColor.rgb * 4.5 * 0.5f;
    const float3 RayleighContrib = SkyLightColor.rgb * 0.25f;
    const float3 MieContrib = HorizonColor.rgb * 0.25f;

    for (int index = 0; index < SunRaySampleCount; index++)
    {
        float3 CurrentRay = SampleShadowCascadesUnfiltered(ShadowSampler, StepLength * (index + Jitter), CurrentPos, ShadowBuffer);

        ResultColor += CurrentRay *
                       (SunRayContrib +
                         RayleighContrib * GetRayleighDensity(CurrentPos.z) +
                         GetMieDensity(CurrentPos.z) * MieContrib);

        CurrentPos += Step;
    }
    return ResultColor / (float) SunRaySampleCount;
}

float4 main(float2 Position : VPOS, float2 texCoord : TEXCOORD) : COLOR
{
    float4 OutLighting;
    const float3 ViewPos = ViewInverseMatrix[3].xyz;

    float ViewZ;
    float3 WorldPos;
    float3 normal;
    DecodeDepthNormal(texCoord, FarClip, ViewZ, normal);

    WorldPositionFromDepth(texCoord, ViewZ, ProjectionMatrix, ViewInverseMatrix, WorldPos);
    
    float3 ViewDir = normalize(WorldPos - ViewPos);
    float3 LightDir = normalize(SunDir.xyz);
    
    float SunRaysIntensity = min(max(dot(ViewDir, LightDir) + SunlightBlendOffset, 0.0f), 1.0f) * SunlightIntensity;

    float3 SunRays = ComputeSunRaysWithLighting(Position, LightDir, ViewPos, WorldPos,
        min(length(WorldPos - ViewPos), RaymarchingDistance), ShadowBuffer);

    OutLighting.rgb = SunRays * SunlightIntensity;
    OutLighting.a = 1;

    return OutLighting;
}