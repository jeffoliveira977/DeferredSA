#include "FillGBuffer.hlsl"

float4x3 Bones[64] : register(c12);

float4x3 calculateSkinnedBones(uint4 indices, float4 weight)
{
    float4x3 skinTransform = Bones[indices.x] * weight.x +
                             Bones[indices.y] * weight.y +
                             Bones[indices.z] * weight.z +
                             Bones[indices.w] * weight.w;
    return skinTransform;
}

VS_DeferredOutput main(VS_Input input,
                              float4 weight : BLENDWEIGHT,
                              uint4 indices : BLENDINDICES)
{
    float4x3 bones = calculateSkinnedBones(indices, weight);

    input.Position = float4(mul(input.Position, bones).xyz, 1.0);
    input.Normal = mul(input.Normal, (float3x3) bones);
    input.Binormal = mul(input.Binormal, (float3x3) bones);
    input.Tangent = mul(input.Tangent, (float3x3) bones);
    
    VS_DeferredOutput output;
    VSFillGBuffer(input, output);
    
    return output;
}