sampler2D SamplerLightTarget : register(s0);
sampler2D SamplerAlbedo : register(s1);
sampler2D SamplerDepthNormal : register(s2);
sampler2D SamplerMaterialProps : register(s3);

#define TEXTURE2D_ALBEDO(uv)         tex2D(SamplerAlbedo, uv)
#define TEXTURE2D_DEPTHNORMAL(uv)    tex2D(SamplerDepthNormal, uv)
#define TEXTURE2D_DEPTH(uv)          tex2D(SamplerDepthNormal, uv).zw
#define TEXTURE2D_NORMAL(uv)         tex2D(SamplerDepthNormal, uv).xy
#define TEXTURE2D_MATERIALPROPS(uv)  tex2D(SamplerMaterialProps, uv)
#define TEXTURE2D_LIGHTTARGET(uv)    tex2D(SamplerLightTarget, uv)

#define PI 3.141592653589793f
#define TAU PI * 2.0f
#define EULER 2.718281828459f

#define RGBVALUE(r, g, b) float3(r / 255.0, g / 255.0, b / 255.0)
#define HSVVALUE(h, s, v) float3(h / 360.0, s / 100.0, v / 100.0)

#define mod(x, y) (x - y * floor(x / y))
#define swap(T, a, b)  T tmp = a; a = b; b = tmp;

#define BiasD(x) (0.5f * x + 0.5f)
#define BiasX(x) (2.0f * x - 1.0f)

#define vec2 float2
#define vec3 float3
#define vec4 float4
#define mat2 float2x2
#define mat3 float3x3
#define mat4 float4x4
#define mix lerp
#define fract frac


float2 CalculateScreenPosition(float4 position)
{
    return BiasD(position.xy / position.w * float2(0.5, -0.5) + 0.5);
}

float4 ClipToScreenPos(float4 pos)
{
    float4 o = pos * 0.5f;
    o.xy += o.w;
    o.zw = pos.zw;
    return o;
}

#define REVERSED_Z 0

float4 NdcToClipPos(float3 ndc)
{
	// map xy to -1,1
    float4 clipPos = float4(ndc.xy * 2.0f - 1.0f, ndc.z, 1.0f);

#if REVERSED_Z
	//D3d with reversed Z
	clipPos.z = 1.0f - clipPos.z;
#endif

    return clipPos;
}

float3 NdcToWorldPos(float4x4 inverseVP, float3 ndc)
{
    float4 clipPos = NdcToClipPos(ndc);
    float4 pos = mul(inverseVP, clipPos);
    pos.xyz /= pos.w;

    return pos.xyz;
}


//from unity: https://forum.unity.com/threads/sobel-operator-height-to-normal-map-on-gpu.33159/
float3x3 img3x3(sampler2D color_map, float2 tc, float ts, int ch)
{
    float d = 1.0 / ts; // ts, texture sampling size
    float3x3 c;
    c[0][0] = tex2D(color_map, tc + float2(-d, -d))[ch];
    c[0][1] = tex2D(color_map, tc + float2(0, -d))[ch];
    c[0][2] = tex2D(color_map, tc + float2(d, -d))[ch];
       
    c[1][0] = tex2D(color_map, tc + float2(-d, 0))[ch];
    c[1][1] = tex2D(color_map, tc)[ch];
    c[1][2] = tex2D(color_map, tc + float2(d, 0))[ch];
       
    c[2][0] = tex2D(color_map, tc + float2(-d, d))[ch];
    c[2][1] = tex2D(color_map, tc + float2(0, d))[ch];
    c[2][2] = tex2D(color_map, tc + float2(d, d))[ch];
   
    return c;
}

float3 height2normal_sobel(float3x3 c)
{
    float3x3 x = float3x3(1.0, 0.0, -1.0,
	 		              2.0, 0.0, -2.0,
			              1.0, 0.0, -1.0);
	 
    float3x3 y = float3x3(1.0, 2.0, 1.0,
			              0.0, 0.0, 0.0,
		 	             -1.0, -2.0, -1.0);
		
    x = x * c;
    y = y * c;
		 
    float cx = x[0][0] + x[0][2] +
			   x[1][0] + x[1][2] +
			   x[2][0] + x[2][2];
		
    float cy = y[0][0] + y[0][1] + y[0][2] +
			   y[2][0] + y[2][1] + y[2][2];
		
    float cz = sqrt(1 - (cx * cx + cy * cy));
		
    return float3(cx, cy, cz);
}

// Christian Schuler, "Normal Mapping without Precomputed Tangents", ShaderX 5, Chapter 2.6, pp. 131-140
// See also follow-up blog post: http://www.thetenthplanet.de/archives/1180
inline float3x3 CalculateTBN(float3 p, float3 n, float2 tex)
{
    float3 dp1 = ddx(p);
    float3 dp2 = ddy(p);
    float2 duv1 = ddx(tex);
    float2 duv2 = ddy(tex);

    float3x3 M = float3x3(dp1, dp2, cross(dp1, dp2));
    float2x3 inverseM = float2x3(cross(M[1], M[2]), cross(M[2], M[0]));
    float3 t = normalize(mul(float2(duv1.x, duv2.x), inverseM));
    float3 b = normalize(mul(float2(duv1.y, duv2.y), inverseM));
    return float3x3(t, b, n);
}

inline float3 PeturbNormal(float3 localNormal, float3 position, float3 normal, float2 texCoord)
{
    const float3x3 TBN = CalculateTBN(position, normal, texCoord);
    return normalize(mul(localNormal, TBN));
}

// Calculates UV offset for parallax bump mapping
float2 ParallaxOffset(float h, float height, float3 viewDir)
{
    h = h * height - height / 2.0;
    float3 v = normalize(viewDir);
    v.z += 0.42;
    return h * (v.xy / v.z);
}

inline float3 TwoChannelNormalX2(float2 normal)
{
    float3 n;
    n.xy = normal * 2 - 1;
    n.z = sqrt(max(1 - dot(n.xy, n.xy), 0.0f));
    return n;
}

//from unity: https://github.com/TwoTailsGames/Unity-Built-in-Shaders/blob/master/CGIncludes/UnityCG.cginc
inline float DecodeFloatRG(float2 enc)
{
    float2 kDecodeDot = float2(1.0, 1 / 255.0);
    return dot(enc, kDecodeDot);
}

inline float DecodeFloatRG(float2 enc, float farPlane)
{
    float2 kDecodeDot = float2(1.0, 1 / 255.0);
    float res = dot(enc, kDecodeDot);
    return res * farPlane;
}

inline float2 EncodeFloatRG(float v)
{
    float2 kEncodeMul = float2(1.0, 255.0);
    float kEncodeBit = 1.0 / 255.0;
    float2 enc = kEncodeMul * v;
    enc = frac(enc);
    enc.x -= enc.y * kEncodeBit;
    return enc;
}
// Encoding/decoding [0..1) floats into 8 bit/channel RGBA. Note that 1.0 will not be encoded properly.
inline float4 EncodeFloatRGBA(float v)
{
    float4 kEncodeMul = float4(1.0, 255.0, 65025.0, 16581375.0);
    float kEncodeBit = 1.0 / 255.0;
    float4 enc = kEncodeMul * v;
    enc = frac(enc);
    enc -= enc.yzww * kEncodeBit;
    return enc;
}
inline float DecodeFloatRGBA(float4 enc)
{
    float4 kDecodeDot = float4(1.0, 1 / 255.0, 1 / 65025.0, 1 / 16581375.0);
    return dot(enc, kDecodeDot);
}
float4 EncodeFloatRGB(float f)
{
    float4 color;
    f *= 256;
    color.x = floor(f);
    f = (f - color.x) * 256;
    color.y = floor(f);
    f = (f - color.y) * 256;
    color.z = floor(f);
    color.w = f - color.z;
    color.xyz *= 0.00390625; // *= 1.0/256
    return color;
}

float DecodeFloatRGB(float4 color)
{
    const float4 byte_to_float = float4(1.0, 1.0 / 256, 1.0 / (256 * 256), 1.0 / (256 * 256 * 256));
    return dot(color, byte_to_float);
}

inline float DecodeDepth(float2 texCoord, float farPlane)
{
    float2 depth = TEXTURE2D_DEPTH(texCoord);
    depth.x = DecodeFloatRG(depth);
    depth.x = depth.x < 0.0 ? 1.0 : depth.x;
    return depth.x * farPlane;
}

inline float DecodeDepth(sampler2D samplerDepth, float2 texCoord)
{
    float2 depth = tex2D(samplerDepth, texCoord).zw;
    depth.x = DecodeFloatRG(depth);
    return depth.x;
}

inline void DecodeDepthNormal(sampler2D samplerDepth, float2 texCoord, float farPlane, out float depth, out float3 normal)
{
    float4 depthNormal = tex2D(samplerDepth, texCoord);
    normal = TwoChannelNormalX2(depthNormal.xy);
    depth = DecodeFloatRG(depthNormal.zw, farPlane);
    depth = depth <= 0 ? farPlane : depth;
}

inline void DecodeDepthNormal(float2 texCoord, float farPlane, out float depth, out float3 normal)
{
    float4 depthNormal = TEXTURE2D_DEPTHNORMAL(texCoord);
    normal = TwoChannelNormalX2(depthNormal.xy);
    depth = DecodeFloatRG(depthNormal.zw, farPlane);
    depth = depth <= 0 ? farPlane : depth;
}

inline float4 EncodeDepthNormal(float depth, float3 normal)
{
    return float4(BiasD(normal.xy), EncodeFloatRG(depth));
}

inline void WorldPositionFromDepth(float2 texCoord, float depth, float2 scale, float4x4 inverseView, out float3 worldPosition)
{
    float4 clipSpace = float4(texCoord * 2.0 - 1.0, depth, 1.0);
    clipSpace.y = -clipSpace.y;
    clipSpace.xy *= 1 / scale.xy * depth;

    worldPosition = mul(clipSpace, inverseView).xyz;
}

inline void WorldPositionFromDepth(float2 texCoord, float depth, float4x4 projection, float4x4 inverseView, out float3 worldPosition)
{
    float2 scale = float2(projection[0][0], projection[1][1]);
    WorldPositionFromDepth(texCoord, depth, scale, inverseView, worldPosition);
}

float4 DepthToViewPos(float depth, float2 texCoord, float4x4 projection)
{
    float x = texCoord.x * 2 - 1;
    float y = (1 - texCoord.y) * 2 - 1;
    float2 screenSpaceRay = float2(x / projection[0].x, y / projection[1].y);
    float4 pos = float4(screenSpaceRay * depth, depth, 1.0);
    return pos;
}

float3 DepthToWorldPos(float depth, float2 texCoord, float4x4 projection, float4x4 viewInverse)
{
    float4 pos = DepthToViewPos(depth, texCoord, projection);
    pos = mul(pos, viewInverse);
    return pos.xyz;
}

float Fresnel(float3 normal, float3 view, float bias, float scale, float power)
{
    float R = bias + scale * pow(1.0 + dot(view, normal), power);
    return R;
}

void fresnel(float3 I, float3 N, float ior, out float kr)
{
    float cosi = clamp(-1, 1, dot(I, N));
    float etai = 1, etat = ior;
    if (cosi > 0)
    {
        swap(float, etai, etat);
    }
    
    // Compute sini using Snell's law
    float sint = etai / etat * sqrt(max(0.f, 1 - cosi * cosi));
    
    // Total internal reflection
    if (sint >= 1)
    {
        kr = 1;
    }
    else
    {
        float cost = sqrt(max(0.f, 1 - sint * sint));
        cosi = abs(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        kr = (Rs * Rs + Rp * Rp) / 2;
    }
}

//-------------------------------------------------------------------------------------------------
// Helper for computing the GGX visibility term
//-------------------------------------------------------------------------------------------------
float GGX_V1(in float m2, in float nDotX)
{
    return 1.0f / (nDotX + sqrt(m2 + (1 - m2) * nDotX * nDotX));
}

//-------------------------------------------------------------------------------------------------
// Computes the GGX visibility term
//-------------------------------------------------------------------------------------------------
float GGXVisibility(in float m2, in float nDotL, in float nDotV)
{
    return GGX_V1(m2, nDotL) * GGX_V1(m2, nDotV);
}

//-------------------------------------------------------------------------------------------------
// Computes the specular term using a GGX microfacet distribution, with a matching
// geometry factor and visibility term. Based on "Microfacet Models for Refraction Through
// Rough Surfaces" [Walter 07]. m is roughness, n is the surface normal, h is the half vector,
// l is the direction to the light source, and specAlbedo is the RGB specular albedo
//-------------------------------------------------------------------------------------------------
float GGX_Specular(in float m, in float3 n, in float3 h, in float3 v, in float3 l)
{
    float nDotH = saturate(dot(n, h));
    float nDotL = saturate(dot(n, l));
    float nDotV = saturate(dot(n, v));

    float nDotH2 = nDotH * nDotH;
    float m2 = m * m;

    // Calculate the distribution term
    float d = m2 / (PI * pow(nDotH * nDotH * (m2 - 1) + 1, 2.0f));

    // Calculate the matching visibility term
    float v1i = GGX_V1(m2, nDotL);
    float v1o = GGX_V1(m2, nDotV);
    float vis = v1i * v1o;

    return d * GGXVisibility(m2, nDotL, nDotV);
} 

//-------------------------------------------------------------------------------------------------
// Helper for computing the Beckmann geometry term
//-------------------------------------------------------------------------------------------------
float Beckmann_G1(float m, float nDotX)
{
    float nDotX2 = nDotX * nDotX;
    float tanTheta = sqrt((1 - nDotX2) / nDotX2);
    float a = 1.0f / (m * tanTheta);
    float a2 = a * a;

    float g = 1.0f;
    if (a < 1.6f)
        g *= (3.535f * a + 2.181f * a2) / (1.0f + 2.276f * a + 2.577f * a2);

    return g;
}

//-------------------------------------------------------------------------------------------------
// Computes the specular term using a Beckmann microfacet distribution, with a matching
// geometry factor and visibility term. Based on "Microfacet Models for Refraction Through
// Rough Surfaces" [Walter 07]. m is roughness, n is the surface normal, h is the half vector,
// l is the direction to the light source, and specAlbedo is the RGB specular albedo
//-------------------------------------------------------------------------------------------------
float Beckmann_Specular(in float m, in float3 n, in float3 h, in float3 v, in float3 l)
{
    float nDotH = max(dot(n, h), 0.0001f);
    float nDotL = saturate(dot(n, l));
    float nDotV = max(dot(n, v), 0.0001f);

    float nDotH2 = nDotH * nDotH;
    float nDotH4 = nDotH2 * nDotH2;
    float m2 = m * m;

    // Calculate the distribution term
    float tanTheta2 = (1 - nDotH2) / nDotH2;
    float expTerm = exp(-tanTheta2 / m2);
    float d = expTerm / (PI * m2 * nDotH4);

    // Calculate the matching geometric term
    float g1i = Beckmann_G1(m, nDotL);
    float g1o = Beckmann_G1(m, nDotV);
    float g = g1i * g1o;

    return d * g * (1.0f / (4.0f * nDotL * nDotV));
}

float3 DistanceBetweenPoints(float3 a, float3 b)
{
    return sqrt(pow(a, 2) + pow(b, 2) * 1.0);
}

inline float3 UnpackNormalDXT5nm(float4 packednormal)
{
    float3 normal;
    normal.xy = packednormal.wy * 2 - 1;
    normal.z = sqrt(1 - saturate(dot(normal.xy, normal.xy)));
    return normal;
}

// Unpack normal as DXT5nm (1, y, 1, x) or BC5 (x, y, 0, 1)
// Note neutral texture like "bump" is (0, 0, 1, 1) to work with both plain RGB normal and DXT5nm/BC5
float3 UnpackNormalmapRGorAG(float4 packednormal)
{
    // This do the trick
    packednormal.x *= packednormal.w;

    float3 normal;
    normal.xy = packednormal.xy * 2 - 1;
    normal.z = sqrt(1 - saturate(dot(normal.xy, normal.xy)));
    return normal;
}
inline float3 UnpackNormal(float4 packednormal)
{
#if defined(UNITY_NO_DXT5nm)
    return packednormal.xyz * 2 - 1;
#else
    return UnpackNormalmapRGorAG(packednormal);
#endif
}

float smootherstep(float x)
{
    x = saturate(x);
    return saturate(x * x * x * (x * (6 * x - 15) + 10));
}
 
float4x4 inverseMatrix(float4x4 input)
{
#define minor(a,b,c) determinant(float3x3(input.a, input.b, input.c))
     
    float4x4 cofactors = float4x4(
          minor(_22_23_24, _32_33_34, _42_43_44),
         -minor(_21_23_24, _31_33_34, _41_43_44),
          minor(_21_22_24, _31_32_34, _41_42_44),
         -minor(_21_22_23, _31_32_33, _41_42_43),
         
         -minor(_12_13_14, _32_33_34, _42_43_44),
          minor(_11_13_14, _31_33_34, _41_43_44),
         -minor(_11_12_14, _31_32_34, _41_42_44),
          minor(_11_12_13, _31_32_33, _41_42_43),
         
          minor(_12_13_14, _22_23_24, _42_43_44),
         -minor(_11_13_14, _21_23_24, _41_43_44),
          minor(_11_12_14, _21_22_24, _41_42_44),
         -minor(_11_12_13, _21_22_23, _41_42_43),
         
         -minor(_12_13_14, _22_23_24, _32_33_34),
          minor(_11_13_14, _21_23_24, _31_33_34),
         -minor(_11_12_14, _21_22_24, _31_32_34),
          minor(_11_12_13, _21_22_23, _31_32_33)
     );
#undef minor
    return transpose(cofactors) / determinant(input);
}


float3 mod289(float3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
float2 mod289(float2 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
float3 permute(float3 x)
{
    return mod289(((x * 34.0) + 1.0) * x);
}

float snoise(float2 v)
{
	// Precompute values for skewed triangular grid
    const float4 C = float4(0.211324865405187, // (3.0-sqrt(3.0))/6.0
		0.366025403784439, // 0.5*(sqrt(3.0)-1.0)
		-0.577350269189626, // -1.0 + 2.0 * C.x
		0.024390243902439 // 1.0 / 41.0
		);


	// First corner (x0)
    float2 i = floor(v + dot(v, C.yy));
    float2 x0 = v - i + dot(i, C.xx);

	// Other two corners (x1, x2)
    float2 i1;
    i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
    float2 x1 = x0.xy + C.xx - i1;
    float2 x2 = x0.xy + C.zz;

	// Do some permutations to avoid
	// truncation effects in permutation
    i = mod289(i);
    float3 p = permute(
		permute(i.y + float3(0.0, i1.y, 1.0))
		+ i.x + float3(0.0, i1.x, 1.0));

    float3 m = max(0.5 - float3(
		dot(x0, x0),
		dot(x1, x1),
		dot(x2, x2)
		), 0.0);

    m = m * m;
    m = m * m;

	// Gradients: 
	//  41 pts uniformly over a line, mapped onto a diamond
	//  The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

    float3 x = 2.0 * frac(p * C.www) - 1.0;
    float3 h = abs(x) - 0.5;
    float3 ox = floor(x + 0.5);
    float3 a0 = x - ox;

	// Normalise gradients implicitly by scaling m
	// Approximation of: m *= inversesqrt(a0*a0 + h*h);
    m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);

	// Compute final noise value at P
    float3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * float2(x1.x, x2.x) + h.yz * float2(x1.y, x2.y);
    return 130.0 * dot(m, g);
}

static float2 fInverseViewportDimensions = float2(1. / 1920.0, 1. / 1080.0);

float2 CalculateVPos(float4 p)
{
    return float2((0.5 * (float2(p.x + p.w, p.w - p.y) + p.w * fInverseViewportDimensions.xy)+0.5) / p.w);
}
float2 GetUnsizedVPos(float2 VPos)
{
    return float2((VPos - (fInverseViewportDimensions * 0.5f)) / fInverseViewportDimensions);
}


float3 LambertLighting(float3 lightNormal, float3 surfaceNormal, float3 lightColor)
{
    // compute amount of contribution per light
    float diffuseAmount = saturate(dot(lightNormal, surfaceNormal));
    float3 diffuse = diffuseAmount * lightColor;
    return diffuse;
}


//void CalculateDiffuseTerm(in float3 vNormal, in float3 vLightDir, out float fDiffuseTerm, in float fRoughness)
//{
//    float fLambert = LambertDiffuse(vNormal, vLightDir);
//    fDiffuseTerm = fLambert;
//}

float PhongSpecular(in float3 vNormal, in float3 vLightDir, in float3 vViewDir, in float fRoughness)
{
    float specPower = pow(2.0f, saturate(1.0f - fRoughness) * 12.0f);
	
    float3 vReflectDir = reflect(vLightDir, vNormal); // reflection vector
    float fCosPhi = max(dot(vReflectDir, vViewDir), 0.0); // cosine of angle between reflection vector and viewer direction
	
    return pow(fCosPhi, specPower / 4.0);
}
float3 BlinnPhongSpecular(float3 lightDir, float3 viewDir, float3 normal, float shininess)
{
    //shininess = pow(2.0, shininess * 100);
    float3 halfDir = normalize(viewDir + lightDir);
    float specDot = max(dot(halfDir, normal), 0.0);
    return pow(specDot, shininess);
}
