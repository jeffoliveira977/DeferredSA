#include "Utilities.hlsl"
#include "Shadow.hlsl"

row_major float4x4 ViewInverseMatrix : register(c0);
row_major float4x4 ProjectionMatrix : register(c4);


float4 main() : SV_TARGET
{
    float depth;
    float3 normal;
    DecodeDepthNormal(texCoord, fogData.y, depth, normal);
    normal = normalize(normal);

    float3 worldPosition;
    WorldPositionFromDepth(texCoord, depth, ProjectionMatrix, ViewInverseMatrix, worldPosition);
    
    float3 lightDir = normalize(sunDir);
    float3 viewDir = normalize(ViewInverseMatrix[3].xyz - worldPosition);
    float3 H = normalize(viewDir + lightDir);
    
    float3 ViewDir = normalize(worldPosition.xyz - viewDir); // View direction vector
    const float3 ViewPos = ViewInverseMatrix[3].xyz;
    float Distance = length(worldPosition.xyz - ViewPos);

    float3 shadow = DrawShadow(ShadowSampler, Distance, worldPosition, ShadowBuffer);
}