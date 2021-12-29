
float4 Diffuse : register(c0);

sampler2D DiffuseSampler : register(s0);

float3 lightPos : register(c1);
float FarClip : register(c2);


float4 mapDepthToARGB32(const float value)
{
    const float4 bitSh = float4(256.0 * 256.0 * 256.0, 256.0 * 256.0, 256.0, 1.0);
    const float4 mask = float4(0.0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0);
    float4 res = frac(value * bitSh);
    res -= res.xxyz * mask;
    return res;
}

void main(float2 Texcoord : TEXCOORD0, 
          float4 Color : TEXCOORD1, 
          float3 Depth : TEXCOORD2,
          out float4 color: COLOR)
{
    float a = tex2D(DiffuseSampler, Texcoord).a;
    clip(a - 0.5);
    
    //float d = Depth.x / Depth.y;
    //color = depth;

    
    float3 p = Depth - lightPos.xyz;
   // p.y *= -1.0;
    float d = 1-(length(p)/FarClip);
    
    color = float4(d, d * d, 0, 1);
}