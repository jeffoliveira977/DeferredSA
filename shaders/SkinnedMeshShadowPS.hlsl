float fAlpha : register(c0);

float3 lightPos : register(c1);
float FarClip : register(c2);


struct PS_input
{
    float2 Texcoord : TEXCOORD0;
    float2 Depth : DEPTH;
};

sampler2D Diffuse : register(s0);

float4 main(PS_input input) : COLOR
{

   float d = input.Depth.x / input.Depth.y;

    return d;
}