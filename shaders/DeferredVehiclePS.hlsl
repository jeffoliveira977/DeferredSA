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

    //if (HasNormalMap)
    //{
    //    float3 normalMap = tex2D(NormalMap, input.Texcoord).rgb;
    //    normal = PeturbNormal(normalMap, input.WorldPosition.xyz, normal, input.Texcoord);
    //}
    //else if (ForceNormalMap)
    //{
    //    float3x3 c = img3x3(Diffuse, input.Texcoord, 512, 0);
    //    float3 normalMap = height2normal_sobel(c);
    //    normalMap = normalize(float3(normalMap.xy, normalMap.z * MaterialProps.w));
    //    normal = PeturbNormal(normalMap, input.WorldPosition.xyz, normal, input.Texcoord);
    //}
    
    float4 radiance = dot(MaterialColor, MaterialColor) > 4.0 ? MaterialColor.rgba : float4(lerp(SkyTopColor.rgb, HorizonColor.rgb, normal.z) * 0.5f, 1);
    
    PS_DeferredOutput output;
    PSFillGBuffer(outColor, input.Depth / FarClip, normal, float4(Metallicness, Glossiness, 0, 2), radiance, output);
    
    return output;
}