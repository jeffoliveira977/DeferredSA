#include "FillGBuffer.hlsl"

float3 SkyTopColor : register(c0);
float4 HorizonColor : register(c1);
float4 MaterialColor : register(c2);
float4 MaterialProps : register(c3);

bool4 Info : register(b0);

#define FarClip HorizonColor.w
#define HasTexture Info.x
#define ForceNormalMap Info.y
#define HasNormalMap Info.z

#define Metallicness MaterialProps.x
#define Glossiness MaterialProps.y
#define DNBalance MaterialProps.z

sampler2D Diffuse : register(s0);
sampler2D NormalMap : register(s1);
PS_DeferredOutput main(VS_DeferredOutput input)
{
    float3 normal = input.Normal;
    float4 outColor = MaterialColor;
    
    if (HasTexture)
        outColor *= tex2D(Diffuse, input.Texcoord);

    // Transform the surface normal into world space (in order to compute reflection
    float4 radiance = dot(MaterialColor, MaterialColor) > 4.0 ? MaterialColor.rgba : float4(lerp(SkyTopColor.rgb, HorizonColor.rgb, normal.z) * 0.5f, 1);
    
    PS_DeferredOutput output;
    PSFillGBuffer(outColor, input.Depth / FarClip, normal, float4(Metallicness, Glossiness, 0, 2), 0, output);
    
    return output;
}