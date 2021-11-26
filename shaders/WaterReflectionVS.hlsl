row_major float4x4 World : register(c0);
row_major float4x4 View : register(c4);
row_major float4x4 Projection : register(c8);
float2 WaterLevel : register(c12);

struct VS_Input
{
    float4 Position : POSITION0;
    float2 Texcoord : TEXCOORD0;
    float4 Color : COLOR0;
};

struct VS_Output
{
    float4 Position : POSITION0;
    float3 Texcoord : TEXCOORD0;
    float4 Color : COLOR0;
};

VS_Output main(VS_Input input)
{
    VS_Output output;
    
     // Calculate screen pos of vertex
    float4 world = mul(input.Position, World);
    
   // if ( WaterLevel.y == 1.0)
   // {
        world.z = -world.z;
        world.z += 2 * WaterLevel;
    //}
    
    output.Position = mul(world, View);
    output.Position = mul(output.Position, Projection);
    
    output.Texcoord = float3(input.Texcoord.xy, world.z);
    output.Color = input.Color;
    return output;
}