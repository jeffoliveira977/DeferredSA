#include "Utilities.hlsl"
row_major float4x4 ViewInverseMatrix : register(c0);
row_major float4x4 ProjectionMatrix : register(c4);
float4 FogData : register(c8);
float Time : register(c9);

sampler2D RandomSampler : register(s0);

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

static float2 Radius = { 0.005f,0.5f };
static float Bias = 0.00001f;


#define SAMPLE_COUNT 16
float3 RAND_SAMPLES[SAMPLE_COUNT] =
{
    float3(0.5381, 0.1856, -0.4319),
	  float3(0.1379, 0.2486, 0.4430),
      float3(0.3371, 0.5679, -0.0057),
	  float3(-0.6999, -0.0451, -0.0019),
      float3(0.0689, -0.1598, -0.8547),
	  float3(0.0560, 0.0069, -0.1843),
      float3(-0.0146, 0.1402, 0.0762),
	  float3(0.0100, -0.1924, -0.0344),
      float3(-0.3577, -0.5301, -0.4358),
	  float3(-0.3169, 0.1063, 0.0158),
      float3(0.0103, -0.5869, 0.0046),
	  float3(-0.0897, -0.4940, 0.3287),
      float3(0.7119, -0.0154, -0.0918),
	  float3(-0.0533, 0.0596, -0.5411),
      float3(0.0352, -0.0631, 0.5460),
	  float3(-0.4776, 0.2847, -0.0271)
};
  
float4 main(float4 texCoord : TEXCOORD0) : COLOR
{
    
    float3 outColor;
    float4 target = tex2D(SamplerLightTarget, texCoord.xy);
    float4 albedo = TEXTURE2D_ALBEDO(texCoord.xy);
    
    float depth;
    float3 normal;
    DecodeDepthNormal(texCoord.xy, FogData.y, depth, normal);
    
    float4 depthNormal = TEXTURE2D_DEPTHNORMAL(texCoord.xy);
    depth = DecodeFloatRG(depthNormal.zw, FogData.y);
    
    float4 OutLighting = float4(1, 1, 1, 1.0);
    float3 WorldPos;
    WorldPositionFromDepth(texCoord.xy, depth, ProjectionMatrix, ViewInverseMatrix, WorldPos);

	//ignore areas where we have no depth/normal information
    clip(depth + 0.9999f);

	// total occlusion
    float totalOcclusion = 0;
	
	//prevent near 0 divisions
    float scale = min(Radius.y, Radius.x / max(1, depth));
		
	//this will be used to avoid self-shadowing		  
    half3 normalScaled = normal * 0.25f;

	//pick a random normal, to add some "noise" to the output
   // half3 randNormal = (rand2dTo3d(texCoord) * 2.0 - 1.0);
			
	//pick a random normal, to add some "noise" to the output
    half3 randNormal = (tex2D(RandomSampler, texCoord.zw).rgb * 2.0 - 1.0);
			
    for (int i = 0; i < SAMPLE_COUNT; i++)
    {
		// reflect the pre-computed direction on the random normal 
        half3 randomDirection = reflect(RAND_SAMPLES[i], randNormal);
			
		// Prevent it pointing inside the geometry
        randomDirection *= sign(dot(normal, randomDirection));

		// add that scaled normal
        randomDirection += normalScaled;
		
		//we use a modified depth in the tests
        half modifiedDepth = depth - (randomDirection.z * Radius.x);
		//according to the distance to the camera, we should scale the direction to account the perspective
        half2 offset = randomDirection.xy * scale;
		
		// Sample depth at offset location

        float4 depthNormal = TEXTURE2D_DEPTHNORMAL(texCoord.xy + offset);
        float newDepth = DecodeFloatRG(depthNormal.zw, FogData.y);
        
		//we only care about samples in front of our original-modifies 
        float deltaDepth = saturate(modifiedDepth - newDepth);
			
		//ignore negative deltas
        totalOcclusion += (1 - deltaDepth) * (deltaDepth > Bias);
			      
    }
    totalOcclusion /= SAMPLE_COUNT;

    return totalOcclusion;
}