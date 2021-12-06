#include "Utilities.hlsl"
#include "Shadow.hlsl"
#include "PBR.hlsl"

float3 HorizonColor : register(c8);
float3 SkyLightColor : register(c9);
float3 SunColor : register(c10);
float3 SunPosition : register(c11);
float3 FogData : register(c12);
float3 CameraPosition : register(c13);
float4 MaterialColor : register(c14);
float4 MaterialProps : register(c15);

#define fSpecularIntensity MaterialProps.x
#define fGlossiness MaterialProps.y
#define DNBalance MaterialProps.z
#define FogStart FogData.x
#define FarClip FogData.y
#define FogRange FogData.z

bool4 Info : register(b0);

#define HasTexture Info.x
#define ForceNormalMap Info.y
#define HasNormalMap Info.z

struct VS_Output
{
    float4 Position      : POSITION0;
	float2 Texcoord      : TEXCOORD0;
	float3 Normal        : TEXCOORD1;
    float3 Binormal      : TEXCOORD2;
    float3 Tangent       : TEXCOORD3;
    float3 WorldPosition : TEXCOORD4;
	float4 Color         : TEXCOORD5;
    float Depth          : DEPTH;
};

sampler2D Diffuse : register(s0);
sampler2D NormalMap : register(s1);
ShadowData ShadowBuffer : register(c16);
sampler2D ShadowSampler[4] : register(s2);


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

    float3 MieScattering = CalculateMieScattering(MieCosPhi, MieCosSqPlOne, Height) * SunColor.rgb * DNBalance;
    float3 RayleighScattering = CalculateRayleighScattering(CosSqPlOne, Height) * SkyColor;
    
    float3 SunContribution = min(pow(CosPhi, 8.0f), 1.0f) * SunColor.rgb * DNBalance;
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
    
    return lerp(FullScattering, SunDiskColor, SunDiskCoeff * DNBalance);
}

float4 main(VS_Output input, float2 vpos :VPOS) : COLOR
{
    //float3 normal = normalize(input.Normal);
    ////normal.xy = BiasD2(normal);
    ////normal = TwoChannelNormalX2(normal.xy);
    
    //if (HasNormalMap)
    //{
    //    float3 normalMap = tex2D(NormalMap, input.Texcoord).rgb;
    //    normal = PeturbNormal(normalMap, input.WorldPosition, normal, input.Texcoord);
    //}
    //else if (ForceNormalMap)
    //{
    //    float3x3 c = img3x3(Diffuse, input.Texcoord, 512, 0);
    //    float3 normalMap = height2normal_sobel(c);
    //    normalMap = normalize(float3(normalMap.xy, normalMap.z * MaterialProps.w));
    //    normal = PeturbNormal(normalMap, input.WorldPosition, normal, input.Texcoord);
    //} 
    
    //float specularFactor = MaterialProps.x;
    //float shininessFactor = MaterialProps.y;
    
    //float4 outColor;
    //outColor.rgb = MaterialColor.rgb * input.Color.rgb;
    //outColor.rgb += max(input.Color.rgb, 0.2) * lerp(0.5, 1, DNBalance);
    //outColor.a = MaterialColor.a * input.Color.a;
    
    //float3 lightDir = normalize(SunPosition);
    //float3 viewDir = normalize(CameraPosition);

    //float3 d = lerp(SkyTop, SkyBottom, saturate(normal.z)) * 1.34;
    //outColor.rgb *= LambertLighting(lightDir, normal, d*SunColor*0.5) + 0.2;
    //float specular = PhongSpecular(lightDir, viewDir, normal, shininessFactor) * specularFactor;

    //if (HasTexture)
    //    outColor *= tex2D(Diffuse, input.Texcoord);
    
    //outColor.rgb += specular;
    
    //float3 skyColor = lerp(SkyTop, SkyBottom, saturate((input.WorldPosition.z - 20) * (1.0f / 500.0f)));
    //float fHeightMultiplier = saturate((input.WorldPosition.z - 20) * (1.0f / 500.0f));
    //float fFogCoef = saturate(max(input.Depth - FogStart, 0) / max(FarClip - FogStart, 0.001f)) * (1 - fHeightMultiplier);
    //outColor.rgb = lerp(outColor.rgb, skyColor, fFogCoef);
    
    
    
    const float3 ViewPos = CameraPosition.xyz;  
    float3 WorldPos = input.WorldPosition.xyz;
    float3 Normals = input.Normal.xyz * 0.5f + 0.5f;
    Normals.xy = Normals * 2 - 1;
    Normals.z = sqrt(max(1 - dot(Normals.xy, Normals.xy), 0.0f));
    Normals = normalize(Normals);
    
    float3 ViewDir = normalize(WorldPos.xyz - ViewPos);
    float3 LightDir = normalize(SunPosition.xyz);

    
    float SpecIntensity = MaterialProps.x;
    float Roughness = 1 - MaterialProps.y;
    
    float DiffuseTerm, SpecularTerm;
    CalculateDiffuseTerm_ViewDependent(Normals.xyz, LightDir, ViewDir, DiffuseTerm, Roughness);
    CalculateSpecularTerm(Normals.xyz, LightDir, -ViewDir, Roughness, SpecularTerm);
    // clamp to avoid unrealisticly high values
   // SpecularTerm = min(SpecularTerm, 16.0f);
    //DiffuseTerm *= DNBalance;

    float4 albedoSample = MaterialColor;
    if (HasTexture)
        albedoSample *= tex2D(Diffuse, input.Texcoord);
    
    float4 outColor;
    float ShadowTerm = DrawShadow(ShadowSampler, WorldPos, length(WorldPos.xyz - ViewPos), WorldPos, ShadowBuffer) * DNBalance;

    //float3 ReflDir = normalize(reflect(ViewDir, normalize(Normals.xyz)));
    //float3 FullScattering;

    //float3 ObjectColor = CalculateFogColor(float3(0, 0, 0), ReflDir, LightDir, 1000, 0, FullScattering);
    //float3 SkyColor = GetSkyColor(ReflDir, LightDir, FullScattering);

    //float3 ReflectionFallBack = 0.0f;
    ////ReflDir.x *= -1;
  
    ////float4 CubeMap =
    ////    txCubeMap.SampleLevel(samLinear, ReflDir, (1 - fGlossiness) * 9.0f);
    ////ReflectionFallBack = lerp(CubeMap.rgb, SkyColor, 1 - CubeMap.a);
    
    //float3 sun_lighting = DiffuseTerm * ShadowTerm * SunColor.rgb;
    //float3 radiance = input.Color.rgb * saturate(1.0f - DNBalance + 0.2f);
    //// todo: add lighting methods for forward renderer
    
    //outColor.rgb = albedoSample.rgb * (sun_lighting + radiance + SkyLightColor.rgb * 0.3f) +
    //    SpecularTerm * fSpecularIntensity * DNBalance * SunColor.rgb * ShadowTerm + ReflectionFallBack * fSpecularIntensity;
   
    //outColor.rgb = CalculateFogColor(outColor.rgb, ViewDir, LightDir, input.Depth, WorldPos.z, FullScattering);
    //outColor.a = albedoSample.a * input.Color.a;
    
    //float3 Radiance = i.vColor.rgb * saturate(1.0f - vSunLightDir.w + 0.2f);

    //float2 Lighting = float2(DiffuseTerm, SpecularTerm * SpecIntensity) * ShadowTerm;
    //outColor.xyzw = float4(Lighting.x * vSunColor.rgb + Radiance.rgb * saturate(1.0f - vSunLightDir.w + 0.2f), Lighting.y);
    float3 Radiance = input.Color * lerp(0.25f, 1.0f, 1 - DNBalance);
   
    float2 Lighting = float2(DiffuseTerm, SpecularTerm * SpecIntensity) * ShadowTerm;
    outColor.xyzw = float4(Lighting.x * SunColor.rgb + Radiance.rgb * saturate(1.0f - DNBalance + 0.2f), Lighting.y);

    float3 ReflDir = normalize(reflect(ViewDir, normalize(Normals.xyz)));
    ReflDir.x *= -1;
    
    float3 FullScattering;
    float3 ObjectColor = CalculateFogColor(float3(0, 0, 0), ViewDir, SunPosition.xyz, 1000, 0, FullScattering);
  
    float3 SkyColor = GetSkyColor(ViewDir, SunPosition.xyz, FullScattering);
    float3 ReflectionTerm = 0;
    
    float3 Diffuse = (outColor.xyz + SkyLightColor.rgb * 0.3f);
    SpecularTerm = (outColor.w * outColor.xyz);
    
    float FresnelCoeff = MicrofacetFresnel(Normals, -ViewDir, MaterialProps.y);
    outColor.xyz = Diffuse * albedoSample.rgb + SpecularTerm * MaterialProps.x + ReflectionTerm * FresnelCoeff * MaterialProps.x;
    outColor.rgb = CalculateFogColor(outColor.rgb, ViewDir, SunPosition.xyz, input.Depth, WorldPos.z, FullScattering);
    outColor.a = albedoSample.a;
    return outColor;
}