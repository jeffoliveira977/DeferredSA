#include "Utilities.hlsl"
#include "Shadow.hlsl"
#include "PBR.hlsl"

#define PI 3.14
#define USE_ADDITIONALGBUFFER	
#define AMBIENT_TERM_TYPE 1

row_major float4x4 ViewInverseMatrix : register(c0);
row_major float4x4 ProjectionMatrix : register(c4);
row_major float4x4 BasisMatrix : register(c8);

float3 HorizonColor : register(c12);
float3 SkyLightColor : register(c13);
float3 SunColor : register(c14);
float3 SunDir : register(c15);
float4 FogData : register(c16);

#define FogStart FogData.x
#define FarClip FogData.y

samplerCUBE CubeMapSampler : register(s4);
sampler2D SphericMapSampler : register(s5);
sampler2D ParaboloidSampler[2] : register(s6);

float4 ParaboloidEnvMap(float3 normal, float3 viewDir)
{
    float3 R = reflect(viewDir, normal);
    R = mul(float4(R, 0), BasisMatrix);
    
    float2 front;
    front.x = (R.x / (2 * (1 + R.z))) + 0.5;
    front.y = 1 - ((R.y / (2 * (1 + R.z))) + 0.5);

    float2 back;
    back.x = (R.x / (2 * (1 - R.z))) + 0.5;
    back.y = 1 - ((R.y / (2 * (1 - R.z))) + 0.5);
     
    float4 forward = tex2D(ParaboloidSampler[0], front.xy);
    float4 backward = tex2D(ParaboloidSampler[1], back.xy);
    return max(forward, backward);
}

float4 SphereEnvMap(float3 normal, float3 viewDir)
{
    float3 R = reflect(viewDir, normal);
    R.xy = normalize(R.xy);
    
    R.xy = R.xy * (R.z + 1.0f) / 2.0f;
    R.xy = (R.xy + 1.0f) / 2.0f;
    R.y = 1 - R.y;
    
    return tex2D(SphericMapSampler, R.xy);
}

float4 main(float2 texCoord : TEXCOORD0, float2 vpos:VPOS) : COLOR
{
    float3 outColor;
    float4 target = tex2D(SamplerLightTarget, texCoord);
    float4 albedo = TEXTURE2D_ALBEDO(texCoord);
  
    float4 Parameters = TEXTURE2D_MATERIALPROPS(texCoord);
    float specularFactor = Parameters.x;
    float Roughness = 1 - Parameters.y;
    float materialType = Parameters.w;

    float depth;
    float3 normal;
    float3 worldPosition;
    DecodeDepthNormal(texCoord, FogData.y, depth, normal);
    //normal = normalize(normal);
   // normal = mul(normal, (float3x3) ViewInverseMatrix);
    WorldPositionFromDepth(texCoord, depth, ProjectionMatrix, ViewInverseMatrix, worldPosition);

    float3 viewDir = normalize(worldPosition - ViewInverseMatrix[3].xyz);
    float H = max(dot(viewDir, SunDir), 0.0);

    float3 diffuse = target.rgb;
    float3 specular = target.a * target.rgb;
    
    outColor = (albedo.rgb * diffuse) + specular;

    float3 skyColor = lerp(HorizonColor, SkyLightColor, saturate((worldPosition.z - 20) * (1.0f / 500.0f)));
    float3 DiffuseTerm = (target.xyz + SkyLightColor.rgb * 0.3f);
    float FresnelCoeff = MicrofacetFresnel(normal, -viewDir, specularFactor);
    float4 R = 0.0;
    
    if (albedo.a <= 0)
    {
        outColor = float3(0, 0, 0);
    }
    else
    {
        if (materialType == 1) // Buldigns
        {
        }
        else if (materialType == 2) // Vehicles
        {
            int reflectionType = 1;
            if (reflectionType == 1)
            {
                float3 r = reflect(viewDir, normal);
                r.x *= -1.0;
                R = texCUBE(CubeMapSampler, r);
                R *= Roughness * 9.0f;

            }
            else if (reflectionType == 2)
            {
                R = SphereEnvMap(normal, viewDir);
            }
            else if (reflectionType == 3)
            {
                R = ParaboloidEnvMap(normal, viewDir);
            }

            R *= 1.5f;


        }
        else if (materialType == 3) // Skins
        {
        }

        outColor = DiffuseTerm * albedo.rgb + specular * Parameters.x + R.rgb * FresnelCoeff * Parameters.x;
        return float4(outColor, 1);
    }

    return float4(outColor, 0);
}


