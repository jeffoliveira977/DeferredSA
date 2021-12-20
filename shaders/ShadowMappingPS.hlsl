
float4 Diffuse : register(c0);

sampler2D DiffuseSampler : register(s0);

void main(float2 Texcoord : TEXCOORD0, 
          float4 Color : TEXCOORD1, 
          float2 Depth : TEXCOORD2,
          out float4 color: COLOR)
{
    float a = tex2D(DiffuseSampler, Texcoord).a;
    clip(a - 0.5);
    
    color = Depth.x / Depth.y;


}