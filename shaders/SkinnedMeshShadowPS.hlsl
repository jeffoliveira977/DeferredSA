float fAlpha : register(c0);

float3 lightPos : register(c1);
float FarClip : register(c2);


struct PS_input
{
    float2 Texcoord : TEXCOORD0;
    float2 Depth : TEXCOORD1;
};

sampler2D Diffuse : register(s0);

float4 main(PS_input input) : COLOR
{
   // return input.Depth.x;
    
    //float4 color;
    return input.Depth.x / input.Depth.y;
   // color.y = input.Depth * input.Depth;
   // color.z = 0.0;
   // color.w = 1.0;
   // return color;
    
   // return length(Depth - lightPos);
   // float d = length(lightPos.xyz - input.Depth)/FarClip;
  //  return  float4(d, d * d, 0, 1);
}