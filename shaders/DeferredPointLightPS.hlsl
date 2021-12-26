#include "Utilities.hlsl"
#include "Shadow.hlsl"
#include "PBR.hlsl"

#define PI 3.14
#define USE_ADDITIONALGBUFFER	
#define AMBIENT_TERM_TYPE 1

row_major float4x4 InverseViewMatrix : register(c0);
row_major float4x4 ProjectionMatrix : register(c4);
float FarClip : register(c8);

struct LightData
{
    float4 position; // Position xyz, Range w
    float4 direction; // Direction xyz, Light type w
    float4 color;
};

LightData lightData : register(c9);

float3 LightPosition : register(c9);
float3 LightDirection : register(c10);
float3 LightColor : register(c11);
float LightRadius : register(c12);
float LightIntensity : register(c13);
float3 frustumCorners[4] : register(c14);
row_major float4x4 matProj : register(c18);
samplerCUBE SamplerShadow : register(s5);

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

float attenuate(float3 ldir, float r)
{
    ldir /= r;

    float atten = dot(ldir, ldir);
    float att_s = 15;

    atten = 1.0f / (atten * att_s + 1);
    att_s = 1.0f / (att_s + 1);

    atten = atten - att_s;
    atten /= (1.0f - att_s);

    return saturate(atten);
}


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


void CalculateLighing(float3 albedo, float3 normal, float3 lightPosition, float3 viewDir, float roughness, float metallicness, out float3 diffuse, out float3 specular)
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
    float3 diffuseBRDF = kd * LightColor * LightIntensity;

	// Cook-Torrance specular microfacet BRDF.
    float3 specularBRDF = (F * D * G) / max(0.00001, 4.0 * cosLi * cosLo) * metallicness;
   
    diffuse = diffuseBRDF * cosLi;
    specular = specularBRDF * cosLi;
    //return (diffuseBRDF + specularBRDF) * cosLi;
}

float chebyshevUpperBound(float2 moments, float distance)
{
    // Surface is fully lit. as the current fragment is before the light occluder
    if (distance <= moments.x)
        return 1.0;
	
		// The fragment is either in shadow or penumbra. We now use chebyshev's upperBound to check
		// How likely this pixel is to be lit (p_max)
    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, 0.00002);
	
    float d = distance - moments.x;
    float p_max = variance / (variance + d * d);
    if(p_max > 0.0)
        p_max = 0.0;
    p_max = ((moments.x < 0.001f) ? 1.0f : p_max);
    return p_max;
}

float lengthSquared(float3 v1)
{
    return v1.x * v1.x + v1.y * v1.y + v1.z * v1.z;
}

float4 main(float2 texCoord : TEXCOORD0, float2 vpos : VPOS) : COLOR
{
    float3 albedo = TEXTURE2D_ALBEDO(texCoord).rgb;
    float4 Parameters = TEXTURE2D_MATERIALPROPS(texCoord);
    float metallicness = Parameters.x;
    float Roughness = 1 - Parameters.y;
    
    float2 g_fInverseViewportDimensions = {1/ 1920.0f, 1/1080.0f };
    texCoord = vpos * g_fInverseViewportDimensions.xy + g_fInverseViewportDimensions.xy * 0.5;
    float depth;
    float3 normal;
    DecodeDepthNormal(texCoord, FarClip, depth, normal);
   // normal = mul(normal, (float3x3) InverseViewMatrix);
    
    float3 worldPosition;
    WorldPositionFromDepth(texCoord, depth, ProjectionMatrix, InverseViewMatrix, worldPosition);
   // worldPosition = WSPositionFromDepth(texCoord,depth,  matProj);
    
    float3 ViewDir = normalize(worldPosition.xyz - InverseViewMatrix[3].xyz);

    float4 color = 0;
   

    //if (Parameters.w != 3)
     //   worldPosition.xyz = wpos.xyz;
    
    float3 lightPos = normalize(worldPosition.xyz-LightPosition.xyz);
    
    
    
    float dirLen = length(worldPosition-LightPosition);
    
    float s2 = 0.2f;
    float atten = 1.0f - smoothstep(LightRadius * s2, LightRadius, dirLen);
    atten = 1.0f - pow(saturate(dirLen / LightRadius), 2);
       

    float3 ldir = worldPosition.xyz - LightPosition.xyz;
    ldir.y*= -1.0;

    // shadow term
    float2 sd = texCUBE(SamplerShadow, normalize(ldir)).rg;
    float s = chebyshevUpperBound(sd, length(ldir));
    
    float currentDepth = dirLen;
    
    float shadow = 1;
    // test for shadows
    float bias = 0.05;
    float closestDepth = sd.x;
    closestDepth *= LightRadius;
    shadow = currentDepth - bias > closestDepth ? 1.0 : 0;

    
    float Attenuation = 1.0f - pow(saturate(dirLen / LightRadius), 2);
    Attenuation *= Attenuation;
    
    float3 Diff, Spec;
    CalculateLighing(albedo, normal, -lightPos, -ViewDir, Roughness, metallicness, Diff, Spec);
    
    // Total contribution for this light.
    
    float DistanceSq = lengthSquared(worldPosition.xyz - LightPosition.xyz);
    float radius = LightRadius;
    //[branch]
   // if (dirLen < abs(radius * radius))
    //{
        float Distance = sqrt(dirLen);


        float du = Distance / (1 - dirLen / (radius * radius - 1));

        float denom = du / abs(radius) + 1;

            //The attenuation is the falloff of the light depending on distance basically
      //  Attenuation = 1 / (denom * denom);
    
        color.xyz = (1 - Attenuation) * (shadow) * Diff;
        color.w = (1 - Attenuation) * (shadow) * Spec;
   // }
    
    return color;
    float3 FinalDiffuseTerm = float3(0, 0, 0);
    float FinalSpecularTerm = 0;
    float DiffuseTerm, SpecularTerm;
    CalculateDiffuseTerm_ViewDependent(normal, lightPos.xyz, ViewDir, DiffuseTerm, Roughness);
    CalculateSpecularTerm(normal, lightPos.xyz, ViewDir, Roughness, SpecularTerm);
    FinalDiffuseTerm += DiffuseTerm * Attenuation * shadow * LightColor * LightIntensity;
    FinalSpecularTerm += SpecularTerm * Attenuation * shadow * metallicness;
    //FinalDiffuseTerm += SpecularTerm * s * Attn * metallicness;
    float4 Lighting = float4(FinalDiffuseTerm, FinalSpecularTerm);
    color.xyzw = Lighting;
    return color;
}