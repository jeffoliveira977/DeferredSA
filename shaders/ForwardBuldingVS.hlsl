#include "Utilities.hlsl"

row_major float4x4 World : register(c0);
row_major float4x4 View : register(c4);
row_major float4x4 Projection : register(c8);

struct VS_Input
{
    float4 Position : POSITION0;
    float2 Texcoord : TEXCOORD0;
	float4 Color    : COLOR0;
	float3 Normal   : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_Output
{
    float4 Position      : POSITION0;
	float2 Texcoord      : TEXCOORD0;
	float3 Normal        : TEXCOORD1;
    float3 Binormal      : TEXCOORD2;
    float3 Tangent       : TEXCOORD3;
    float3 WorldPosition : TEXCOORD4;
	float4 Color         : TEXCOORD5;
    float Depth          : DEPTH;
};

VS_Output main(VS_Input input)
{
    VS_Output output;
    
    output.Position = mul(input.Position, World);
    float4 worldView = mul(output.Position, View);
    output.Position = mul(worldView, Projection);
    
    output.Texcoord = input.Texcoord;
    float4x4 modelview = mul(World, View);
    output.Normal = normalize(mul(input.Normal, (float3x3) modelview));
    output.Tangent = normalize(mul(input.Tangent, (float3x3) modelview));
    output.Binormal = normalize(cross(output.Normal, output.Tangent));
    output.WorldPosition = mul(input.Position, World).xyz;
    output.Depth = worldView.z;
    output.Color = input.Color;

    return output;
}