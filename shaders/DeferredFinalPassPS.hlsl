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
sampler2D SSAOSampler: register(s8);

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
//Variables
static float SunSize = { 0.1 };
static float SunStrength = { 0.21 };
static float SunSpread = { 11.33 };
static float SunSpreadSmoothness = { 0.0 };
static float SunSetHeight = { 0.14 };
static float SunSetSpread = { 1.0 };
static float SunSetSpreadSmoothness = { 0.37 };

static float SkyTransitionAng = 0.17;
static float SkySmoothness = 0.32;

static float3 S_D = { 1, 1, 1 };
static float3 S_S = { 1, 0.482, 0.325 };
static float3 ST_D = { 0.22, 0.431, 0.643 };
static float3 ST_N = { 0.0275, 0.0863, 0.145 };
static float3 ST_S = { 0.0353, 0.408, 0.671 };
static float3 SB_D = { 0.82, 0.937, 1 };
static float3 SB_N = { 0.212, 0.153, 0.133 };
static float3 SB_S = { 0.541, 0.459, 0.4 };

//just a simple one
float4 sky(const in float3 cam_dir, float moonLight, float tf)
{
    
    float
		noon_index = smoothstep(-0.1, 0.25, tf),
		sunset_index = smoothstep(0.4, 0.15, tf) * smoothstep(-0.26, -0.05, tf),
		suncross = dot(SunColor.xyz, cam_dir);

    float3
        top = lerp(lerp(ST_N, ST_D, noon_index), ST_S, sunset_index),
        bottom = lerp(lerp(SB_N, SB_D, noon_index), SB_S, sunset_index),
        sun = lerp(S_S, S_D, noon_index),
        c = lerp(bottom, top, smoothstep(SkyTransitionAng - SkySmoothness, SkyTransitionAng + SkySmoothness, cam_dir.z));

    if (-SunSize / 1000.0 < suncross - 1)
        c = sun * suncross * SunStrength * 3.0;
    else
    {
        c += cam_dir.z * sun * exp((suncross - 1.0) / SunSpread) * SunStrength / 2.0;
        c += sunset_index * sun * exp((suncross - 1.0) / SunSetSpread) * SunStrength * smoothstep(SunSetHeight + SunSetSpreadSmoothness, SunSetHeight - SunSetSpreadSmoothness, cam_dir.z) * smoothstep(-0.2, 0.1, cam_dir.z);
    }
    c += sun * exp((suncross - 1.0) / SunSpread * 500.0) * SunStrength;
    
    return float4(c, 1.0);
}
inline float clampMap(float x, float a, float b, float c, float d)
{
    return saturate((x - b) / (a - b)) * (c - d) + d;
}
float rand2dTo1d(float2 value, float2 dotDir = float2(12.9898, 78.233))
{
    float2 smallValue = sin(value);
    float random = dot(smallValue, dotDir);
    random = frac(sin(random) * 143758.5453);
    return random;
}
float3 rand2dTo3d(float2 value)
{
    return float3(
		rand2dTo1d(value, float2(12.989, 78.233)),
		rand2dTo1d(value, float2(39.346, 11.135)),
		rand2dTo1d(value, float2(73.156, 52.235))
	);
}
float2 rand2dTo2d(float2 value)
{
    return float2(
		rand2dTo1d(value, float2(12.989, 78.233)),
		rand2dTo1d(value, float2(39.346, 11.135))
	);
}
float4 main(float4 position : TEXCOORD2, float2 texCoord : TEXCOORD0, float2 vpos : VPOS) : COLOR
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
    normal = mul(normal, (float3x3) ViewInverseMatrix);
    WorldPositionFromDepth(texCoord, depth, ProjectionMatrix, ViewInverseMatrix, worldPosition);

    float3 viewDir = normalize(worldPosition - ViewInverseMatrix[3].xyz);
    float H = max(dot(viewDir, SunDir), 0.0);


    float3 AmbientOcclusion = tex2D(SSAOSampler, texCoord).rgb;

    float3 diffuse = target.rgb /** AmbientOcclusion * 1.75f*/;
    float3 specular = (target.a * target.rgb) /** AmbientOcclusion * 1.75f*/;
    
    outColor = (albedo.rgb * diffuse) + specular;
    float3 skyColor = lerp(HorizonColor, SkyLightColor, saturate((worldPosition.z - 20) * (1.0f / 500.0f)));
    float3 DiffuseTerm = (target.xyz + SkyLightColor.rgb * 0.3f) ;
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
                R = texCUBE(CubeMapSampler, r);
                R *= Parameters.x * 9.0f;
                R.rgb *= R.a;

            }
            else if (reflectionType == 2)
            {
                R = SphereEnvMap(normal, viewDir);
            }
            else if (reflectionType == 3)
            {
                R = ParaboloidEnvMap(normal, viewDir);
            }

            //R *= 1.5f;
            
            float3 noise = normalize(rand2dTo3d(worldPosition.xy / 16.0f) * 2.0f - 1.0f);
            float3 jitter = noise * Parameters.x;
             // fresnel
            float f;
            f = clamp(1.0 - dot(normal, -viewDir), 0.0, 1.0);
            f = pow(f, 8.0);
            R.rgb = lerp(albedo.rgb, R.rgb, f);
            R.rgb+= jitter*f;

        }
        else if (materialType == 3) // Skins
        {
        }

        outColor = DiffuseTerm * albedo.rgb + specular * Parameters.x + R.rgb  * Parameters.x;
            
        return float4(outColor, 1);
       
    }
    
    //float ti = SunColor.z;
    //return sky(normalize(worldPosition), 0.0, ti);
    return float4(outColor, 0);
}


