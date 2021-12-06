float4 MaterialDiffuse : register(c0);
float WaterLevel : register(c1);

struct Input
{
    float3 Texcoord : TEXCOORD0;
    float4 Color : COLOR0;
};

sampler2D Diffuse : register(s0);
float4 main(Input input) : COLOR0
{
    if (input.Texcoord.z > WaterLevel)
        discard;
   
    float4 color = input.Color ? input.Color : 1.0;
    float4 outColor = MaterialDiffuse * color;
    outColor *= tex2D(Diffuse, input.Texcoord.xy);
    
    return outColor;
}