
float4 Diffuse : register(c0);

sampler2D DiffuseSampler : register(s0);

float3 lightPos : register(c1);
float FarClip : register(c2);

void main(float2 Texcoord : TEXCOORD0, 
          float4 Color : TEXCOORD1, 
          float2 Depth : TEXCOORD2,
          out float4 color: COLOR)
{
    float a = tex2D(DiffuseSampler, Texcoord).a;
    clip(a - 0.5);
    
    float d = Depth.x / Depth.y;
    color = d;
}