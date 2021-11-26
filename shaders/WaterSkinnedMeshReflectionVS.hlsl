row_major float4x4 World : register(c0);
row_major float4x4 View : register(c4);
row_major float4x4 Projection : register(c8);

float2 WaterLevel : register(c12);
float4x3 Bones[64] : register(c13);

float4x3 calculateSkinnedBones(uint4 indices, float4 weight)
{
    float4x3 skinTransform = Bones[indices.x] * weight.x +
                             Bones[indices.y] * weight.y +
                             Bones[indices.z] * weight.z +
                             Bones[indices.w] * weight.w;
    return skinTransform;
}

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

VS_Output main(VS_Input input, float4 weight : BLENDWEIGHT,
                               uint4 indices : BLENDINDICES)
{
    VS_Output output;
    
    float4x3 bones = calculateSkinnedBones(indices, weight);
    output.Position = float4(mul(input.Position, bones).xyz, 1.0);
    
     // Calculate screen pos of vertex
    float4 world = mul(output.Position, World);
    
    if ( WaterLevel.y == 1.0)
    {
        world.z = -world.z;
        world.z += 2 * WaterLevel;
    }
    
    output.Position = mul(world, View);
    output.Position = mul(output.Position, Projection);
    
    output.Texcoord = float3(input.Texcoord.xy, world.z);
    output.Color = input.Color;
    return output;
}