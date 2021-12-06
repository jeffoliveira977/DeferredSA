#include "Utilities.hlsl"
#include "Shadow.hlsl"

row_major float4x4 ViewInverseMatrix : register(c0);
row_major float4x4 ProjectionMatrix : register(c4);

float3 sunDir : register(c11);
float2 fogData : register(c12);

#define FogStart fogData.x
#define FarClip fogData.y

ShadowData ShadowBuffer : register(c13);
sampler2D ShadowSampler[4] : register(s6);

float4 main(float2 texCoord: TEXCOORD, float2 vpos: VPOS) : COLOR
{
    float depth;
    float3 normal;
    DecodeDepthNormal(texCoord, fogData.y, depth, normal);
   // normal = normalize(normal);

    float3 worldPosition;
    WorldPositionFromDepth(texCoord, depth, ProjectionMatrix, ViewInverseMatrix, worldPosition);

    ShadowData buf = ShadowBuffer;
    float cosTheta = clamp(dot(normalize(normal), normalize(sunDir)), 0, 1);
    float bias = ShadowBuffer.bias * tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
    bias = clamp(bias, 0, 0.01);
    //buf.bias = bias;
    const float3 ViewPos = ViewInverseMatrix[3].xyz;
    float Distance = length(worldPosition.xyz - ViewPos);
    float4 shadow = DrawShadow(ShadowSampler, float3(vpos, 1), Distance, worldPosition, buf);
  // float shadow = SampleShadowCascades(ShadowSampler[0], worldPosition, Distance, ShadowBuffer);
    
  //  FadeOutShadow(shadow.r, Distance, ShadowBuffer.params.y / 8);
    return shadow;
}