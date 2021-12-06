
float4 Diffuse : register(c0);

sampler2D DiffuseSampler : register(s0);

void main(float2 Texcoord : TEXCOORD0, 
          float4 Color : TEXCOORD1, 
          float Depth : DEPTH, 
          out float4 color: COLOR)
{
    float a = tex2D(DiffuseSampler, Texcoord).a;
    clip(a - 0.5);
    
    //color = Depth;

    color.x = Depth;
    color.y = Depth * Depth;
    color.z = 0.0;
    color.w = 1.0;

}