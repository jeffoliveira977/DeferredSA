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
row_major float4x4 ViewProjectionMatrix : register(c40);
sampler2D ShadowSampler[4] : register(s5);

static float LightIntensity = 1.0;

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
float3 fresnelSchlick(float3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 CalculateLighing(float3 albedo, float3 normal, float3 lightPosition, float3 viewDir, float roughness, float metallicness)
{
    // Outgoing light direction (vector from world-space fragment position to the "eye").
    float3 Lo = viewDir;

	// Get current fragment's normal and transform to world space.
    float3 N = normal;
	
	// Angle between surface normal and outgoing light direction.
    float cosLo = max(0.0, dot(N, Lo));
		
	// Specular reflection vector.
    float3 Lr = 2.0 * cosLo * N - Lo;
    
    const float3 Fdielectric = 0.04;
	// Fresnel reflectance at normal incidence (for metals use albedo color).
    float3 F0 = lerp(Fdielectric, albedo, metallicness);

    // Half-vector between Li and Lo.
    float3 Lh = normalize(lightPosition + Lo);

	// Calculate angles between surface normal and various light vectors.
    float cosLi = max(0.0, dot(N, lightPosition));
    float cosLh = max(0.0, dot(N, Lh));

	// Calculate Fresnel term for direct lighting. 
    float3 F = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
    
	// Calculate normal distribution for specular BRDF.
    float D = ndfGGX(cosLh, roughness);
    
	// Calculate geometric attenuation for specular BRDF.
    float G = gaSchlickGGX(cosLi, cosLo, roughness);

	// Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
	// Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
	// To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metallicness);

	// Lambert diffuse BRDF.
	// We don't scale by 1/PI for lighting & material units to be more convenient.
	// See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
    float3 diffuseBRDF = kd * sunColor * HorizonColor * LightIntensity;

	// Cook-Torrance specular microfacet BRDF.
    float3 specularBRDF = (F * D * G) / max(0.00001, 4.0 * cosLi * cosLo) * roughness;
   
    return (diffuseBRDF + specularBRDF) * cosLi;
}

float4 main(float2 texCoord : TEXCOORD0, float2 vpos:VPOS) : COLOR
{
    float4 AlbedoColor = TEXTURE2D_ALBEDO(texCoord);

    clip(length(AlbedoColor) <= 0);

   // float4 Shadow = tex2D(ShadowSampler, texCoord);

    float4 Parameters = TEXTURE2D_MATERIALPROPS(texCoord);
    float SpecIntensity = Parameters.x;
    float Roughness = 1 - Parameters.y;
    
    float4 color;
    
    float depth;
    float3 normal;
    DecodeDepthNormal(texCoord, fogData.y, depth, normal);
   

   // normal = normalize(normal);
    normal = mul(normal, (float3x3) ViewInverseMatrix);
        
    float3 worldPosition;
    WorldPositionFromDepth(texCoord, depth, ProjectionMatrix, ViewInverseMatrix, worldPosition);
    float4 Shadow = DrawShadow(ShadowSampler, worldPosition, length(worldPosition.xyz - ViewInverseMatrix[3].xyz), worldPosition, ShadowBuffer) * ShadowDNBalance;
  
    float FarClip2 = 160.0;
    float FogStart2 = 0.0;
    float fogdist;
    fogdist = worldPosition.z;
    float fadefact = (FarClip2 - depth) / (FarClip2 - FogStart2);
    fadefact = saturate(1.0 - fadefact);
    
  //  Shadow = lerp(Shadow, 1.0, fadefact); 
 //   Shadow = lerp(1.0, Shadow, 0.7); 
    float3 lightDir = normalize(sunDir);
    float3 ViewDir = normalize(worldPosition.xyz - ViewInverseMatrix[3].xyz); // View direction vector

    float DiffuseTerm, SpecularTerm;
    CalculateDiffuseTerm_ViewDependent(normal, lightDir.xyz, -ViewDir, DiffuseTerm, Roughness);
    CalculateSpecularTerm(normal, lightDir.xyz, -ViewDir, Roughness, SpecularTerm);
    
    float3 Radiance = tex2D(RadianceSampler, texCoord);
    
    float2 Lighting = float2(DiffuseTerm, SpecularTerm * SpecIntensity * ShadowDNBalance) * (ShadowDNBalance <= 0.0 ? 1.0 : Shadow);
    color = float4(Lighting.x * sunColor.rgb + Radiance + 0.4 * (1 - ShadowDNBalance), Lighting.y);
    
   // color.w = 0.0;
    //color.xyz = CalculateLighing(AlbedoColor.rgb, normal, lightDir, -ViewDir, Roughness, SpecIntensity) /** ShadowDNBalance * (ShadowDNBalance <= 0.0 ? 1.0 : Shadow)*/;
    //color.xyz +=  Radiance * saturate(1.0f - ShadowDNBalance + 0.2f);
    return color;
}