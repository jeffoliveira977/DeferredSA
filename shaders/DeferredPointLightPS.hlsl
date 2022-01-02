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
float CastShadow : register(c14);
row_major float4x4 matProj[6] : register(c15);
samplerCUBE SamplerShadow : register(s5);
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
    float bias = 0.05;

    int samples = 20;
    float viewDistance = length(position - viewDir);
    //float diskRadius = 0.05;
    float diskRadius = (1.0 + (-viewDistance / LightRadius)) / 25.0;
    float currentDepth = length(fragToLight);

    for (int i = 0; i < samples; ++i)
    {
        float closestDepth = 1-texCUBE(SamplerShadow, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        if (closestDepth < 1)
        {
            closestDepth *= LightRadius; // undo mapping [0;1]
            if (currentDepth-bias  > closestDepth)
                shadow += 1;
        }
    }
    shadow /= float(samples);
    return 1-shadow;
}

float2 NormalToUvFace(float3 v, out int faceIndex)
{
    float3 vAbs = abs(v);
    float ma;
    float2 uv;
    if (vAbs.z >= vAbs.x && vAbs.z >= vAbs.y)
    {
        faceIndex = v.z < 0.0 ? 5 : 4; //FACE_FRONT : FACE_BACK;   // z major axis...  we designate negative z forward.
        ma = 0.5f / vAbs.z;
        uv = float2(v.z < 0.0f ? -v.x : v.x, -v.y);
    }
    else if (vAbs.y >= vAbs.x)
    {
        faceIndex = v.y < 0.0f ? 3 : 2; //FACE_BOTTOM : FACE_TOP;  // y major axis.
        ma = 0.5f / vAbs.y;
        uv = float2(v.x, v.y < 0.0 ? -v.z : v.z);
    }
    else
    {
        faceIndex = v.x < 0.0 ? 1 : 0; //FACE_LEFT : FACE_RIGHT; // x major axis.
        ma = 0.5f / vAbs.x;
        uv = float2(v.x < 0.0 ? v.z : -v.z, -v.y);
    }
    return uv * ma + float2(0.5f, 0.5f);
}


static const float3 FaceDirectons[6] =
{
    float3(1, 0, 0),
    float3(-1, 0, 0),
    float3(0, 1, 0),
    float3(0, -1, 0),
    float3(0, 0, 1),
    float3(0, 0, -1),
};

static const float2 irreg_kernel[8] =
{
    { -0.072, -0.516 },
    { -0.105, 0.989 },
    { 0.949, 0.258 },
    { -0.966, 0.216 },
    { 0.784, -0.601 },
    { 0.139, 0.230 },
    { -0.816, -0.516 },
    { 0.529, 0.779 }
};
float rand2dTo1d(float2 value, float2 dotDir = float2(12.9898, 78.233))
{
    float2 smallValue = sin(value);
    float random = dot(smallValue, dotDir);
    random = frac(sin(random) * 143758.5453);
    return random;
}
float3 rand2dTo3d(float2 value)
{
    return float3(
		rand2dTo1d(value, float2(12.989, 78.233)),
		rand2dTo1d(value, float2(39.346, 11.135)),
		rand2dTo1d(value, float2(73.156, 52.235))
	);
}
float2 rand2dTo2d(float2 value)
{
    return float2(
		rand2dTo1d(value, float2(12.989, 78.233)),
		rand2dTo1d(value, float2(39.346, 11.135))
	);
}

float4 main(float3 ViewRay : TEXCOORD2, float2 texCoord : TEXCOORD0, float3 frustumRay : TEXCOORD1, float2 vpos : VPOS) : COLOR
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

    //clip(depth + 0.9999f);
    
    float4 depthNormal = TEXTURE2D_DEPTHNORMAL(texCoord);
    depth = DecodeFloatRG(depthNormal.zw);

    worldPosition = InverseViewMatrix[3].xyz + depth * frustumRay;
    float3 ViewDir = normalize(worldPosition - InverseViewMatrix[3].xyz);

    float4 color = 0;
   
    float3 lightPos = normalize(worldPosition.xyz - LightPosition.xyz);
    
    
    float distance_to_light = distance(LightPosition.xyz, worldPosition.xyz);

    vec3 pl_dir = normalize(LightPosition.xyz - worldPosition.xyz);
    float ndotl = max(0, dot(normal.xyz, lightPos));
    //if (ndotl <= 0.0f)
    //    return 0.0;
    
    float dirLen = length(worldPosition - LightPosition);
    
    //float s2 = 0.2f;
    //float atten = 1.0f - smoothstep(LightRadius * s2, LightRadius, dirLen);
    //atten = 1.0f - pow(saturate(dirLen / LightRadius), 2);
       
   
    float Attenuation = 1.0f - pow(saturate(dirLen / LightRadius), 2);
    Attenuation *= Attenuation;
    

    float currentDepth = dirLen;
    
    float3 ldir = worldPosition.xyz - LightPosition.xyz;


    // shadow term
   

    float shadow = 0.0;
    
    [branch]
    if (dirLen > LightRadius)
    {
        clip(-1);
        return 0;
    }
    
   
    float3 directionToFragment = normalize(worldPosition - LightPosition.xyz);
    float closestDirection = -1;
    int faceIndex = 0;
    float4 lightingPosition[6];
    for (int face = 0; face < 6; ++face)
    {
        float3 forward = FaceDirectons[face];
        float result = dot(directionToFragment, forward);
        if (result > closestDirection)
        {
            closestDirection = result;
            faceIndex = face;
            lightingPosition[face] = mul(float4(worldPosition, 1), matProj[face]);
        }
    }
      
       // float4 LightPosition = lightingPosition[faceIndex];
    float2 shadowTexCoord = NormalToUvFace(directionToFragment, faceIndex);
    if (shadowTexCoord.x < 0 || shadowTexCoord.x > 1 || shadowTexCoord.y < 0 || shadowTexCoord.y > 1)
        return 1;
        
    float2 offset = float2((float) faceIndex / 6.0f, 0.0f);
    shadowTexCoord.x *= 1.0f / 6.0f;
    shadowTexCoord += offset;
        //float2 shadowTexCoord = mad(0.5, LightPosition.xy / LightPosition.w, float2(0.5, 0.5));
        //shadowTexCoord.y = 1.0f - shadowTexCoord.y;
    
      
    shadow = ComputeShadowFactor(LightPosition.xyz, InverseViewMatrix[3].xyz, worldPosition, normal);
       // float sd = 1-texCUBE(SamplerShadow, normalize(ldir)).r;
    
     
       //// float sd = 1 - tex2D(SamplerShadow, shadowTexCoord).r;
       // float bias = 0.05;
       // float closestDepth = sd;
       // closestDepth *= LightRadius;
       // if (currentDepth - bias < closestDepth)
       // {
       //     shadow = 1.0; // Avoid lights turn off

       // }
       //// shadow = Compute4Samples(SamplerShadow, shadowTexCoord, float2(512.0f * 6, 512.0f), currentDepth - bias, LightRadius);

       // float s = 0, t;

       // float2 stex = vpos.xy / 16.0f;
       // float2 noise;
       // float2 rotated;

       // noise = tex2D(SamplerNoise, stex);
       // noise = normalize(rand2dTo2d(vpos)*2-1);

       // float2x2 rotmat = { noise.x, -noise.y, noise.y, noise.x };
       // shadow = 0;
       // float2 texels = (1.0f / 6.0f/ (512.0*6), 1.0/ 512.0f);
       // float v = 0.6;
       // for (int i = 0; i < 8; ++i)
       // {
       //     rotated = irreg_kernel[i];
       //     rotated = mul(rotated, rotmat) * 0.8f;
    
       //     //shadowTexCoord += offset;
       //     sd = 1 - tex2D(SamplerShadow, shadowTexCoord + rotated * texels).r;
       //      closestDepth = sd;
       //     closestDepth *= LightRadius;
       //     t = (currentDepth - bias < closestDepth);
       //     shadow += t;

       // }

       // shadow = saturate(shadow * 0.125f); // simulate ambient light here

        
        
    //if (CastShadow == 0.0)
        shadow = 1.0;
      
    

    //float3 Diff, Spec;
    //CalculateLighing(albedo, normal, -lightPos, -ViewDir, Roughness, metallicness, Diff, Spec);
    
    //color.xyz = Attenuation * Diff * (1 - shadow);
    //color.w = Attenuation * Spec * (1 - shadow);
    

    
    //return color;
    float3 FinalDiffuseTerm = float3(0, 0, 0);
    float FinalSpecularTerm = 0;
    float DiffuseTerm, SpecularTerm;
    CalculateDiffuseTerm_ViewDependent(normal, -lightPos.xyz, -ViewDir, DiffuseTerm, Roughness);
    CalculateSpecularTerm(normal, -lightPos.xyz, -ViewDir, Roughness, SpecularTerm);
    FinalDiffuseTerm += DiffuseTerm * Attenuation * shadow * LightColor * LightIntensity;
    FinalSpecularTerm += SpecularTerm * Attenuation * shadow * metallicness;
    //FinalDiffuseTerm += SpecularTerm * s * Attn * metallicness;
    float4 Lighting = float4(FinalDiffuseTerm, FinalSpecularTerm);
    
        
    color.xyzw = Lighting;

 
    return color;
}