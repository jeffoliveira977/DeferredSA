float4 MaterialDiffuse : register(c0);
bool HasTexture : register(b0);

struct VS_DualOutput
{
    float4 Position : POSITION0;
    float4 Color    : COLOR0;
    float2 Texcoord : TEXCOORD0;
    float DualZ     : TEXCOORD1;
};

sampler2D Diffuse : register(s0);
float4 main(VS_DualOutput input) : COLOR0
{
    clip(input.DualZ);
    
    float4 outColor = MaterialDiffuse * input.Color;
    if (HasTexture)
        outColor *= tex2D(Diffuse, input.Texcoord);
    
    return outColor;
}