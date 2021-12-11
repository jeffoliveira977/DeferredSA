#include "Utilities.hlsl"

row_major float4x4 mViewInv : register(c0);
row_major float4x4 Projection : register(c4);
float3 vSunColor : register(c8);
float3 vSunLightDir : register(c9);
float3 vGradingColor0 : register(c10);
float3 vGradingColor1 : register(c11);
float3 fogData : register(c12);
float Time : register(c13);
sampler2D ScreenSampler : register(s4);
sampler2D SamplerDis : register(s5);
#define FogStart fogData.x
#define FarClip fogData.y

//Variables
float VolumeBox_top : register(c14);
float VolumeBox_bottom : register(c15);
static float MinStepLength = 6.0f;
static float MaxStepLength = 125.0f;
static float StepLengthMultiplyer = 8.0f;
static float MaxStep = 80.0f;
static float DensityCut = 0.05f;
static float TransparencyCut = 0.2f;

 float BodyTop : register(c16);
 float BodyMiddle : register(c17);
 float BodyBottom : register(c18);
float BodyThickness : register(c19);

static float DensityIncrease = 0.62f;
static float DensityIncrease_cloudy = 0.55f;

static float DensitySoftnessTop = 0.9f;
static float DensitySoftnessBottom = 1.2f;
static float DensitySoftnessBottom_cloudy = 1.55f;

static float ChunkDensityA = 0.5f;
static float ChunkDensityB = 0.55f;
static float DetailDensityC = 0.33f;
static float DetailDensityD = 0.1f;
static float DetailDensityE = 0.1f;
static float ChunkSizeA = 15.0f;
static float ChunkSizeB = 3.0f;
static float DetailSizeC = 3.5f;
static float DetailSizeD = 1.5f;
static float DetailSizeE = 0.75f;

static float temp1 = 1.0f;
static float temp2 = 0.2f;

static float3 cloud_shift = { -0.5, 0.0, 0.0 };

static float curvy_scale = 9.05f;
static float curvy_ang = 2.5f;
static float curvy_strength = 50.0f;

static float BeerFactor = 1.3f;
static float EffectFactor = 1.0f;
static float CloudSoftness = 0.05f;

static float ScatteringRange = 0.1f;
static float ScatteringStrength = 0.8f;
static float MoonStrength = 1.6f;

static float CloudContrast = 1.05f;
static float cloudBrightnessMultiply = 1.0f;

static float ShadowStepLength = 180.0f;
static float DetailShadowStepLength = 60.0f;
static float DetailShadowStrength = 0.2f;
static float shadowExpand = -0.22f;
static float shadowStrength = 0.2f;
static float shadowContrast = 0.2f;
static float shadowSoftness = 0.2f;

static float fogEffect = 0.2f;
static float FogEffect_cloudy = 2.0f;

static float AlphaCurve = 0.1f;
static float FadeStart = 2000.0f;
static float FadeEnd = 10000.0f;

float Atomesphere_Distance : register(c20);
float Atomesphere_Smoothness : register(c21);
 float3 Atomesphere : register(c22);

static float speed = 0.8f;
static float3 nA_move = { 1.0f, -3.0f, 0 };
static float3 nB_move = { 2.8f, 0, 0 };
static float3 nC_move = { 3.2f, 0, 0.3f };
static float3 nD_move = { 3.5f, 0, 0.5f };

static float3 CBC_D = { 0.616f, 0.686f, 0.718f };
static float3 CBC_N = { 0.118f, 0.118f, 0.118f };
static float3 CBC_S = { 0.416f, 0.502f, 0.525f };
static float3 CBC_CD = { 0.557f, 0.616f, 0.659f };
static float3 CBC_CN = { 0.275f, 0.275f, 0.275f };

static float3 CTC_D = { 0.953f, 1.0f, 0.988f };
static float3 CTC_N = { 0.153f, 0.125f, 0.0824f };
static float3 CTC_S = { 1.0f, 0.69f, 0.482f };
static float3 CTC_CD = { 0.894f, 0.949f, 0.957f };
static float3 CTC_CN = { 0.125f, 0.157f, 0.173f };

static float fatnessH = 0.46f;
static float softness_h = 0.74f;
static float scatteringStrength_h = 3.0f;
static float H_scaleA = 5.3f;
static float H_scaleB = 1.77f;
static float curvy_scale_h = 2.84f;
static float curvy_ang_h = 2.0f;
static float curvy_strength_h = 10.0f;
static float HdensA = 0.6f;
static float HdensB = -0.3f;
static float3 H_NoiseA_Direction = { -3.0f, 3.3f, 0.29f };
static float3 H_NoiseB_Direction = { -5.0f, 5.0f, 0.234f };

#define nsA		0.01/ChunkSizeA
#define nsB		0.01/ChunkSizeB
#define nsC		0.1/DetailSizeC
#define nsD		0.1/DetailSizeD
#define nsE		0.1/DetailSizeE

float3 PosOnPlane(const in float3 origin, const in float3 direction, const in float h, inout float dis)
{
    dis = (h - origin.z) / direction.z;

    return float3((origin + direction * dis).xy, h);
}

float3 PosOnSphere(const in float3 o, const in float3 d, const in float h, const in float r, inout float distance)
{
    float d1 = -h * d.z,
        d2 = sqrt(r * r - (h * h - d1 * d1));
    distance = d1 + d2;
    return o + d * distance;
}

float clampMap(float x, float a, float b, float c, float d)
{
    return clamp((x - b) / (a - b), 0.0, 1.0) * (c - d) + d;

}

float4 cloud_shape(float z, float4 profile, float3 dens_thres)
{
    float soft = clampMap(z, profile.y, profile.x, dens_thres.z, dens_thres.y);

    return
        float4(
            smoothstep(profile.z, lerp(profile.y, profile.z, profile.w), z) * smoothstep(profile.x, lerp(profile.y, profile.x, profile.w), z),
            dens_thres.x + soft,
            dens_thres.x - soft,
            soft);
}

//============IQ's noise===================
//This noise generator is developed by Inigo Quilez.
//License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
float hash(float n)
{
    return frac(sin(n) * 9487.5987);
}

float noise_p(float3 p)
{
    float3 fr = floor(p),
        ft = frac(p);


    float n = 3.0 * fr.x + 17.0 * fr.y + fr.z,
        nr = n + 3.0,
        nd = n + 17.0,
        no = nr + 17.0,

        v = lerp(hash(n), hash(n + 1.0), ft.z),
        vr = lerp(hash(nr), hash(nr + 1.0), ft.z),
        vd = lerp(hash(nd), hash(nd + 1.0), ft.z),
        vo = lerp(hash(no), hash(no + 1.0), ft.z);

    return ((v * (1.0 - ft.x) + vr * (ft.x)) * (1.0 - ft.y) +
            (vd * (1.0 - ft.x) + vo * ft.x) * ft.y);
}

float noise_d(const in float2 p)
{
    return tex2Dlod(SamplerDis, float4(p.xy,0, 0.0f)).r;
}
////1d noise function
//float hash11(float n)
//{
//    return fract(sin(n) * 43758.5453);
//}

////value noise function
//float noise_d(in vec3 n)
//{
//	vec3 p = floor(n);
//    vec3 f = fract(n);
//    f = f * f * (3. - 2. * f);
//    float q = p.x + p.y * 57. + p.z * 113.;
//    float r = mix(mix(mix(hash11(q + 0.), hash11(q + 1.), f.x),
//                      mix(hash11(q + 57.), hash11(q + 58.), f.x), f.y),
//                  mix(mix(hash11(q + 113.), hash11(q + 114.), f.x),
//                      mix(hash11(q + 170.), hash11(q + 171.), f.x), f.y), f.z);
//    return r;
//}

//float noise_d(in float3 x)
//{
//    vec3 p = floor(x);
//    vec3 f = fract(x);
//    f = f * f * (3.0 - 2.0 * f);
//	vec2 uv = (p.xy + vec2(37.0, 17.0) * p.z) + f.xy;
//	vec2 rg = tex2Dlod(SamplerDis, float4((uv + 0.5) / 112.0, 0.0, 0.0)).yx;
//    return mix(rg.x, rg.y, f.z);
//}

//static float randomSeed = 1618.03398875;

//float hash(float n)
//{
//    return frac(sin(n / 1873.1873) * randomSeed);
//}

//float noise2d(float3 p)
//{

//    float3 fr = floor(p),
//    ft = frac(p);

//    float n = 1153.0 * fr.x + 2381.0 * fr.y + p.z,
//    nr = n + 1153.0,
//    nd = n + 2381.0,
//    no = nr + 2381.0,

//    v = hash(n),
//    vr = hash(nr),
//    vd = hash(nd),
//    vo = hash(no);

//    return ((v * (1.0 - ft.x) + vr * (ft.x)) * (1.0 - ft.y) + (vd * (1.0 - ft.x) + vo * ft.x) * ft.y);
//}

//float noise3d(float3 p)
//{

//    float3 fr = floor(p),
//    ft = frac(p);

//    float n = 1153.0 * fr.x + 2381.0 * fr.y + fr.z,
//    nr = n + 1153.0,
//    nd = n + 2381.0,
//    no = nr + 2381.0,

//    v = lerp(hash(n), hash(n + 1.0), ft.z),
//    vr = lerp(hash(nr), hash(nr + 1.0), ft.z),
//    vd = lerp(hash(nd), hash(nd + 1.0), ft.z),
//    vo = lerp(hash(no), hash(no + 1.0), ft.z);

//    return lerp(lerp(v, vr, ft.x), lerp(vd, vo, ft.x), ft.y);
//}

//large chunk for the distribution of clouds.
float chunk(in float3 pos, const in float3 offsetA, const in float3 offsetB, in float cs)
{
    pos += cloud_shift * pos.z;
    float3
        pA = pos + offsetA,
        pB = pos + offsetB;

    return clamp((ChunkDensityA * noise_p(pA * nsA) + ChunkDensityB * noise_p(pB * nsB)) * cs, 0.0, 1.0);
}

//Detial noise for clouds
float detial(const in float3 pos, const in float3 offsetC, const in float3 offsetD)
{
    float3
        pC = pos + offsetC,
        pD = pos + offsetD;

    float nc = noise_p(pC * nsC);

    //curvy distortion
    float a = nc * curvy_ang;
    float3 d = float3(cos(a), sin(a), 0.0) * curvy_strength;

    float dens = DetailDensityC * nc + DetailDensityD * (1.0 - noise_p((pD + d) * nsD)) + DetailDensityE * noise_p((pD + d / 2) * nsE);

    return dens;
}

float detial2(const in float3 pos, const in float3 offsetC)
{
    float3
        pC = pos + offsetC;

    return DetailDensityC * noise_p(pC * nsC) * 2.0;
}

//one step marching for lighting
float brightness(float3 p, float4 profile, float3 dens_thres, const in float3 offsetA, const in float3 offsetB, const in float3 offsetC)
{
    float3
        p1 = p + vSunLightDir.xyz * ShadowStepLength,
        p2 = p + vSunLightDir.xyz * DetailShadowStepLength;

    float4
        cs = cloud_shape(p1.z, profile, dens_thres);

    float
        dens1 = chunk(p1, offsetA, offsetB, cs.x),
        dens2 = detial2(p2, offsetC) * DetailShadowStrength,

        d = (dens1 + dens2) * shadowContrast + shadowExpand - (cs.z - shadowSoftness);
    d /= (cs.w + shadowSoftness) * 2.0;

    return 1.0 - clamp(d, 0.0, 1.0) * shadowStrength;
}

float4 Cloud_mid(const in float3 cam_dir, in float3 p, in float dist, const in float depth, float time, float moonLight, float tf, float sunny)
{
    float4 d = float4(0.0, 0.0, 0.0, MinStepLength); //distance, distance factor, gap, the last march step

    p = PosOnPlane(p, cam_dir, clamp(p.z, VolumeBox_bottom, VolumeBox_top), d.x);
    d.y = d.x;

    if (d.x >= 0.0 && d.x < FadeEnd)
    {
        float
            fe = lerp(FogEffect_cloudy, fogEffect, sunny) / 100.0;

        float3
            flowA = -time * nA_move,
            flowB = -time * nB_move,
            flowC = -time * nC_move,
            flowD = -time * nD_move,

            dens_thres = float3(
                1.0 / lerp(DensityIncrease_cloudy, DensityIncrease, sunny),
                DensitySoftnessTop,
                lerp(DensitySoftnessBottom_cloudy, DensitySoftnessBottom, sunny)),

            fx = float3(0.5, 0.0, 1.0); //brightness, density, transparency

        float4 profile = float4(BodyTop, BodyMiddle, BodyBottom, BodyThickness);

        if (depth == 1)
            dist = FadeEnd;


        for (int i = 0.0; fx.z > 0.0 && p.z <= VolumeBox_top && p.z >= VolumeBox_bottom && i < MaxStep && d.x - d.w < dist; i += 1.0)
        {
            float3 cs = cloud_shape(p.z, profile, dens_thres).xyz;

            float
                dens = (chunk(p, flowA, flowB, cs.x) + temp2) * (detial(p, flowC, flowD) + temp1) / (1 + temp2) / (1 + temp1),
                marchStep = clampMap(dens, cs.z, DensityCut, MinStepLength, MaxStepLength) * clampMap(d.x, 0.0, 10000.0, 1.0, StepLengthMultiplyer),
                coverage = smoothstep(cs.z, cs.y, dens) * marchStep / CloudSoftness,
                ds = fx.z + fe;

            //fade into near objects
            if (d.x >= dist)
            {
                coverage *= d.z / d.w;
                fx.y += fe * d.z / d.w;
            }
            else
                fx.y += fe;

            ds *= coverage / EffectFactor;
            fx.x = lerp(fx.x, brightness(p, profile, dens_thres, flowA, flowB, flowC), ds);
            d.y = d.y * (1.0 - ds) + ds * d.x;

            fx.y += coverage;
            fx.z = (exp(-fx.y / BeerFactor) - TransparencyCut) / (1.0 - TransparencyCut);

            d.z = dist - d.x;

            //marching
            p += cam_dir * marchStep;
            d.x += marchStep;
            d.w = marchStep;
        }

        if (fx.y > 0.0)
        {
            float
                noon_index = smoothstep(-0.09, 0.3, tf),
                sunset_index = smoothstep(0.3, 0.1, tf) * smoothstep(-0.18, -0.08, tf),
                suncross = dot(vSunLightDir.xyz, cam_dir);

            float3
                c_dark = lerp(lerp(lerp(CBC_CN, CBC_CD, noon_index), lerp(CBC_N, CBC_D, noon_index), sunny), CBC_S, sunset_index),
                c_bright = lerp(lerp(lerp(CTC_CN, CTC_CD, noon_index), lerp(CTC_N, CTC_D, noon_index), sunny), CTC_S, sunset_index) * cloudBrightnessMultiply;

            c_bright += normalize(c_bright) * ScatteringStrength * exp((suncross - 1.0) / ScatteringRange) * (smoothstep(-0.13, -0.08, tf));

            float3 C = lerp(c_dark, c_bright, smoothstep(0.0, 1.0, (fx.x - 0.5) * CloudContrast + 0.5));

            float rsf = smoothstep(Atomesphere_Distance - Atomesphere_Smoothness, Atomesphere_Distance + Atomesphere_Smoothness, d.y);
            C += Atomesphere / 100.0 * rsf;

            //fading
            fx.y = 1.0 - exp(-fx.y / AlphaCurve);
            fx.y *= smoothstep(FadeEnd, FadeStart, d.y);
            return float4(C, fx.y);
        }
    }
    return float4(0.0, 0.0, 0.0, 0.0);
}


//high cloud
float Density_h(const in float3 pos, const in float3 flowA, const in float3 flowB)
{
    float3
        pA = pos + flowA,
        pB = pos + flowB;

    float a = noise_p(pB / curvy_scale_h) * curvy_ang_h;

    float3 d = float3(cos(a), sin(a), 0.0) * curvy_strength_h;

    float
        densA = HdensA * noise_p((pA + d) / H_scaleA),
        densB = HdensB * noise_p((pB + d) / H_scaleB) * densA;

    return densA + densB;
}

float4 Cloud_high(const in float3 cam_dir, const in float3 cam_pos, const in float dist, const in float depth, float time, float height, float moonLight, float tf, float sunny)
{
    float distance = 0.0;
    float3 position = PosOnSphere(cam_pos, cam_dir, cam_pos.z + 1000000.0, height + 1000000.0, distance);

    if (distance > 0.0 && (depth == 1.0 || distance < dist) && cam_dir.z > -0.1)
    {
        float
            alpha = 0.0,
            noon_index = smoothstep(0.0, 0.3, tf),
            sunset_index = smoothstep(0.3, 0.1, tf) * smoothstep(-0.2, -0.1, tf) * sunny,
            suncross = dot(vSunLightDir.xyz, cam_dir),
            nearMoon = MoonStrength * smoothstep(0.9, 1.2, dot(float3(-0.12, -0.9, 0.45), cam_dir)) * moonLight;

        float3
            c_dark = lerp(lerp(lerp(CBC_CN, CBC_CD, noon_index), lerp(CBC_N, CBC_D, noon_index), sunny), CBC_S, sunset_index),
            c_bright = lerp(lerp(lerp(CTC_CN, CTC_CD, noon_index), lerp(CTC_N, CTC_D, noon_index), sunny), CTC_S, sunset_index);

        c_bright += normalize(c_bright) * scatteringStrength_h * exp((suncross - 1.0) / ScatteringRange / 1.5);

        float3 C = float4(0.0, 0.0, 0.0, 0.0),
            flowA = -time * H_NoiseA_Direction,
            flowB = -time * H_NoiseB_Direction;

        alpha = Density_h(position / 1500.0, flowA, flowB);

        //density threshold
        alpha *= smoothstep(fatnessH - softness_h, fatnessH + softness_h, alpha);

        alpha *= smoothstep(-0.1, 0.1, cam_dir.z);

        C = lerp(c_bright, c_dark, alpha);

        return float4(C, alpha);
    }
    return float4(0.0, 0.0, 0.0, 0.0);
}

//maybe fixed?
//float SunnyFactor()
//{
//    float4 c, w = WeatherAndTime;
//
//    if(w.x <= 3 || w.x == 6 || w.x == 11 || w.x == 13 || w.x == 14 || w.x == 15 || w.x == 17)
//        c.x = 1.0;
//    else
//        c.x = 0.0;
//
//    if(w.x <= 3 || w.x == 6 || w.x == 11 || w.x == 13 || w.x == 14 || w.x == 15 || w.x == 17)
//        c.y = 1.0;
//    else
//        c.y = 0.0;
//
//    return lerp(c.x, c.y, w.z);
//}

inline void DecodeDepthNormal2(float2 texCoord, float farPlane, out float depth, out float3 normal)
{
    float4 depthNormal = TEXTURE2D_DEPTHNORMAL(texCoord);
    normal = TwoChannelNormalX2(depthNormal.xy);
    depth = DecodeFloatRG(depthNormal.zw, farPlane);
    depth = depth <= 0.0 ? 100000.0f : depth;
}

static float2 ScreenSize = { 1.0 / 1080.0, 1 / 1920.0f };

float4 main(float4 position : POSITION, float2 texcoord : TEXCOORD, float2 vPos: VPOS) : COLOR
{
    float4 OutLighting;

    float3 ViewPos = mViewInv[3].xyz;

    float depth;
    float3 normal;
    DecodeDepthNormal2(texcoord, fogData.y, depth, normal);
    
    float3 WorldPos;
    WorldPositionFromDepth(texcoord, depth, Projection, mViewInv, WorldPos);
    float3 LightDir = normalize(vSunLightDir.xyz);
    float4 r0 = tex2D(ScreenSampler, texcoord.xy);
    float
        ti = LightDir.z,
        moon = 0.0,
        sunny = 0.5,
        dist = length(WorldPos - ViewPos);

    float3 direction = normalize(WorldPos.xyz);
    float3 ViewDir = normalize(WorldPos - ViewPos);

    //clouds
    float4 highCloud = Cloud_high(direction, ViewPos.xyz, dist, depth, Time / 220, 2000.0, moon, ti, sunny);
    r0.xyz = lerp(r0.rgb, highCloud.rgb, highCloud.a);

    float4 midCloud = Cloud_mid(direction, ViewPos.xyz, dist, depth, Time / 2.2, moon, ti, sunny);
    r0.xyz = lerp(r0.rgb, midCloud.rgb, midCloud.a);

    return r0;
}