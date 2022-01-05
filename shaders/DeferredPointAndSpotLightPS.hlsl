#include "Utilities.hlsl"
#include "Shadow.hlsl"
#include "PBR.hlsl"

#define PI 3.14
#define USE_ADDITIONALGBUFFER	
#define AMBIENT_TERM_TYPE 1

row_major float4x4 InverseViewMatrix : register(c0);
row_major float4x4 ProjectionMatrix : register(c4);
float2 FarClip: register(c8);

struct LightData{
    float4 position;  // Position xyz, Range w
    float4 direction; // Direction xyz, Light type w
    float4 color;
};

LightData lightData : register(c9);


float3 LightPosition : register(c9);
float3 LightDirection:  register(c10);
float3 LightColor : register(c11);
float LightRadius : register(c12);
float LightIntensity : register(c13);
float LightConeAngle : register(c14);
float CastShadow : register(c15);
row_major float4x4 ShadowMatrix : register(c16);

sampler2D SamplerShadow : register(s5);
sampler2D SamplerNoise : register(s6);

float CalculateAttenuation(float Range, float dis, float d)
{
    return 1.0f - smoothstep(Range * dis, Range, d);
}

float CalculateSpotCone(float3 lightDir, float3 lightPos, float spotAngle)
{
    float minCos = cos(radians(spotAngle));

    float maxCos = lerp(minCos, 1, 0.9f);
    float cosAngle = dot(lightDir, -lightPos);

    return smoothstep(minCos, maxCos, cosAngle);
}

float ComputeAttenuation(float3 lDir, float len)
{
    return 1 - saturate(dot(lDir, lDir) * len);
}

float SpotLightIntensity(float cosA, float umbraAngle, float penumbraAngle)
{
    float t = saturate((cosA - cos(umbraAngle)) / (cos(penumbraAngle) - cos(umbraAngle)));
    return smoothstep(0, 1, t);
}

static float2 pcfOffsets[8] =
{
    { -1, -1 },
    { -1, 1 },
    { 1, -1 },
    { 1, 1 },
    { -1, 0 },
    { 1, 0 },
    { 0, 1 },
    { 0, -1 }
};


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


float3 CalculateLighing(float3 albedo, float3 normal, float3 lightPosition, float3 viewDir, float roughness, float specularIntensity)
{
    // Outgoing light direction (vector from world-space fragment position to the "eye").
    float3 Lo = viewDir;

	// Get current fragment's normal and transform to world space.
    float3 N = normal;
	
	// Angle between surface normal and outgoing light direction.
    float cosLo = max(0.0, dot(N, Lo));
		
	// Specular reflection vector.
    float3 Lr = 2.0 * cosLo * N - Lo;
    
    float metalness = 0.1;
    const float3 Fdielectric = 0.04;
	// Fresnel reflectance at normal incidence (for metals use albedo color).
    float3 F0 = lerp(Fdielectric, albedo, metalness);

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
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);

		// Lambert diffuse BRDF.
		// We don't scale by 1/PI for lighting & material units to be more convenient.
		// See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
    float3 diffuseBRDF = kd  * LightColor * LightIntensity;

		// Cook-Torrance specular microfacet BRDF.
    float3 specularBRDF = (F * D * G) / max(0.00001, 4.0 * cosLi * cosLo) * specularIntensity;
   
    return (diffuseBRDF + specularBRDF) * cosLi;
}



float4 main(float4 position: TEXCOORD3, float2 texCoord : TEXCOORD0, float3 frustumRay : TEXCOORD1, float2 vpos:VPOS) : COLOR
{
    float4 Parameters = TEXTURE2D_MATERIALPROPS(texCoord);
    float SpecIntensity = Parameters.x;
    float Roughness = 1 - Parameters.y;
    

    float depth;
    float3 normal;
    DecodeDepthNormal(texCoord, FarClip.y, depth, normal);
    normal = mul(normal, (float3x3) InverseViewMatrix);
    
    float3 worldPosition;
    WorldPositionFromDepth(texCoord, depth, ProjectionMatrix, InverseViewMatrix, worldPosition);
    
    
    float4 depthNormal = TEXTURE2D_DEPTHNORMAL(texCoord);
    depth = DecodeFloatRG(depthNormal.zw);

    worldPosition = InverseViewMatrix[3].xyz + depth * frustumRay;
    float3 ViewDir = normalize(worldPosition.xyz - InverseViewMatrix[3].xyz);

    float4 color = 0;
   
    float3 lightPos = normalize(worldPosition.xyz - LightPosition.xyz);
    float dirLen = length(worldPosition - LightPosition);
    
    float s2 = 1.1f;
    float atten = 1.0f - smoothstep(LightRadius * s2, LightRadius, dirLen);
    atten = 1.0f - pow(saturate(dirLen / LightRadius), 2);
    
    
    float Attenuation = 1.0f - pow(saturate(dirLen / LightRadius), 2);
    Attenuation *= Attenuation;
   // float LightDistance = length(worldPosition.xyz - LightPosition.xyz) / 2.0;
   // atten = 1.0f - pow(saturate(LightDistance / LightRadius), 2);
    float minCos = cos(radians(LightConeAngle));
    float maxCos = lerp(minCos, 1, 0.7f);
    float cosAngle = dot(LightDirection, lightPos);
    float distance = length(worldPosition - LightPosition);
    

   // Attenuation *= smoothstep(minCos, maxCos, cosAngle);
    
    vec2 fragCoord = vpos.xy / 512.0f;
    //Attenuation *= length(vec3(tex2D(SamplerNoise, texCoord).xyz));
    //fragCoord = CalculateScreenPosition(float4(frustumRay, 1.0));
    
 
    
    float fSpot = SpotLightIntensity(max(dot(lightPos, LightDirection), 0.0f), radians(LightConeAngle), radians(10)); //pow(, 4.0f);
    Attenuation *= fSpot;
    //atten *= 0.5;
    float texelSize = 1.0 / 512.0f;
   
    float4 LightViewPos = mul(float4(worldPosition, 1.0), ShadowMatrix);
    
    float4 projTexC = LightViewPos;
  // Complete projection by doing division by w.
    projTexC.xyz /= projTexC.w;
	
	//// Points outside the light volume are in shadow.
 //   if (projTexC.x < -1.0f || projTexC.x > 1.0f ||
	//    projTexC.y < -1.0f || projTexC.y > 1.0f ||
	//    projTexC.z < 0.0f)
 //       return 0.0f;
	    
	// Transform from NDC space to texture space.
    projTexC.x = +0.5f * projTexC.x + 0.5f;
    projTexC.y = -0.5f * projTexC.y + 0.5f;
    float4 flashlight = tex2D(SamplerNoise, projTexC.xy);
    //Attenuation *= length(flashlight);
    float bias = max((1.0 - dot(normal, -lightPos)), 0.0001);
    
    float z = projTexC.z /*- 0.0001f*/;
    float s = ComputeShadowSamples(SamplerShadow, 512.0f, projTexC.xy, z);
    if (CastShadow == 0.0)
        s = 1.0;
    
    float3 FinalDiffuseTerm = float3(0, 0, 0);
    float FinalSpecularTerm = 0;
    float DiffuseTerm, SpecularTerm;
    CalculateDiffuseTerm_ViewDependent(normal, -lightPos.xyz, -ViewDir, DiffuseTerm, Roughness);
    CalculateSpecularTerm(normal, -lightPos.xyz, -ViewDir, Roughness, SpecularTerm);
   // float3 albedo = TEXTURE2D_ALBEDO(texCoord).rgb;

   // color.xyz = atten * s * CalculateLighing(albedo, normal, lightPos, -ViewDir, Roughness, SpecIntensity);

    FinalDiffuseTerm += DiffuseTerm /** flashlight.xyz*/* Attenuation * s * LightColor * LightIntensity;
    FinalSpecularTerm += SpecularTerm * Attenuation *s* SpecIntensity;
    float4 Lighting = float4(FinalDiffuseTerm, FinalSpecularTerm);
    color.xyzw = Lighting;
    return color;
}