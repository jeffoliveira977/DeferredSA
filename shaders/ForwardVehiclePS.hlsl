#include "Utilities.hlsl"

float3 SkyTop : register(c8);
float3 SkyBottom : register(c9);
float3 SunColor : register(c10);
float3 SunPosition : register(c11);
float2 FogData : register(c12);
float3 CameraPosition : register(c13);
float4 MaterialColor : register(c14);
float4 MaterialProps : register(c15);

bool HasTexture : register(b0);

#define fSpecularFactor MaterialProps.x
#define fReflectionFactor MaterialProps.y
#define DNBalance MaterialProps.z

#define FogStart FogData.x
#define FarClip FogData.y

struct VS_Output
{
    float4 Position      : POSITION0;
    float2 Texcoord      : TEXCOORD0;
    float3 WorldNormal   : TEXCOORD1;
    float3 WorldPosition : TEXCOORD2;
    float Depth          : DEPTH;
};

sampler2D Diffuse : register(s0);
sampler2D Paraboloid[2] : register(s1);
samplerCUBE CubeMap : register(s3);

float4 main(VS_Output input) : COLOR
{
    float3 normal = normalize(input.WorldNormal);
    normal.xy = BiasD(normal);
    normal = TwoChannelNormalX2(normal.xy);
    
    float specularFactor = MaterialProps.x;
    float shininessFactor = MaterialProps.y;
    
    float4 outColor = MaterialColor;
    if (HasTexture)
        outColor *= tex2D(Diffuse, input.Texcoord);
    
    float3 lightDir = normalize(SunPosition);
    float3 viewDir = normalize(input.WorldPosition.xyz - CameraPosition);

    float3 R = reflect(viewDir, normal);
    R.xy = normalize(R.xy);
    R.xy = R.xy * (R.z + 1.0f) / 2.0f;
    R.xy = (R.xy + 1.0f) / 2.0f;
    R.y = 1 - R.y;
    float4 reflection = tex2D(Paraboloid[0], R.xy);
          
    float f = clamp(1.0 - dot(normal, -viewDir), 0.0, 1.0);
    float p = pow(2.0, shininessFactor * 20);
    f = pow(f, p);
    reflection.rgb = lerp(reflection.rgb, outColor.rgb, 1 - reflection.a);
    outColor.rgb = lerp(outColor.rgb, reflection.rgb, f * specularFactor);
    
    float3 d = lerp(SkyTop, SkyBottom, saturate(normal.z)) * 1.34;

    outColor.rgb *= LambertLighting(lightDir, normal, d * SunColor * 0.5) * specularFactor;
    float specular = PhongSpecular(lightDir, viewDir, normal, shininessFactor) * specularFactor;
    outColor.rgb += specular;
    
    float3 skyColor = lerp(SkyTop, SkyBottom, saturate((input.WorldPosition.z - 20) * (1.0f / 500.0f)));
    float fHeightMultiplier = saturate((input.WorldPosition.z - 20) * (1.0f / 500.0f));
    float fFogCoef = saturate(max(input.Depth - FogStart, 0) / max(FarClip - FogStart, 0.001f)) * (1 - fHeightMultiplier);
    outColor.rgb = lerp(outColor.rgb, skyColor, fFogCoef);
    
    return outColor;
}