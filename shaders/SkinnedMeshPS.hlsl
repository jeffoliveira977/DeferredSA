#include "FillGBuffer.hlsl"


float3 SkyTopColor : register(c0);
float4 HorizonColor : register(c1);
float4 MaterialColor : register(c2);
float4 MaterialProps : register(c3);

#define FarClip HorizonColor.w

bool4 Info : register(b0);

#define HasTexture Info.x
#define ForceNormalMap Info.y
#define HasNormalMap Info.z

#define DNBalance MaterialProps.z
sampler2D Diffuse : register(s0);
sampler2D NormalMap : register(s1);

PS_DeferredOutput main(VS_DeferredOutput input)
{
    float3 normal = input.Normal.xyz;
    float4 outColor = MaterialColor;
    
    if (HasTexture)
        outColor *= tex2D(Diffuse, input.Texcoord);
    
    if (HasNormalMap)
    {
        if (length(input.Tangent.xyz) > 0)
        { // normal = PeturbNormal(normalMap, input.WorldPosition.xyz, normal, input.Texcoord);
            //float3x3 tbn =
            //float3x3(normalize(input.Binormal.xyz),
            //          normalize(cross(normalize(normal),
            //          normalize(input.Tangent.xyz))),
            //          normalize(normal));

            //normal = tex2D(NormalMap, input.Texcoord).rgb;
            //normal = normalize(normal * 2.0 - 1.0);
            //normal = normalize(mul(normal, tbn));
            normal = NormalMapToSpaceNormal(tex2D(NormalMap, input.Texcoord).rgb, normal, input.Binormal, input.Tangent);
        }
    }
    else if (1)
    {
        float3x3 c = img3x3(Diffuse, input.Texcoord, 512, 0);
        float3 normalMap = height2normal_sobel(c);
        normalMap = normalize(float3(normalMap.xy, normalMap.z * MaterialProps.w));
        float3x3 tbn = float3x3(normalize(input.Tangent), normalize(input.Binormal), normalize(normal));
        normal = normalize(mul(normalMap, tbn));
    }
    float4 radiance = float4(lerp(SkyTopColor.rgb, HorizonColor.rgb, normal.z) * 0.25f, 1);
    
    PS_DeferredOutput output;
    PSFillGBuffer(outColor, input.Depth / FarClip, normal, float4(MaterialProps.x, MaterialProps.y, 0, 3), radiance, output);
    
    return output;
}