#include "Utilities.hlsl"
#include "Shadow.hlsl"
#include "PBR.hlsl"

#define PI 3.14
#define USE_ADDITIONALGBUFFER	
#define AMBIENT_TERM_TYPE 1

row_major float4x4 InverseViewMatrix : register(c0);
row_major float4x4 ProjectionMatrix : register(c4);
float2 FarClip : register(c8);

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
row_major float4x4 matProj : register(c14);
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

float chebyshevUpperBound(float2 sd, float d)
{
  //  // Surface is fully lit. as the current fragment is before the light occluder
  //  if (distance <= moments.x)
  //      return 1.0;
	
		//// The fragment is either in shadow or penumbra. We now use chebyshev's upperBound to check
		//// How likely this pixel is to be lit (p_max)
  //  float variance = moments.y - (moments.x * moments.x);
  //  variance = max(variance, 0.00002);
	
  //  float d = distance - moments.x;
  //  float p_max = variance / (variance + d * d);
  //  if (p_max > 0.0)
  //      p_max = 0.0;
  //  p_max = ((moments.x < 0.001f) ? 1.0f : p_max);
  //  return p_max;
    
    float mean = sd.x;
    float variance = max(sd.y - sd.x * sd.x, 0.0002f);

    float md = mean - d;
    float pmax = variance / (variance + md * md);

    float t = max(d <= mean, pmax);
    float s = ((sd.x < 0.001f) ? 1.0f : t);

    s = saturate(s);
    return s;
}

static vec3 sampleOffsetDirections[20] =
{
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
};   

float ComputeAttenuation(float3 lDir)
{
    return 1 - saturate(dot(lDir, lDir) * 1.0f / (LightRadius * LightRadius));
}

float ComputeShadowFactor(float3 lightPos, float3 viewDir, float3 position, float3 normal)
{
    vec3 fragToLight = position - lightPos;
    
    float shadow = 0.0;
    float bias = max(0.5f * (1.0f - dot(normal, fragToLight)), 0.0005f);

    int samples = 20;
    float viewDistance = length(position-viewDir);
    //float diskRadius = 0.05;
    float diskRadius = (1.0 + (-viewDistance / LightRadius)) / 25.0;
    float currentDepth = length(fragToLight);

    for (int i = 0; i < samples; ++i)
    {
        float closestDepth = texCUBE(SamplerShadow, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        if (closestDepth < 1)
        {
            closestDepth *= LightRadius; // undo mapping [0;1]
            if (currentDepth - bias > closestDepth)
                shadow += 0.5;
        }
    }
    shadow /= float(samples);  
    return shadow;
}


float4 main(float3 ViewRay : TEXCOORD2, float2 texCoord : TEXCOORD0, float3 frustumRay : TEXCOORD1, float face : VFACE) : COLOR
{
    float3 albedo = TEXTURE2D_ALBEDO(texCoord).rgb;
    float4 Parameters = TEXTURE2D_MATERIALPROPS(texCoord);
    float metallicness = Parameters.x;
    float Roughness = 1 - Parameters.y;

    float depth;
    float3 normal;
    DecodeDepthNormal(texCoord, FarClip.y, depth, normal);
    normal = mul(normal, (float3x3) InverseViewMatrix);

    float3 worldPosition;
    WorldPositionFromDepth(texCoord, depth, ProjectionMatrix, InverseViewMatrix, worldPosition);

    clip(depth + 0.9999f);
    
    float4 depthNormal = TEXTURE2D_DEPTHNORMAL(texCoord);
    depth = DecodeFloatRG(depthNormal.zw);

    worldPosition = InverseViewMatrix[3].xyz + depth * frustumRay;
    float3 ViewDir = normalize(worldPosition - InverseViewMatrix[3].xyz);

    float4 color = 0;
   
    float3 lightPos = normalize(worldPosition.xyz-LightPosition.xyz);
    
    
    float distance_to_light = distance(LightPosition.xyz, worldPosition.xyz);

    vec3 pl_dir = normalize(LightPosition.xyz - worldPosition.xyz);
    float ndotl = max(0, dot(normal.xyz, pl_dir));
    if (ndotl <= 0.0f)
        return 0.0;
    
    float dirLen = length(worldPosition-LightPosition);
    
    //float s2 = 0.2f;
    //float atten = 1.0f - smoothstep(LightRadius * s2, LightRadius, dirLen);
    //atten = 1.0f - pow(saturate(dirLen / LightRadius), 2);
       
   
    float Attenuation = 1.0f - pow(saturate(dirLen / LightRadius), 2);
    Attenuation *= Attenuation;
    

    float currentDepth = dirLen;
    
    float3 ldir = worldPosition.xyz - LightPosition.xyz;


    // shadow term
    float2 sd = texCUBE(SamplerShadow, normalize(ldir)).rg;
    
  //  sd = 1-sd;

    float shadow = 1;
    //if(sd.x < 1.0)
   // {
        shadow = chebyshevUpperBound(sd, dirLen);
        
        float bias = 0.05;
    float closestDepth = sd.x;
    closestDepth *= LightRadius;
        
    if (currentDepth - bias < closestDepth)
    {
       // shadow = 0.5;
            //return Attenuation;

    }
       // shadow = ComputeShadowFactor(LightPosition.xyz, InverseViewMatrix[3].xyz, worldPosition, normal);
    //float Distance = dirLen;

    //if (Distance < sd.x + bias)
    //        shadow = 0.5; // Inside the light
    //    else
    //        shadow = 0.5; // Inside the shadow
    
   // }
    
    float3 Diff, Spec;
    CalculateLighing(albedo, normal, -lightPos, -ViewDir, Roughness, metallicness, Diff, Spec);
    
    color.xyz = Attenuation * Diff * ( shadow);
    color.w = Attenuation * Spec * (shadow);
    
    return color;
    //float3 FinalDiffuseTerm = float3(0, 0, 0);
    //float FinalSpecularTerm = 0;
    //float DiffuseTerm, SpecularTerm;
    //CalculateDiffuseTerm_ViewDependent(normal, lightPos.xyz, ViewDir, DiffuseTerm, Roughness);
    //CalculateSpecularTerm(normal, lightPos.xyz, ViewDir, Roughness, SpecularTerm);
    //FinalDiffuseTerm += DiffuseTerm * Attenuation * shadow * LightColor * LightIntensity;
    //FinalSpecularTerm += SpecularTerm * Attenuation * shadow * metallicness;
    ////FinalDiffuseTerm += SpecularTerm * s * Attn * metallicness;
    //float4 Lighting = float4(FinalDiffuseTerm, FinalSpecularTerm);
    //color.xyzw = Lighting;
    //return color;
}