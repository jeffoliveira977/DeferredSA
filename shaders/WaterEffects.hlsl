#include "Utilities.hlsl"
// VS
row_major float4x4 xView : register(c4);

// VS/PS
row_major float4x4 xProjection : register(c8);
float Time : register(c12);

// PS
row_major float4x4 xViewInverse : register(c4);
float3 xLightDirection : register(c13);
float3 Fog : register(c14);

float4 FogColor : register(c15);
float4 Watercolor : register(c16);
float4 SunColor : register(c17);
float4 SkyLightColor : register(c18);
float4 HorizonColor : register(c19);

// variables for the BUMP MAP
static float xDrawMode = 2.0;
static float xWaveLength = 0.4f;
static float xWaveHeight = 0.4f;
static int fresnelMode = 2;
static float specPerturb =12.0;
static float specPower = 400.0;
static float xdullBlendFactor = 0.5f;
static float xWindForce = 0.1;

sampler WaterBumpMapSampler : register(s1);
sampler ReflectionSampler : register(s2);
sampler RefractionSampler : register(s3);
sampler EnvSampler : register(s4);
sampler2D SamplerDepth : register(s5);
sampler2D WaterWake : register(s6);
//------- Technique: Water --------

struct WaterVertexToPixel
{
    float4 Position       : POSITION;  
    float2 texcoor : TEXCOORD0;
    float4 vposition     : TEXCOORD1;
    float4 worldPosition : TEXCOORD2;
    float4 waveCoord     : TEXCOORD3;
    float2 texcoord : TEXCOORD4;
    float2 texcoord1 : TEXCOORD5;
    float depth          : DEPTH;
};


struct WaterPixelToFrame
{
    float4 Color : COLOR0;
};

static float flowSpeed = 0.5;

static float2 ScreenSize = float2(1.0 / 1920.0, 1.0/1080.0);

WaterVertexToPixel WaterVS(float4 inPos : POSITION, float2 inTex: TEXCOORD0)
{
    WaterVertexToPixel Output = (WaterVertexToPixel) 0;
    float4x4 identity = float4x4(1, 0, 0, 0,
                                 0, 1, 0, 0,
                                 0, 0, 1, 0,
                                 0, 0, 0, 1);
     
    float4 position = float4(inPos.xyz, 1.0);
    float4 wPos = mul(position, identity);
    float4 vPos = mul(wPos, xView);
    float4 pPos = mul(vPos, xProjection);
    
    Output.texcoor = inTex;
    Output.Position = pPos;
    Output.vposition = pPos;
    Output.worldPosition = position;
    Output.depth = pPos.z;
    Output.texcoord = CalculateScreenPosition(inPos)/ ScreenSize ;
    
    float timer = fmod(Time / 12, 1) * flowSpeed;
    Output.waveCoord.xy = inTex * 3 + float2(timer, timer);
    Output.waveCoord.zw = inTex * 3 + float2(-timer, timer);
    
    float2 uvpos1 = 0;
    float2 uvpos2 = 0;

    uvpos1.x = sin(Time / 40);
    uvpos1.y = fmod(Time / 50, 1);

    uvpos2.x = fmod(Time / 10, 1);
    uvpos2.y = sin(Time / 12);

    Output.waveCoord.x = inTex.x * 1 + uvpos1.x;
    Output.waveCoord.y = inTex.y * 1 + uvpos1.y;
    Output.waveCoord.z = inTex.x * 2 + uvpos2.x;
    Output.waveCoord.w = inTex.y * 2 + uvpos2.y;
    Output.texcoord1 = CalculateVPos(pPos);
    return Output;
}

float4 GetFog(float4 color, float fogFactor)
{
    return lerp(color, FogColor, fogFactor);
}

float GetFogFactor(float depth)
{
    return saturate(max(depth - Fog.x, 0) / max(Fog.y, 0.001f));
}

float GetFogFactor(float depth, float far, float near)
{
    return saturate(max(depth - far, 0) / max(near, 0.001f));
}

float4 CalculateFog(float4 color, float depth)
{
    return GetFog(color, GetFogFactor(depth));
}

float diffuse(vec3 n, vec3 l, float p)
{
    return pow(dot(n, l) * 0.4 + 0.6, p);
}
float specular(vec3 n, vec3 l, vec3 e, float s)
{
    float nrm = (s + 8.0) / (PI * 8.0);
    return pow(max(dot(reflect(e, n), l), 0.0), s) * nrm;
}

float3 HUEtoRGB(in float H)
{
    float R = abs(H * 6 - 3) - 1;
    float G = 2 - abs(H * 6 - 2);
    float B = 2 - abs(H * 6 - 4);
    return saturate(float3(R, G, B));
}

float3 HSVtoRGB(in float3 HSV)
{
    float3 RGB = HUEtoRGB(HSV.x);
    return ((RGB - 1) * HSV.y + 1) * HSV.z;
}

static float Epsilon = 1e-10;
 
float3 RGBtoHCV(in float3 RGB)
{
    // Based on work by Sam Hocevar and Emil Persson
    float4 P = (RGB.g < RGB.b) ? float4(RGB.bg, -1.0, 2.0 / 3.0) : float4(RGB.gb, 0.0, -1.0 / 3.0);
    float4 Q = (RGB.r < P.x) ? float4(P.xyw, RGB.r) : float4(RGB.r, P.yzx);
    float C = Q.x - min(Q.w, Q.y);
    float H = abs((Q.w - Q.y) / (6 * C + Epsilon) + Q.z);
    return float3(H, C, Q.x);
}

float3 RGBtoHSV(in float3 RGB)
{
    float3 HCV = RGBtoHCV(RGB);
    float S = HCV.y / (HCV.z + Epsilon);
    return float3(HCV.x, S, HCV.z);
}
inline float pow5(float v)
{
    return v * v * v * v * v;
}

vec3 fresnelSchlick(float cosTheta, float F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, float F0, float roughness)
{
    return F0 + (max(1.0 - roughness, F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

float GGX_PartialGeometry(float cosThetaN, float alpha)
{
    float cosTheta_sqr = saturate(cosThetaN * cosThetaN);
    float tan2 = (1 - cosTheta_sqr) / cosTheta_sqr;
    float GP = 2 / (1 + sqrt(1 + alpha * alpha * tan2));
    return GP;
}
float GGX_Distribution(float cosThetaNH, float alpha)
{
    float alpha2 = alpha * alpha;
    float NH_sqr = saturate(cosThetaNH * cosThetaNH);
    float den = NH_sqr * alpha2 + (1.0 - NH_sqr);
    return alpha2 / (PI * den * den);
}
float MicrofacetNDF_GGX(in float3 vNormal, in float3 vHalfWay, in float fRoughness)
{
    float fRoughnessSqr = fRoughness * fRoughness;

    float fCosAlphaSqr = saturate(dot(vNormal, vHalfWay)); // Alpha is angle between normal and half-way vector
	//fCosAlphaSqr *= fCosAlphaSqr;		// need to check if can be optimized to use less instructions
    float fDiv = ((fRoughnessSqr - 1) * fCosAlphaSqr + 1);
    float fNDCoeff = fRoughnessSqr;
    fNDCoeff /= PI * (fDiv * fDiv + 1e-7f);

    half d = (fCosAlphaSqr * fRoughnessSqr - fCosAlphaSqr) * fCosAlphaSqr + 1.0f; // 2 mad
    return fRoughnessSqr / (d * d + 1e-7f);
	//return fNDCoeff;
	//float 
}
inline float SmithJointGGXVisibilityTerm(float NdotL, float NdotV, float roughness)
{
	// Original formulation:
	//	lambda_v	= (-1 + sqrt(a2 * (1 - NdotL2) / NdotL2 + 1)) * 0.5f;
	//	lambda_l	= (-1 + sqrt(a2 * (1 - NdotV2) / NdotV2 + 1)) * 0.5f;
	//	G			= 1 / (1 + lambda_v + lambda_l);

	// Reorder code to be more optimal
    float a = roughness;
    float a2 = a * a;

    float lambdaV = NdotL * sqrt((-NdotV * a2 + NdotV) * NdotV + a2);
    float lambdaL = NdotV * sqrt((-NdotL * a2 + NdotL) * NdotL + a2);

	// Simplify visibility term: (2.0f * NdotL * NdotV) /  ((4.0f * NdotL * NdotV) * (lambda_v + lambda_l + 1e-5f));
    return 0.5f / (lambdaV + lambdaL + 1e-5f);
}

#define USE_PBR 1
void CalculateSpecularTerm(in float3 vNormal, in float3 vLightDir, in float3 vViewDir, in float fRoughness, out float fSpecularTerm)
{
    float3 vHalfWay = normalize(vLightDir + vViewDir);
    float roug_sqr = fRoughness * fRoughness;
    float G = GGX_PartialGeometry(dot(vNormal, vViewDir), roug_sqr) * GGX_PartialGeometry(dot(vNormal, vLightDir), roug_sqr);
    float D = GGX_Distribution(dot(vNormal, vHalfWay), roug_sqr);
    float ndfTerm = MicrofacetNDF_GGX(vNormal, vHalfWay, fRoughness * fRoughness);
    float ndl = saturate(dot(vNormal, vLightDir));
    float ndv = saturate(dot(vNormal, vViewDir));
    float visibilityTerm = SmithJointGGXVisibilityTerm(ndl, ndv, fRoughness * fRoughness);
    /*if (ndv <= 0.0 || ndl <= 0.0)
    {
        fSpecularTerm = 0.0f;
        return;
    }*/
	//MicrofacetGeometricShadow(vNormal, vViewDir, fRoughness);
#if USE_PBR==1
    fSpecularTerm = ndfTerm * 0.5f;
#else
    fSpecularTerm = PhongSpecular(vNormal, vLightDir, -vViewDir, fRoughness) * 4.0f;
#endif
    //
    //ndfTerm * fresnelTerm;
}
static const float3 g_vLuminance = float3(.299, .587, .114);
float GetLuminance(float3 Color)
{
    return dot(g_vLuminance, Color);
}

float3 hash1(float3 p3)
{
    p3 = frac(p3 * float3(0.1031, 0.11369, 0.13787));
    p3 += dot(p3, p3.yxz + 19.19);
    p3 = float3((p3.x + p3.y), (p3.x + p3.z), (p3.y + p3.z)) * p3.zyx;
    return frac(p3);
}

float WorleyNoise(float3 n, float scale)
{
    n *= scale;
    float2 dis = 1.0;
    for (float x = -1.0; x < 1.01; x += 1.0)
    {
        for (float y = -1.0; y < 1.01; y += 1.0)
        {
            for (float z = -1.0; z < 1.01; z += 1.0)
            {
                float3 xyz = float3(x, y, z);
                float3 frcn = frac(n);
                float3 p = n - frcn + xyz;
                float3 pp = p;
                if (pp.z < 0.0)
                    pp.z = scale + pp.z;
                if (pp.z > 1.0)
                    pp.z = pp.z - scale;
                float2 sqrdist;
                frcn = xyz - frcn;
                p = hash1(pp) + frcn;
                sqrdist.x = dot(p, p);
                pp.z += 0.012;
                p = hash1(pp) + frcn;
                sqrdist.y = dot(p, p);
                dis.xy = min(dis.xy, sqrdist.xy);
            }
        }
    }
    dis.x = dis.x * dis.y;
    return saturate(sqrt(dis.x));
}
vec3 hash(vec3 n)
{
    return fract(sin(dot(n, vec3(0.9898, 0.1414, 0.233))));
}

float voronoi3D(vec3 n, float time)
{
    float t = Time * 2;
    float2 md = 1.0;
    
    for (float x = -1.0; x <= 1.0; x++)
    {
        for (float y = -1.0; y <= 1.0; y++)
        {
            for (float z = -1.0; z <= 1.0; z++)
            {
                float3 g = float3(x, y, z);
                float3 p = floor(n) + g;
                float3 o = g - frac(n);
                float2 d;
                
                g = 0.27 * sin(hash(p) * 64.0 + t) + o;
                d.x = mul(g, g);               
                p.z += 0.782;
                g = 0.27 * sin(hash(p) * 64.0 + t) + o;
                d.y = mul(g, g);               
                md = min(md, d);
            }
        }
    }
    return sqrt(md.x * md.y);
}
float farFade(float texap, float3 worldPos)
{
    float d = length(worldPos - xViewInverse[3].xyz);
    texap = lerp(texap, 0, GetFogFactor(d, Fog.x, Fog.y));
    return texap;
}
inline float getLinearDepth(in float z, in float near, in float far)
{
    float z_n = 2.0 * z - 1.0;
    float lin = 2.0 * far * near / (near + far - z_n * (near - far));
    return lin;
}
float MicrofacetFresnel(in float3 LightDir, in float3 Normal, in float fRoughness)
{
    float IOR = 1.5f;
    float f0 = (1 - IOR) / (1 + IOR);
    f0 *= f0;
    // Cosine between light and normal
    float CosPhi = max(dot(LightDir, Normal), 0);

    return f0 + (1 - f0) * pow(1 - CosPhi, 5.0);
}
static float normalMult = 2.6;
float4 WaterPS(WaterVertexToPixel PSIn, float2 vpos : VPOS0) : COLOR
{
    // Calculate water depth    
    float2 sPixelSize = float2(1920.0, 1080.0);
    float2 txcoord = vpos / sPixelSize;
    
    float sceneZ = DecodeDepth(SamplerDepth, txcoord) * Fog.y;
    float objectZ = PSIn.depth;

    sceneZ = sceneZ <= objectZ ? Fog.y : sceneZ;
    float waterDepth, causDepth, foamDepth, alphaDepth;
    waterDepth = sceneZ - objectZ;
   
    causDepth = saturate(waterDepth / 4.0f);
    foamDepth = saturate(waterDepth / .3f);
    alphaDepth = saturate(waterDepth / 2.2f);
    waterDepth = saturate(waterDepth);
 
    // Get world position from depth buffer
    float3 depthPosition;
    WorldPositionFromDepth(txcoord, sceneZ, xProjection, xViewInverse, depthPosition);
    
    // create caustics
   // float caustic = voronoi3D(depthPosition * 2.2, Time);
    
    float3 causticspos = depthPosition;
    causticspos *= 0.25;
    causticspos.z *= 0.25;
    causticspos.z += Time * 0.1;
    causticspos.z = frac(causticspos.z);
    float caustic = WorleyNoise(causticspos, 8);
    
    // get light and view direction
    float3 lightDir = normalize(xLightDirection);
    float3 viewDir = normalize(PSIn.worldPosition.xyz - xViewInverse[3].xyz);
    
    // half vector from view and light direction;
    float3 H = normalize(-viewDir + lightDir);
    
    // calculate screen projection
    float2 ProjectedTexCoords;
    ProjectedTexCoords.x = PSIn.vposition.x / PSIn.vposition.w / 2.0f + 0.5f;
    ProjectedTexCoords.y = -PSIn.vposition.y / PSIn.vposition.w / 2.0f + 0.5f;

    float3 vFlakesNormal = tex2D(WaterBumpMapSampler, PSIn.waveCoord.xy).rgb;
    float3 vFlakesNormal2 = tex2D(WaterBumpMapSampler, PSIn.waveCoord.zw).rgb;
   
    float water_depth = objectZ; // linear depth spanning from near plane to far plane
    float scene_depth = sceneZ;
    scene_depth = getLinearDepth(scene_depth, Fog.x, Fog.y);
    float fade_strength = 0.002; // how strong the fade effect is
    float depth_difference = saturate(abs(scene_depth - water_depth) * fade_strength);
    
    //normalMult *= (1 - depth_difference);
  
    vFlakesNormal = (vFlakesNormal + vFlakesNormal2) / 2;
    vFlakesNormal = 2.0 * vFlakesNormal - 1.0;
    float3 normal = normalize(vFlakesNormal.xyz * float3(0.1 + normalMult, 0.1 + normalMult, 1));
    normal = (normal.x * float3(1, 0, 0) + normal.y * float3(0, 1, 0));
    float2 perturbation = normal.xy * 0.03;
    ProjectedTexCoords += perturbation;
    float4 reflectiveColor = tex2D(ReflectionSampler, ProjectedTexCoords);
    float4 refractiveColor = tex2D(RefractionSampler, ProjectedTexCoords);

    float4 sWaterColor = float4(RGBVALUE(10, 51, 102), 1) * 0.7;
    float4 waterColorBase = float4(RGBVALUE(10, 51, 102), 1) * 0.7;
    waterColorBase.rgb = HSVtoRGB(HSVVALUE(206, 90, 41));
       
    // fresnel
    float f;
    f = clamp(1.0 - dot(normal, -viewDir), 0.0, 1.0);
    f = pow(f, 2.0);
    float4 combinedColor = lerp(refractiveColor, reflectiveColor, f);
    
    int waterType = 1;
    if (waterType == 1)
        combinedColor = lerp(waterColorBase * 0.7, Watercolor * 0.8, f * causDepth) * combinedColor * 0.8;
    else
    {
        sWaterColor = lerp(waterColorBase, sWaterColor, f * 0.7);
        combinedColor = lerp(waterColorBase * 0.4, sWaterColor, f * causDepth) * combinedColor;
    }
    
    // caustics
    caustic = caustic * caustic;
    caustic = caustic - 0.1;
    caustic += 1.0;
    combinedColor = lerp(combinedColor, refractiveColor * caustic, 0.4 * (1 - causDepth));

    float3 sun = SunColor.rgb;
    sun = RGBtoHSV(sun);
    sun = HSVtoRGB(float3(sun.x, sun.y*0.7, sun.z));
    // luminance
    //combinedColor.rgb += diffuse(normal, H, 40.0) * SunColor.rgb * 0.4;
    
    float glow = 100.0;
    combinedColor.rgb += BlinnPhongSpecular(lightDir, -viewDir, normal, glow) * sun;
    
    // foam 
    float sinTimer = sin(Time);
    float2 foamuv = 7.0f * PSIn.texcoor + 0.5 * float2(sinTimer, 1);
    float4 foamColor = tex2D(WaterWake, foamuv);
    combinedColor += foamColor * (1 - foamDepth);
    
    combinedColor = CalculateFog(combinedColor, objectZ);
    
    combinedColor.a = 1.0;
    return combinedColor;
}