#include "FillGBuffer.hlsl"

float FarClip : register(c0);
float4 MaterialColor : register(c1);
float4 MaterialProps : register(c2);

bool4 Info : register(b0);

#define HasTexture Info.x
#define ForceNormalMap Info.y
#define HasNormalMap Info.z

#define DNBalance MaterialProps.z

VS_DeferredOutput VS_Deferred(VS_Input input)
{
    VS_DeferredOutput output;
    VSFillGBuffer(input, output);
    
    return output;
}

sampler2D Diffuse : register(s0);
sampler2D NormalMap : register(s1);
sampler2D Reflection : register(s2);

PS_DeferredOutput PS_Deferred(VS_DeferredOutput input)
{
    float4 outColor;
    outColor.rgb = MaterialColor.rgb * input.Color.rgb;
    outColor.rgb += max(input.Color.rgb, 0.2) * lerp(0.5, 1, DNBalance);
    outColor.a = 1;
    
    if (HasTexture)
        outColor *= tex2D(Diffuse, input.Texcoord);
    
    float3 normal = input.Normal;
    if (HasNormalMap)
    {
        float3 normalMap = tex2D(NormalMap, input.Texcoord).rgb;
        normal = PeturbNormal(normalMap, input.WorldPosition.xyz, normal, input.Texcoord);
    }
    else if (ForceNormalMap)
    {
        float3x3 c = img3x3(Diffuse, input.Texcoord, 512, 0);
        float3 normalMap = height2normal_sobel(c);
        normalMap = normalize(float3(normalMap.xy, normalMap.z * MaterialProps.w));
        normal = PeturbNormal(normalMap, input.WorldPosition.xyz, normal, input.Texcoord);
    }
    
    PS_DeferredOutput output;
    PSFillGBuffer(outColor, input.Depth / FarClip, normal, float4(MaterialProps.x, MaterialProps.y, 0, 1), output);
    
    return output;
}