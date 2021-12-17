row_major float4x4 World : register(c0);
float2 FogData : register(c12);
float3 CameraPosition : register(c13);

#define NearPlane FogData.x
#define FarPlane  FogData.y

struct VS
{
    float4 Position : POSITION0;
    float4 Color    : COLOR0;
    float2 Texcoord : TEXCOORD0;
};

VS main(VS input)
{
    VS output;

    float4 position = mul(input.Position, World);
    float3 view = normalize(position.xyz - CameraPosition.xyz);
    output.Position.xy = view.xy * ((view.z + 1.0f) / 2.0f);
    float L = length(position.xyz - CameraPosition.xyz);
    output.Position.z = L / FarPlane;
    output.Position.w = 1.0;

    output.Texcoord = input.Texcoord;
    output.Color = input.Color;
    
    return output;
}
