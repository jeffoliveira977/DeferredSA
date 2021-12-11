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
static float vb_t = 550.0;
static float vb_b = 440.0;
static float min_sl = {6.0};
static float max_sl = {125.0};
static float step_mul = {8.0};
static float maxStep = {100.0};
static float dCut = {0.05};
static float tCut = {0.2};

static float body_top = {800.0};
static float body_mid = {450.0};
static float body_bot = {300.0};
static float body_thickness = {0.05};

static float grow = {0.62};
static float grow_c = {0.55};

static float soft_top = {0.9};
static float soft_bot = {1.2};
static float soft_bot_c = {1.55};

static float noise_densA = {0.5};
static float noise_densB = {0.55};
static float noise_densC = {0.33};
static float noise_densD = {0.1};
static float noise_densE = {0.1};
static float noiseSizeA = {15.0};
static float noiseSizeB = {3.0};
static float noiseSizeC = {3.5};
static float noiseSizeD = {1.5};
static float noiseSizeE = {0.75};

static float temp1 = {1.0};
static float temp2 = {0.2};

static float3 cloud_shift = {-0.5, 0.0, 0.0};

static float curvy_scale = {9.05};
static float curvy_ang  ={2.5};
static float curvy_strength = {50.0};

static float BeerFactor = {1.3};
static float EffectFactor = {1.0};
static float CloudSoftness = {0.05};

static float scattering = {0.1};
static float scatteringStrength = {0.8};
static float moonlight_strength = {1.6};

static float CloudContrast = {0.1};
static float cloudBrightnessMultiply = {1.0};

static float ShadowStepLength = {100.0};
static float DetailShadowStepLength  = {60.0};
static float ShadowDetail  = {0.2};
static float shadowExpand  = {-0.22};
static float shadowStrength = {0.2};
static float shadowContrast = {0.9};
static float shadowSoftness = {0.2};

static float fogEffect = {0.2};
static float fogEffect_c = {2.0};

static float alpha_curve = {0.1};
static float fade_s = {2000.0};
static float fade_e = {10000.0};

static float Atomesphere_Distance = {2000.0};
static float Atomesphere_Smoothness = {6000.0};
static float3 Atomesphere = {-5.92, -5.32, 3.81};

static float speed = {10.0};
static float3 nA_move = {1, -3, 0};
static float3 nB_move = {2.8, 0, 0};
static float3 nC_move = {3.2, 0, 0.3};
static float3 nD_move = {3.5, 0, 0.5};

static float3 CBC_D = {0.616, 0.686, 0.718};
static float3 CBC_N = {0.118, 0.118, 0.118};
static float3 CBC_S = {0.416, 0.502, 0.525};
static float3 CBC_CD = {0.557, 0.616, 0.659};
static float3 CBC_CN = {0.275, 0.275, 0.275};

static float3 CTC_D = {0.953, 1, 0.988};
static float3 CTC_N = {0.153, 0.125, 0.0824};
static float3 CTC_S = {1, 0.69, 0.482};
static float3 CTC_CD = {0.894, 0.949, 0.957};
static float3 CTC_CN = {0.125, 0.157, 0.173};

static float grow_h = {0.46};
static float softness_h = {0.74};
static float scatteringStrength_h = {3.0};
static float H_scaleA = {5.3};
static float H_scaleB = {1.77};
static float curvy_scale_h = {2.84};
static float curvy_ang_h = {2.0};
static float curvy_strength_h = {10.0};
static float HdensA = {0.6};
static float HdensB = {-0.3};
static float3 H_NoiseA_Direction = {-3, 3.3, 0.29};
static float3 H_NoiseB_Direction = {-5, 5, 0.234};

#define nsA		0.0005/noiseSizeA
#define nsB		0.0005/noiseSizeB
#define nsC		0.1/noiseSizeC
#define nsD		0.1/noiseSizeD
#define nsE		0.1/noiseSizeE

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
//static float randomSeed = 1618.03398875;
//
//float hash(float n)
//{
//    return frac(sin(n / 1873.1873) * randomSeed);
//}

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

//=========================================
//float noise_d(float3 p)
//{
//
//    float3 fr = floor(p),
//        ft = frac(p);
//
//    float n = 1153.0 * fr.x + 2381.0 * fr.y + fr.z,
//        nr = n + 1153.0,
//        nd = n + 2381.0,
//        no = nr + 2381.0,
//
//        v = lerp(hash(n), hash(n + 1.0), ft.z),
//        vr = lerp(hash(nr), hash(nr + 1.0), ft.z),
//        vd = lerp(hash(nd), hash(nd + 1.0), ft.z),
//        vo = lerp(hash(no), hash(no + 1.0), ft.z);
//
//    return lerp(lerp(v, vr, ft.x), lerp(vd, vo, ft.x), ft.y);
//}
//

float noise_d(const in float2 xy)
{
    return tex2Dlod(SamplerDis, float4(xy, 0.0, 0.0)).r;
}

//large chunk for the distribution of clouds.
float chunk(in float3 pos, const in float3 offsetA, const in float3 offsetB, in float cs)
{
    pos += cloud_shift * pos.z;
    float2
        pA = pos.xy + offsetA.xy,
        pB = pos.xy + offsetB.xy;

    return clamp((noise_densA * noise_d(pA * nsA) + noise_densB * noise_d(pB * nsB)) * cs, 0.0, 1.0);
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

    float dens = noise_densC * nc + noise_densD * (1.0 - noise_p((pD + d) * nsD)) + noise_densE * noise_p((pD + d / 2) * nsE);

    return dens;
}

float detial2(const in float3 pos, const in float3 offsetC)
{
    float3
        pC = pos + offsetC;

    return noise_densC * noise_p(pC * nsC) * 2.0;
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
        dens2 = detial2(p2, offsetC) * ShadowDetail,

        d = (dens1 + dens2) * shadowContrast + shadowExpand - (cs.z - shadowSoftness);
    d /= (cs.w + shadowSoftness) * 2.0;

    return 1.0 - clamp(d, 0.0, 1.0) * shadowStrength;
}

float4 Cloud_mid(const in float3 cam_dir, in float3 p, in float dist, const in float depth, float time, float moonLight, float tf, float sunny)
{
    float4 d = float4(0.0, 0.0, 0.0, min_sl); //distance, distance factor, gap, the last march step

    p = PosOnPlane(p, cam_dir, clamp(p.z, vb_b, vb_t), d.x);
    d.y = d.x;

    if(d.x >= 0.0 && d.x < fade_e)
    {
        float
            fe = lerp(fogEffect_c, fogEffect, sunny) / 100.0;

        float3
            flowA = -time * nA_move,
            flowB = -time * nB_move,
            flowC = -time * nC_move,
            flowD = -time * nD_move,

            dens_thres = float3(
                1.0 / lerp(grow_c, grow, sunny),
                soft_top,
                lerp(soft_bot_c, soft_bot, sunny)),

            fx = float3(0.5, 0.0, 1.0); //brightness, density, transparency

        float4 profile = float4(body_top, body_mid, body_bot, body_thickness);

        if(depth == 1)
            dist = fade_e;

        for(int i = 0.0; fx.z > 0.0 && p.z <= vb_t && p.z >= vb_b && i < maxStep && d.x - d.w < dist; i += 1.0)
        {
            float3 cs = cloud_shape(p.z, profile, dens_thres).xyz;

            float
                dens = (chunk(p, flowA, flowB, cs.x) + temp2) * (detial(p, flowC, flowD) + temp1) / (1 + temp2) / (1 + temp1),
                marchStep = clampMap(dens, cs.z, dCut, min_sl, max_sl) * clampMap(d.x, 0.0, 10000.0, 1.0, step_mul),
                coverage = smoothstep(cs.z, cs.y, dens) * marchStep / CloudSoftness,
                ds = fx.z + fe;

            //fade into near objects
            if(d.x >= dist)
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
            fx.z = (exp(-fx.y / BeerFactor) - tCut) / (1.0 - tCut);

            d.z = dist - d.x;

            //marching
            p += cam_dir * marchStep;
            d.x += marchStep;
            d.w = marchStep;
        }

        if(fx.y > 0.0)
        {
            float
                noon_index = smoothstep(-0.09, 0.3, tf),
                sunset_index = smoothstep(0.3, 0.1, tf) * smoothstep(-0.18, -0.08, tf),
                suncross = dot(vSunLightDir.xyz, cam_dir);

            float3
                c_dark = lerp(lerp(lerp(CBC_CN, CBC_CD, noon_index), lerp(CBC_N, CBC_D, noon_index), sunny), CBC_S, sunset_index),
                c_bright = lerp(lerp(lerp(CTC_CN, CTC_CD, noon_index), lerp(CTC_N, CTC_D, noon_index), sunny), CTC_S, sunset_index) * cloudBrightnessMultiply;

            c_bright += normalize(c_bright) * scatteringStrength * exp((suncross - 1.0) / scattering) * (smoothstep(-0.13, -0.08, tf));

            float3 C = lerp(c_dark, c_bright, smoothstep(0.0, 1.0, (fx.x - 0.5) * CloudContrast + 0.5));

            float rsf = smoothstep(Atomesphere_Distance - Atomesphere_Smoothness, Atomesphere_Distance + Atomesphere_Smoothness, d.y);
            C += Atomesphere / 100.0 * rsf;

            //fading
            fx.y = 1.0 - exp(-fx.y / alpha_curve);
            fx.y *= smoothstep(fade_e, fade_s, d.y);
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

    if(distance > 0.0 && (depth == 1.0 || distance < dist) && cam_dir.z > -0.1)
    {
        float
            alpha = 0.0,
            noon_index = smoothstep(0.0, 0.3, tf),
            sunset_index = smoothstep(0.3, 0.1, tf) * smoothstep(-0.2, -0.1, tf) * sunny,
            suncross = dot(vSunLightDir.xyz, cam_dir),
            nearMoon = moonlight_strength * smoothstep(0.9, 1.2, dot(float3(-0.12, -0.9, 0.45), cam_dir)) * moonLight;

        float3
            c_dark = lerp(lerp(lerp(CBC_CN, CBC_CD, noon_index), lerp(CBC_N, CBC_D, noon_index), sunny), CBC_S, sunset_index),
            c_bright = lerp(lerp(lerp(CTC_CN, CTC_CD, noon_index), lerp(CTC_N, CTC_D, noon_index), sunny), CTC_S, sunset_index);

        c_bright += normalize(c_bright) * scatteringStrength_h * exp((suncross - 1.0) / scattering / 1.5);

        float3 C = float4(0.0, 0.0, 0.0, 0.0),
            flowA = -time * H_NoiseA_Direction,
            flowB = -time * H_NoiseB_Direction;

        alpha = Density_h(position / 1500.0, flowA, flowB);

        //density threshold
        alpha *= smoothstep(grow_h - softness_h, grow_h + softness_h, alpha);

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

float4 main(float4 position :POSITION,  float2 texcoord : TEXCOORD) : COLOR
{
    float4 OutLighting;

    float3 ViewPos = mViewInv[3].xyz;


    float depth;
    float3 normal;
    DecodeDepthNormal(texcoord, fogData.y, depth, normal);
    
    float3 WorldPos;
    WorldPositionFromDepth(texcoord, depth, Projection, mViewInv, WorldPos);
    float4 r0 = tex2D(ScreenSampler, texcoord.xy);
    float
        ti = vSunLightDir.z,
        moon = 0.0,
        sunny = 0.9,
        dist = distance(float3(0, 0, 0), WorldPos.xyz);

    float3 direction = normalize(WorldPos.xyz);
    ViewPos = normalize(WorldPos - ViewPos);

    //clouds
    float4 highCloud = Cloud_high(direction, ViewPos.xyz, dist, depth, Time * speed, 2000.0, moon, ti, sunny);
    r0.xyz = lerp(r0.rgb, highCloud.rgb, highCloud.a);

    float4 midCloud = Cloud_mid(direction, ViewPos.xyz, dist, depth, Time * speed, moon, ti, sunny);
    r0.xyz = lerp(r0.rgb, midCloud.rgb, midCloud.a);

    return r0;
}
