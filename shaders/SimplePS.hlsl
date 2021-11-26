float4 MaterialDiffuse : register(c0);
bool HasTexture : register(b0);

struct Input
{
    float2 Texcoord : TEXCOORD0;
    float4 Color    : COLOR0;
};

sampler2D Diffuse : register(s0);
float4 main(Input input) : COLOR0
{
    float4 outColor = MaterialDiffuse * input.Color;
    if (HasTexture)
        outColor *= tex2D(Diffuse, input.Texcoord);
    
    return outColor;
}