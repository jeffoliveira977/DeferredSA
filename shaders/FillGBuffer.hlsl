#include "Utilities.hlsl"

row_major float4x4 World : register(c0);
row_major float4x4 View : register(c4);
row_major float4x4 Projection : register(c8);

struct VS_Input
{
    float4 Position : POSITION0;
    float2 Texcoord : TEXCOORD0;
    float4 Color : COLOR0;
    float3 Normal : NORMAL;
    float3 Binormal : BINORMAL;
    float3 Tangent : TANGENT;
};

struct VS_DeferredOutput
{
    float4 Position : POSITION0;
    float2 Texcoord : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float3 Normal : TEXCOORD2;
    float3 Binormal : TEXCOORD3;
    float3 Tangent : TEXCOORD4;
    float4 WorldPosition : TEXCOORD5;
    float4 PositionVS : TEXCOORD6;
    float Depth : DEPTH;
};

struct PS_DeferredOutput
{
    float4 Albedo : COLOR0;
    float4 NormalDepth : COLOR1;
    float4 Glow : COLOR2;
    float4 Radiance : COLOR3;
};

void VSFillGBuffer(VS_Input input, out VS_DeferredOutput output)
{
    //float4x4 inverseWorld = inverseMatrix(World);
   float4x4 modelview= mul(World, View);
    output.Position = mul(input.Position, World);
    float4 worldView = mul(output.Position, View);
    output.Position = mul(worldView, Projection);
    output.Texcoord = input.Texcoord;
    output.Color = input.Color;
    output.Normal = normalize(mul(input.Normal, (float3x3) modelview));
    output.Tangent = normalize(mul(input.Tangent, (float3x3) modelview));
    output.Binormal = normalize(cross(output.Normal, output.Tangent));
    output.WorldPosition = mul(input.Position, World);
    output.PositionVS = worldView;
    output.Depth = worldView.z;
}
/*!
    Encodes material type to float in 0..1 range.
*/
inline float ConvertFromMatType(float p)
{
    return p / 255.0f;
}

void PSFillGBuffer(float4 albedo, float depth, float3 normal, float4 glow, float4 radiance, out PS_DeferredOutput output)
{
    output.Albedo = albedo;
    //normal = mul(normalize(normal), (float3x3) View);
    output.NormalDepth = EncodeDepthNormal(depth, normal);
    output.Glow = glow;
    output.Glow.z = depth;
    output.Radiance = radiance;
    output.Radiance.w = (radiance.w);

}