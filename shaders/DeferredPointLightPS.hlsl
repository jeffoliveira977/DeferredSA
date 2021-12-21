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
float4x4 ShadowMatrix : register(c14);
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
    
    float s2 = 0.2f;
    float atten = 1.0f - smoothstep(LightRadius * s2, LightRadius, dirLen);
    atten = 1.0f - pow(saturate(dirLen / LightRadius), 2);
       
    float4 wpos = mul(worldPosition, ShadowMatrix);
    //wpos /= wpos.w;
    
    float3 ldir = LightPosition.xyz - worldPosition.xyz;;
    ldir.x *= -1.0;
    // shadow term
    float2 sd = texCUBE(SamplerShadow, -ldir).rg;
    float d = length(ldir);

    float mean = sd.x;
    mean *= FarClip;
    float variance = max(sd.y - sd.x * sd.x, 0.0002f);

    float md = mean - d;
    float pmax = variance / (variance + md * md);

    float t = max(d <= mean, pmax);
    float s = ((sd.x < 0.01f) ? 1.0f : t);

    s = saturate(s);
    
    float currentDepth = length(LightPosition.xyz - worldPosition.xyz);
    // test for shadows
    float bias = 0.00005; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth - bias > mean ? 1.0 : 0.0;

    float3 FinalDiffuseTerm = float3(0, 0, 0);
    float FinalSpecularTerm = 0;
    float DiffuseTerm, SpecularTerm;
    CalculateDiffuseTerm_ViewDependent(normal, lightPos.xyz, -ViewDir, DiffuseTerm, Roughness);
    CalculateSpecularTerm(normal, lightPos.xyz, -ViewDir, Roughness, SpecularTerm);
    FinalDiffuseTerm += DiffuseTerm * atten * shadow * LightColor * LightIntensity;
  //  FinalDiffuseTerm = lerp(FinalDiffuseTerm, refl, fresnel);
    FinalSpecularTerm += SpecularTerm * atten * SpecIntensity;
    float4 Lighting = float4(FinalDiffuseTerm, FinalSpecularTerm);
    color.xyzw = Lighting;
    return color;
}