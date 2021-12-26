float fAlpha : register(c0);

float3 lightPos : register(c1);
float FarClip : register(c2);

float2 ComputeMoments(float Depth)
{
    float2 Moments;
    
    // First moment is the depth itself.   
    Moments.x = Depth;
    
    // Compute partial derivatives of depth.    
    float dx = ddx(Depth);
    float dy = ddy(Depth);
    
    // Compute second moment over the pixel extents.  
    Moments.y = Depth * Depth + 0.25 * (dx * dx + dy * dy);
    return Moments;
}

struct PS_input
{
    float2 Texcoord : TEXCOORD0;
    float3 World : TEXCOORD1;
    float2 Depth : DEPTH;
};

sampler2D Diffuse : register(s0);

float4 main(PS_input input) : COLOR
{

   // float4 color;
   //float d = input.Depth.x / input.Depth.y;

   // color.y = input.Depth * input.Depth;
    //color.z = 0.0;
   // color.w = 1.0;
  //  return color;
    
    float3 p = input.World - lightPos.xyz;
    p.y *= -1.0;
    float d = 1 - (length(p) / FarClip);
    return float4(d, d*d, 0, 1);
}