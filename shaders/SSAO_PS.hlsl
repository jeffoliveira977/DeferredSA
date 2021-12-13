#include "Utilities.hlsl"
row_major float4x4 ViewInverseMatrix : register(c0);
row_major float4x4 ProjectionMatrix : register(c4);
float4 FogData : register(c8);
float Time;

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
inline float clampMap(float x, float a, float b, float c, float d)
{
    return saturate((x - b) / (a - b)) * (c - d) + d;
}

static float randomSeed = 1618.03398875;

float hash(float n)
{
    return frac(sin(n / 1873.1873) * randomSeed);
}

float noise3d_timedep(float3 p, float time)
{

    float3 fr = floor(p),
    ft = frac(p);

    float n = 1153.0 * fr.x + 2381.0 * fr.y + fr.z + time,
    nr = n + 1153.0,
    nd = n + 2381.0,
    no = nr + 2381.0,

    v = lerp(hash(n), hash(n + 1.0), ft.z),
    vr = lerp(hash(nr), hash(nr + 1.0), ft.z),
    vd = lerp(hash(nd), hash(nd + 1.0), ft.z),
    vo = lerp(hash(no), hash(no + 1.0), ft.z);

    return lerp(lerp(v, vr, ft.x), lerp(vd, vo, ft.x), ft.y);
}

static float fScreenWidth = 1920.0f;
static float fScreenHeight = 1080.0f;

float MySuckAmbientOcclusion(float ViewZ, float2 texcoord, float3 worldpos, float3 normal)
{
	//early exit
    if (ViewZ > 50.0)
        return 0.0;

    float weight = 0.0f, sum = 0.0f;
    float shift = noise3d_timedep(worldpos * 500.0, Time * 10.15) * 16.0;
    uint NUM = clampMap(ViewZ, 5.0, 50.0, 24, 12); //dynamic quality
	
    [unroll]
    for (float i = 1.0f; i <= NUM; i++)
    {
        float r = 200.0f * i / (NUM - 1.0f) / pow(ViewZ, 0.35);
        float t = 10.166406827f * i + shift;
		
        float2 samplecoord = texcoord + float2(cos(t) / fScreenWidth, sin(t) / fScreenHeight) * r;
		
        if (samplecoord.x < 0.0f || samplecoord.y < 0.0f || samplecoord.x > 1.0f || samplecoord.y > 1.0f)
            continue;
		
        float sampleViewZ = 0.0;
        float3 sampleNorm = 0.0;

        DecodeDepthNormal(samplecoord, FogData.y, sampleViewZ, sampleNorm);
        float3 sample_pos;
        WorldPositionFromDepth(samplecoord, sampleViewZ, ProjectionMatrix, ViewInverseMatrix, sample_pos);
        

        float3 ntz = sample_pos - worldpos;
        float distance = length(ntz);
        float w = clampMap(distance, 0.0, 1.5, 1.0, 0.0);
		
        if (isinf(w) || isnan(w))
            w = 0.0f;
        
        if (w <= 0.0)
            continue;
        
		
        float3 unit_vec = ntz / distance;
		
        sum += (abs(dot(unit_vec, sampleNorm)) + 0.5) * max(dot(unit_vec, normal), 0.0) * w;
        weight += w;
    }
    sum *= smoothstep(50.0, 40.0, ViewZ); //fading
    return weight > 0.0 ? sum / weight : 0.0;
}
float4 main(float2 texCoord : TEXCOORD0) : COLOR
{
    
    float3 outColor;
    float4 target = tex2D(SamplerLightTarget, texCoord);
    float4 albedo = TEXTURE2D_ALBEDO(texCoord);
    
    float depth;
    float3 normal;
    DecodeDepthNormal(texCoord, FogData.y, depth, normal);
    
    float4 OutLighting = float4(1, 1, 1, 1.0);
    float3 WorldPos;
    WorldPositionFromDepth(texCoord, depth, ProjectionMatrix, ViewInverseMatrix, WorldPos);
    OutLighting.r -= 2.0f * MySuckAmbientOcclusion(depth, texCoord, WorldPos, normal);

    return OutLighting;
    float2 uv = texCoord;

    float Occlusion = 0.07f;

    float2 rand2[16] =
    {
        float2(0.53812504, 0.18565957),
        float2(0.13790712, 0.24864247),
        float2(0.33715037, 0.56794053),
        float2(-0.6999805, -0.04511441),
        float2(0.06896307, -0.15983082),
        float2(0.056099437, 0.006954967),
        float2(-0.014653638, 0.14027752),
        float2(0.010019933, -0.1924225),
        float2(-0.35775623, -0.5301969),
        float2(-0.3169221, 0.106360726),
        float2(0.010350345, -0.58698344),
        float2(-0.08972908, -0.49408212),
        float2(0.7119986, -0.0154690035),
        float2(-0.053382345, 0.059675813),
        float2(0.035267662, -0.063188605),
        float2(-0.47761092, 0.2847911)
    };

    float color = 1.0f;

    for (int i = 0; i < 16; i++)
    {
        float2 d = rand2[i] * 0.003f;

        float depth1, depth2;
        DecodeDepthNormal(uv + d, FogData.y, depth1, normal);
        DecodeDepthNormal(uv - d, FogData.y, depth2, normal);

        if (depth > (depth1 + depth2) * 0.5f + 0.005f
         && depth < (depth1 + depth2) * 0.5f + 0.02f)
        {
            color -= Occlusion;
        }
    }

    float4 outDiffuse = color;
    outDiffuse.a = 1.0f;

    return outDiffuse;
}