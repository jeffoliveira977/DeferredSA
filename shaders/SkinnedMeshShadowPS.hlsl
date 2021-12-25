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
    float3 Depth : TEXCOORD1;
};

sampler2D Diffuse : register(s0);

float4 main(PS_input input) : COLOR
{
   // return input.Depth.x;
    
    //float4 color;
   //float d= input.Depth.x / input.Depth.y;
   // color.y = input.Depth * input.Depth;
   // color.z = 0.0;
   // color.w = 1.0;
   // return color;
    
    float d = length(input.Depth - lightPos.xyz);
   //// d.y *= -1.0;
   //// return length(Depth - lightPos);
   // float dx = ddx(d);
   // float dy = ddy(d);
   //  float bias = max(abs(dx), abs(dy)) ;
   // return d + bias;
   // return length(d) /*/ FarClip*/;
    
   // float2 m = ComputeMoments(d);
    return float4(d, d * d, 0, 1);
}