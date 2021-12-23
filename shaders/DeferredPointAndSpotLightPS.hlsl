#include "Utilities.hlsl"
#include "Shadow.hlsl"
#include "PBR.hlsl"

#define PI 3.14
#define USE_ADDITIONALGBUFFER	
#define AMBIENT_TERM_TYPE 1

row_major float4x4 InverseViewMatrix : register(c0);
row_major float4x4 ProjectionMatrix : register(c4);
float FarClip: register(c8);

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

row_major float4x4 ShadowMatrix : register(c15);

sampler2D SamplerShadow : register(s5);
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



float4 main(float2 texCoord : TEXCOORD0) : COLOR
{
    float4 Parameters = TEXTURE2D_MATERIALPROPS(texCoord);
    float SpecIntensity = Parameters.x;
    float Roughness = 1 - Parameters.y;
    

    float depth;
    float3 normal;
    DecodeDepthNormal(texCoord, FarClip, depth, normal);
   // normal = mul(normal, (float3x3) InverseViewMatrix);
    
    float3 worldPosition;
    WorldPositionFromDepth(texCoord, depth, ProjectionMatrix, InverseViewMatrix, worldPosition);
    float3 ViewDir = normalize(worldPosition.xyz - InverseViewMatrix[3].xyz);

    float4 color = 0;
   
    float3 lightPos = -normalize(worldPosition.xyz - LightPosition.xyz);
    float dirLen = length(worldPosition - LightPosition);
    
    float s2 = 1.1f;
    float atten = 1.0f - smoothstep(LightRadius * s2, LightRadius, dirLen);
    atten = 1.0f - pow(saturate(dirLen / LightRadius), 2);
   // float LightDistance = length(worldPosition.xyz - LightPosition.xyz) / 2.0;
   // atten = 1.0f - pow(saturate(LightDistance / LightRadius), 2);
    float minCos = cos(radians(LightConeAngle));
    float maxCos = lerp(minCos, 1, 0.9f);
    float cosAngle = dot(LightDirection, -lightPos);
    float distance = length(worldPosition - LightPosition);
    
    float attenuation = pow(clamp(1 - pow((distance / LightRadius), 4.0), 0.0, 1.0), 2.0) / (1.0 + (distance * distance));
    
    atten *= smoothstep(minCos, maxCos, cosAngle);
    
    float fSpot = SpotLightIntensity(max(dot(-lightPos, LightDirection), 0.0f), radians(60), radians(30)); //pow(, 4.0f);
    //atten *= fSpot;
    //atten *= 0.5;
    
    float4 LightViewPos = mul(float4(worldPosition, 1.0), ShadowMatrix);
    
    float2 ShadowTexC = 0.5 * LightViewPos.xy / LightViewPos.w + float2(0.5, 0.5);
    ShadowTexC.y = 1.0f - ShadowTexC.y;


    float texelSize = 1.0 / 512.0f;
    float z = LightViewPos.z;
    //float shadow = z < tex2D(SamplerShadow, ShadowTexCoord);
    float shadow = (tex2D(SamplerShadow, ShadowTexC) + 0.0001f < LightViewPos.z / LightViewPos.w) ? 0.0f : 1.0f;
    float t, s = 0;
		
    for (int i = 0; i < 8; ++i)
    {
        float4 sd = tex2D(SamplerShadow, ShadowTexC + pcfOffsets[i] * texelSize).r + 0.0001f;

        t = (LightViewPos.z / LightViewPos.w <= sd ? 1 : 0);
        s += (sd == 0 ? 1 : t);
    }
    s *= 0.125f;
    s = saturate(s + 0.2f);
    float3 FinalDiffuseTerm = float3(0, 0, 0);
    float FinalSpecularTerm = 0;
    float DiffuseTerm, SpecularTerm;
    CalculateDiffuseTerm_ViewDependent(normal, lightPos.xyz, -ViewDir, DiffuseTerm, Roughness);
    CalculateSpecularTerm(normal, lightPos.xyz, -ViewDir, Roughness, SpecularTerm);
    float3 albedo = TEXTURE2D_ALBEDO(texCoord).rgb;

    color.xyz = atten * s * CalculateLighing(albedo, normal, lightPos, -ViewDir, Roughness, SpecIntensity);

    FinalDiffuseTerm += DiffuseTerm * atten * s * LightColor * LightIntensity;
   
    FinalSpecularTerm += SpecularTerm * atten * s * SpecIntensity;
    float4 Lighting = float4(FinalDiffuseTerm, FinalSpecularTerm);
  //  color.xyzw = Lighting;
    return color;
}