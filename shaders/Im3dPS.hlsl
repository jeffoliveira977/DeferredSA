float Params : register(c0);

struct VS_in
{
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR0;
};

struct VS_out
{
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR0;
};

sampler Sampler : register(s0);

float4 main(in VS_in input) : COLOR
{
    VS_out output;
    float4 color = input.Color;
    if (Params)
        color *= tex2D(Sampler, input.TexCoord);
    
    return color;
}