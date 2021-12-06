float4 MaterialDiffuse : register(c0);
float2 WaterLevel : register(c1);

struct Input
{
    float3 Texcoord : TEXCOORD0;
    float4 Color : COLOR0;
};

sampler2D Diffuse : register(s0);
float4 main(Input input) : COLOR0
{
    if ((input.Texcoord.z > WaterLevel.x) && WaterLevel.y == 1.0)
        return 0;
   
    float4 outColor = MaterialDiffuse * input.Color;
    outColor *= tex2D(Diffuse, input.Texcoord.xy);
    
    return outColor;
}