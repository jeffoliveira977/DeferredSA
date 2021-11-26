#include "Utilities.hlsl"
#include "Shadow.hlsl"

float3 SkyLightColor : register(c8);
float3 HorizonColor : register(c9);
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

inline float pow5(float v)
{
    return v * v * v * v * v;
}
float MicrofacetNDF_GGX(in float3 vNormal, in float3 vHalfWay, in float fRoughness)
{
    float fRoughnessSqr = fRoughness * fRoughness;

    float fCosAlphaSqr = saturate(dot(vNormal, vHalfWay)); // Alpha is angle between normal and half-way vector
	//fCosAlphaSqr *= fCosAlphaSqr;		// need to check if can be optimized to use less instructions
    float fDiv = ((fRoughnessSqr - 1) * fCosAlphaSqr + 1);
    float fNDCoeff = fRoughnessSqr;
    fNDCoeff /= PI * (fDiv * fDiv + 1e-7f);

    half d = (fCosAlphaSqr * fRoughnessSqr - fCosAlphaSqr) * fCosAlphaSqr + 1.0f; // 2 mad
    return fRoughnessSqr / (d * d + 1e-7f);
	//return fNDCoeff;
	//float 
}
float GGX_PartialGeometry(float cosThetaN, float alpha)
{
    float cosTheta_sqr = saturate(cosThetaN * cosThetaN);
    float tan2 = (1 - cosTheta_sqr) / cosTheta_sqr;
    float GP = 2 / (1 + sqrt(1 + alpha * alpha * tan2));
    return GP;
}
float MicrofacetFresnel(in float3 LightDir, in float3 Normal, in float fRoughness)
{
    float IOR = 1.5f;
    float f0 = (1 - IOR) / (1 + IOR);
    f0 *= f0;
    // Cosine between light and normal
    float CosPhi = max(dot(LightDir, Normal), 0);

    return f0 + (1 - f0) * pow5(1 - CosPhi);
}
float GGX_Distribution(float cosThetaNH, float alpha)
{
    float alpha2 = alpha * alpha;
    float NH_sqr = saturate(cosThetaNH * cosThetaNH);
    float den = NH_sqr * alpha2 + (1.0 - NH_sqr);
    return alpha2 / (PI * den * den);
}
inline float SmithJointGGXVisibilityTerm(float NdotL, float NdotV, float roughness)
{
	// Original formulation:
	//	lambda_v	= (-1 + sqrt(a2 * (1 - NdotL2) / NdotL2 + 1)) * 0.5f;
	//	lambda_l	= (-1 + sqrt(a2 * (1 - NdotV2) / NdotV2 + 1)) * 0.5f;
	//	G			= 1 / (1 + lambda_v + lambda_l);

	// Reorder code to be more optimal
    float a = roughness;
    float a2 = a * a;

    float lambdaV = NdotL * sqrt((-NdotV * a2 + NdotV) * NdotV + a2);
    float lambdaL = NdotV * sqrt((-NdotL * a2 + NdotL) * NdotL + a2);

	// Simplify visibility term: (2.0f * NdotL * NdotV) /  ((4.0f * NdotL * NdotV) * (lambda_v + lambda_l + 1e-5f));
    return 0.5f / (lambdaV + lambdaL + 1e-5f);
}
void CalculateSpecularTerm(in float3 vNormal, in float3 vLightDir, in float3 vViewDir, in float fRoughness, out float fSpecularTerm)
{
    float3 vHalfWay = normalize(vLightDir + vViewDir);
    float fresnelTerm = MicrofacetFresnel(vLightDir, vHalfWay, fRoughness);
    float roug_sqr = fRoughness * fRoughness;
    float G = GGX_PartialGeometry(dot(vNormal, vViewDir), roug_sqr) * GGX_PartialGeometry(dot(vNormal, vLightDir), roug_sqr);
    float D = GGX_Distribution(dot(vNormal, vHalfWay), roug_sqr);
    float ndfTerm = MicrofacetNDF_GGX(vNormal, vHalfWay, fRoughness * fRoughness);
    float ndl = saturate(dot(vNormal, vLightDir));
    float ndv = saturate(dot(vNormal, vViewDir));
    float visibilityTerm = SmithJointGGXVisibilityTerm(ndl, ndv, fRoughness * fRoughness);
    /*if (ndv <= 0.0 || ndl <= 0.0)
    {
        fSpecularTerm = 0.0f;
        return;
    }*/
	//MicrofacetGeometricShadow(vNormal, vViewDir, fRoughness);
#define USE_PBR 0 
#if USE_PBR==1
    fSpecularTerm = ndfTerm * 0.25f;
#else
    fSpecularTerm = PhongSpecular(vNormal, vLightDir, -vViewDir, fRoughness) * 4.0f;
#endif
    //
    //ndfTerm * fresnelTerm;
}
void CalculateDiffuseTerm_ViewDependent(in float3 vNormal, in float3 vLightDir, in float3 vViewDir, out float fDiffuseTerm, in float fRoughness)
{
    float fLambert = LambertDiffuse(vNormal, vLightDir);
    float fFL = pow5(1 - saturate(dot(vNormal, vLightDir)));
    float fFV = pow5(1 - saturate(dot(vNormal, vViewDir)));
    float3 vHalfWay = normalize(vLightDir + vViewDir);
	
    float LdotH = saturate(dot(vHalfWay, vLightDir));

    float fd90 = 0.5 + 2 * LdotH * LdotH * fRoughness;
	// Two schlick fresnel term
    float lightScatter = (1 + (fd90 - 1) * fFL);
    float viewScatter = (1 + (fd90 - 1) * fFV);
    fDiffuseTerm = (lightScatter * viewScatter) * fLambert;
}

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
    float3 Normals = (input.Normal.xyz * 0.5f + 0.5f) * 2.0f - 1.0f;
    Normals.z = sqrt(1.01 - dot(Normals.xy, Normals.xy));
    Normals = normalize(Normals);
    float3 ViewDir = normalize(WorldPos.xyz - ViewPos);
    float3 LightDir = normalize(SunPosition.xyz);

    float DiffuseTerm, SpecularTerm;
    CalculateDiffuseTerm_ViewDependent(Normals.xyz, LightDir, ViewDir, DiffuseTerm, 1 - fGlossiness);
    CalculateSpecularTerm(Normals.xyz, LightDir, -ViewDir, 1 - fGlossiness,  SpecularTerm);
    // clamp to avoid unrealisticly high values
    SpecularTerm = min(SpecularTerm, 16.0f);
    DiffuseTerm *= DNBalance;

    float4 albedoSample = MaterialColor;
    if (HasTexture)
        albedoSample *= tex2D(Diffuse, input.Texcoord);
    
    float4 outColor;
    float ShadowTerm = DrawShadow(ShadowSampler, WorldPos,
    length(WorldPos.xyz - ViewPos), WorldPos, ShadowBuffer) * DNBalance;

    float3 ReflDir = normalize(reflect(ViewDir, normalize(Normals.xyz)));
    float3 FullScattering;

    float3 ObjectColor = CalculateFogColor(float3(0, 0, 0), ReflDir, LightDir, 1000, 0, FullScattering);
    float3 SkyColor = GetSkyColor(ReflDir, LightDir, FullScattering);

    float3 ReflectionFallBack = 0.0f;
    //ReflDir.x *= -1;
  
    //float4 CubeMap =
    //    txCubeMap.SampleLevel(samLinear, ReflDir, (1 - fGlossiness) * 9.0f);
    //ReflectionFallBack = lerp(CubeMap.rgb, SkyColor, 1 - CubeMap.a);
    
    float3 sun_lighting = DiffuseTerm * ShadowTerm * SunColor.rgb;
    float3 radiance = input.Color.rgb * saturate(1.0f - DNBalance + 0.2f);
    // todo: add lighting methods for forward renderer
    
    outColor.rgb = albedoSample.rgb * (sun_lighting + radiance + SkyLightColor.rgb * 0.3f) +
        SpecularTerm * fSpecularIntensity * DNBalance * SunColor.rgb * ShadowTerm + ReflectionFallBack * fSpecularIntensity;
   
    outColor.rgb = CalculateFogColor(outColor.rgb, ViewDir, LightDir, input.Depth, WorldPos.z, FullScattering);
    outColor.a = albedoSample.a * input.Color.a;
    return outColor;
}