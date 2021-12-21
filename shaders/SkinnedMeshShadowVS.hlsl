row_major float4x4 World : register(c0);
row_major float4x4 View : register(c4);
row_major float4x4 Projection : register(c8);

float4x3 Bones[64] : register(c12);

float4x3 calculateSkinnedBones(uint4 indices, float4 weight)
{
    float4x3 skinTransform = Bones[indices.x] * weight.x +
                             Bones[indices.y] * weight.y +
                             Bones[indices.z] * weight.z +
                             Bones[indices.w] * weight.w;
    return skinTransform;
}

struct VS_input
{
    float4 Position : POSITION0;
    float2 Texcoord : TEXCOORD0;
};

struct VS_output
{
    float4 Position : POSITION0;
    float2 Texcoord : TEXCOORD0;
    float2 Depth : TEXCOORD1;
};

VS_output main(VS_input input, float4 weight : BLENDWEIGHT,
                               uint4 indices : BLENDINDICES)
{
    float4x3 bones = calculateSkinnedBones(indices, weight);

    VS_output output;
    output.Position = float4(mul(input.Position, bones).xyz, 1.0);
    output.Position = mul(output.Position, World);
    float4 viewPosition = mul(output.Position, View);
    output.Position = mul(viewPosition, Projection);
    
    output.Texcoord = input.Texcoord;
    output.Depth = output.Position.zw;
    return output;
}
