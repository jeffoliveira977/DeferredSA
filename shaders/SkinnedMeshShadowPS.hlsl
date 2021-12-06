float fAlpha : register(c0);

struct PS_input
{
    float2 Texcoord : TEXCOORD0;
    float Depth : DEPTH;
};

sampler2D Diffuse : register(s0);

float4 main(PS_input input) : COLOR
{
   // return input.Depth.x;
    
    float4 color;
    color.x = input.Depth;
    color.y = input.Depth * input.Depth;
    color.z = 0.0;
    color.w = 1.0;
    return color;
}