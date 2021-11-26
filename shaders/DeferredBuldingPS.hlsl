#include "FillGBuffer.hlsl"

float FarClip : register(c0);
float4 MaterialColor : register(c1);
float4 MaterialProps : register(c2);

bool4 Info : register(b0);

#define HasTexture Info.x
#define ForceNormalMap Info.y
#define HasNormalMap Info.z
#define HasSpecularMap Info.w

#define Metallicness MaterialProps.x
#define Glossiness MaterialProps.y
#define DNBalance MaterialProps.z

sampler2D Diffuse : register(s0);
sampler2D SpecularMap : register(s1);
sampler2D NormalMap : register(s2);
sampler2D Reflection : register(s3);

PS_DeferredOutput main(VS_DeferredOutput input)
{
    float4 outColor;
   // outColor.rgb = MaterialColor.rgb * input.Color.rgb;
    //outColor.rgb += max(input.Color.rgb, 0.2) * lerp(0.5, 1, DNBalance);
    //outColor.a = 1;
   // outColor = input.Color * lerp(0.25f, 1.0f, 1 - DNBalance);
    outColor = MaterialColor;
    float _Bias = 0.5;
    if (HasTexture)
        outColor *= tex2D(Diffuse, input.Texcoord);
    
    float3 params;
    float3 normal = input.Normal;
    
    if (HasSpecularMap)
    {
        params = tex2D(SpecularMap, input.Texcoord).xyz;
    }
    else
    {
        params = float3(Metallicness, Glossiness, 0);
    }
    if (HasNormalMap)
    {
        if ( length(input.Tangent.xyz) > 0)
        { // normal = PeturbNormal(normalMap, input.WorldPosition.xyz, normal, input.Texcoord);
            float3x3 tbn =
            float3x3(normalize(input.Binormal.xyz),
                      normalize(cross(normalize(normal),
                      normalize(input.Tangent.xyz))),
                      normalize(normal));

            normal = tex2D(NormalMap, input.Texcoord).rgb;
            normal = normalize(normal * 2.0 - 1.0);
            normal = normalize(mul(normal, tbn));
        }
    }
    else if (ForceNormalMap)
    {
        float3x3 c = img3x3(Diffuse, input.Texcoord, 512, 0);
        float3 normalMap = height2normal_sobel(c);
        normalMap = normalize(float3(normalMap.xy, normalMap.z * MaterialProps.w));
        normal = PeturbNormal(normalMap, input.WorldPosition.xyz, normal, input.Texcoord);
    }
    
    PS_DeferredOutput output;
    PSFillGBuffer(outColor, input.Depth / FarClip, normal, float4(params, 1), input.Color * lerp(0.25f, 1.0f, 1 - DNBalance), output);
    
    return output;
}