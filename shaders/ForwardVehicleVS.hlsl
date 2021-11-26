#include "Utilities.hlsl"

row_major float4x4 World : register(c0);
row_major float4x4 View : register(c4);
row_major float4x4 Projection : register(c8);

struct VS_Input
{
    float4 Position : POSITION0;
    float2 Texcoord : TEXCOORD0;
    float3 Normal   : NORMAL;
};

struct VS_Output
{
    float4 Position      : POSITION0;
    float2 Texcoord      : TEXCOORD0;
    float3 WorldNormal   : TEXCOORD1;
    float3 WorldPosition : TEXCOORD2;
    float Depth          : DEPTH;
};

VS_Output main(VS_Input input)
{
    VS_Output output;
    output.Position = mul(input.Position, World);
    float4 worldView = mul(output.Position, View);
    output.Position = mul(worldView, Projection);
    
    output.Texcoord = input.Texcoord;
	
    output.WorldNormal = mul(input.Normal, (float3x3) World);
    output.WorldPosition = mul(input.Position, World);
    output.Depth = output.Position.z;
    return output;
}