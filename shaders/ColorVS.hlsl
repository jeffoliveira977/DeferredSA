row_major float4x4 World : register(c0);
row_major float4x4 View : register(c4);
row_major float4x4 Projection : register(c8);

struct VS
{
    float4 Position : POSITION0;
    float4 Color : COLOR0;
};

VS main(VS input)
{
    VS output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    output.Color = input.Color;
    return output;
}