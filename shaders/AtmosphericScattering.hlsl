#include "Utilities.hlsl"

sampler2D ScreenSampler : register(s4);

row_major float4x4 ViewInverseMatrix : register(c0);
row_major float4x4 ProjectionMatrix : register(c4);

float3 SkyLightColor : register(c8);
float3 HorizonColor : register(c9);
float3 SunColor : register(c10);
float4 SunDir : register(c11);
float4 FogData : register(c12);

#define FogStart FogData.x
#define FarClip FogData.y
#define ShadowDNBalance FogData.z
#define FogRange  FogData.w

static const float g_fAtmosphereHeight = 8000.0f;
static const float g_fThickAtmosphereHeight = 1200.0f;

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

float3 CalculateRayleighScattering(float CosSqPlOne, float Height)
{
    // Rayleigh phase function approximation
    float phase = 0.75f * CosSqPlOne;
    // Rayleigh density approximation at current camera height
    float density = GetRayleighDensity(Height);
    // Rayleigh light scattering approximation(very rough)
    return (density * phase).xxx;
}

float3 CalculateMieScattering(float CosPhi, float CosSqPlOne, float Height)
{
    // Assymetry factor for mie phase function
    const float g = -0.85;
    // Henyey-Greenstein mie phase function approximation
    float MiePhase = 1.5f * ((1 - g * g) / (2 + g * g)) * (CosSqPlOne / pow(1 + g * g - 2 * g * CosPhi, 1.5f));
    float MieDensity = GetMieDensity(Height);
    // Mie light scattering approximation(very rough)
    return MieDensity * MiePhase;
}
/*!
    Computes atmospheric scattering color and blends it with screen color.
*/
float3 CalculateFogColor(float3 ScreenColor, float3 ViewDir, float3 LightDir, float ViewDepth, float Height, out float3 FullScattering)
{
    float CosPhi = max(dot(ViewDir, LightDir), 0.0);
    float CosSqPlOne = (1 + CosPhi * CosPhi);

    float MieCosPhi = max(-LightDir.z, 0.0);
    float MieCosSqPlOne = (1 + MieCosPhi * MieCosPhi);
    
    float3 SkyColor = lerp(HorizonColor.rgb, SkyLightColor.rgb, min(max(Height + 300.0f, 0.0f) / 600.0f, 1.0f));

    float3 MieScattering = CalculateMieScattering(MieCosPhi, MieCosSqPlOne, Height) * SunColor.rgb * ShadowDNBalance;
    float3 RayleighScattering = CalculateRayleighScattering(CosSqPlOne, Height) * SkyColor;
    
    float3 SunContribution = min(pow(CosPhi, 8.0f), 1.0f) * SunColor.rgb * ShadowDNBalance;
    FullScattering = ((RayleighScattering * 0.75f + MieScattering * 0.5f) + SunContribution * 0.5f);
        
    float FogFadeCoeff = saturate(max(ViewDepth - FogStart, 0) / max(FogRange, 0.001f));
    
    return lerp(ScreenColor, FullScattering, FogFadeCoeff);
}
/*!
    Calculates sky color in specific direction.
*/
float3 GetSkyColor(float3 ViewDir, float3 LightDir, float3 FullScattering)
{
    float CosPhi = max(dot(ViewDir, LightDir), 0.0);

    // Compute sky color at skydome by blending scattering with sun disk
    float3 SunDiskColor = max(FullScattering, SunColor.rgb * 4.5);
    float SunDiskCoeff = CosPhi > 0.9999f ? 1.0f : 0.0f;
    
    return lerp(FullScattering, SunDiskColor, SunDiskCoeff * ShadowDNBalance);
}


float4 main(float2 texCoord :TEXCOORD) : COLOR
{
    
   
    float4 OutLighting;

    const float3 ViewPos = ViewInverseMatrix[3].xyz;

    // Retrieve all needed buffer samples first
    float ViewZ;
    float3 WorldPos;
    float3 normal;
    DecodeDepthNormal(texCoord, FogData.y, ViewZ, normal);

    WorldPositionFromDepth(texCoord, ViewZ, ProjectionMatrix, ViewInverseMatrix, WorldPos);

    
    float4 ScreenColor = tex2D(ScreenSampler, texCoord);
    
    // Directions calculation maybe we should introduce macroses to encapsulate them
    float3 ViewDir = normalize(WorldPos - ViewPos);
    float3 LightDir = normalize(SunDir.xyz);

    // Some coefficients used in fog computation
    float Height = WorldPos.z;
    
    float3 FullScattering;
    float3 ObjectColor = CalculateFogColor(ScreenColor.rgb, ViewDir, LightDir, ViewZ, Height, FullScattering);
    
    float3 SkyColor = GetSkyColor(ViewDir, LightDir, FullScattering);
    
    // TODO: Remove if operation to improve performance, or maybe not
    if (ScreenColor.a <= 0)
        OutLighting.xyz = SkyColor;
    else
        OutLighting.xyz = ObjectColor;
    OutLighting.a = 1;

    return OutLighting;
}