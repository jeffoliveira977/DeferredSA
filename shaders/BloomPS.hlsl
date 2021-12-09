sampler2D InputTextureA : register(s0);
sampler2D Scene : register(s1);
static float BloomThreshold = 10.1;
static float BloomMagnitude = 0.5;
static float BloomBlurSigma = 0.5;
static float KeyValue = 0.2;
static float WhiteLevel = 10.5;
static float LuminanceSaturation = 1;
static float Exposure = 11.0f;
static float Bias = 1.5f;
static float Tau = 1.25f;
static float ShoulderStrength = 0.22f;
static float LinearStrength = 0.3f;
static float LinearAngle = 0.1f;
static float ToeStrength = 1.2f;
static float ToeNumerator = 0.01f;
static float ToeDenominator = 0.3f;
static float LinearWhite = 1.2f;
static float LumMapMipLevel = 11.0f;
#ifndef AUTO_EXPOSURE
#define AUTO_EXPOSURE 2
#endif
#ifndef TONEMAPPING_TYPE
#define TONEMAPPING_TYPE 6
#endif

// Applies the filmic curve from John Hable's presentation
float3 ToneMapFilmicALU(float3 color)
{
    color = max(0, color - 0.004f);
    color = (color * (6.2f * color + 0.5f)) / (color * (6.2f * color + 1.7f) + 0.06f);

	// result has 1/2.2 baked in
    return pow(color, 2.2f);
}

// Determines the color based on exposure settings
float3 CalcExposedColor(float3 color, float avgLuminance, float threshold, out float exposure)
{
    exposure = 0;

#if (AUTO_EXPOSURE >= 1) && (AUTO_EXPOSURE <= 2)
	// Use geometric mean        
    avgLuminance = max(avgLuminance, 0.001f);

    float keyValue = 0;
#if AUTO_EXPOSURE == 1
    keyValue = KeyValue;
#elif AUTO_EXPOSURE == 2
	keyValue = 1.03f - (2.0f / (2 + log10(avgLuminance + 1)));
#endif
    float linearExposure = (keyValue / avgLuminance);
    exposure = log2(max(linearExposure, 0.0001f));
#else
    exposure = Exposure;
#endif

    exposure -= threshold;
    return exp2(exposure) * color;
}

// Approximates luminance from an RGB value
float CalcLuminance(float3 color)
{
    return max(dot(color, float3(0.299f, 0.587f, 0.114f)), 0.001f);
}
// Retrieves the log-average lumanaince from the texture
float GetAvgLuminance(sampler2D lumTex, float2 texCoord)
{
    return exp(tex2Dlod(lumTex, float4(texCoord, 0, LumMapMipLevel)).x);
}
// Logarithmic mapping
float3 ToneMapLogarithmic(float3 color)
{
    float pixelLuminance = CalcLuminance(color);
    float toneMappedLuminance = log10(1 + pixelLuminance) / log10(1 + WhiteLevel);
    return toneMappedLuminance * pow(max(color / pixelLuminance, 0.0), LuminanceSaturation);
}
// Drago's Logarithmic mapping
float3 ToneMapDragoLogarithmic(float3 color)
{
    float pixelLuminance = CalcLuminance(color);
    float toneMappedLuminance = log10(1 + pixelLuminance);
    toneMappedLuminance /= log10(1 + WhiteLevel);
    toneMappedLuminance /= log10(2 + 8 * ((pixelLuminance / WhiteLevel) * log10(Bias) / log10(0.5f)));
    return toneMappedLuminance * pow(max(color / pixelLuminance, 0.0), LuminanceSaturation);
}
// Exponential mapping
float3 ToneMapExponential(float3 color)
{
    float pixelLuminance = CalcLuminance(color);
    float toneMappedLuminance = 1 - exp(-pixelLuminance / WhiteLevel);
    return toneMappedLuminance * pow(max(color / pixelLuminance, 0.0), LuminanceSaturation);
}
// Applies Reinhard's basic tone mapping operator
float3 ToneMapReinhard(float3 color)
{
    float pixelLuminance = CalcLuminance(color);
    float toneMappedLuminance = pixelLuminance / (pixelLuminance + 1);
    return toneMappedLuminance * pow(color / pixelLuminance, LuminanceSaturation);
}

// Applies Reinhard's modified tone mapping operator
float3 ToneMapReinhardModified(float3 color)
{
    float pixelLuminance = CalcLuminance(color);
    float toneMappedLuminance = pixelLuminance * (1.0f + pixelLuminance / (WhiteLevel * WhiteLevel)) / (1.0f + pixelLuminance);
    return toneMappedLuminance * pow(color / pixelLuminance, LuminanceSaturation);
}


// Function used by the Uncharte2D tone mapping curve
float3 U2Func(float3 x)
{
    float A = ShoulderStrength;
    float B = LinearStrength;
    float C = LinearAngle;
    float D = ToeStrength;
    float E = ToeNumerator;
    float F = ToeDenominator;
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

// Applies the Uncharted 2 filmic tone mapping curve
float3 ToneMapFilmicU2(float3 color)
{
    float3 numerator = U2Func(color);
    float3 denominator = U2Func(LinearWhite);

    return numerator / denominator;
}
// Applies exposure and tone mapping to the specific color, and applies
// the threshold to the exposure value. 
float3 ToneMap(float3 color, float avgLuminance, float threshold, out float exposure)
{
    float pixelLuminance = CalcLuminance(color);
    color = CalcExposedColor(color, avgLuminance, threshold, exposure);
#if TONEMAPPING_TYPE == 0
#elif TONEMAPPING_TYPE == 1
	color = ToneMapLogarithmic(color);
#elif TONEMAPPING_TYPE == 2
	color = ToneMapExponential(color);
#elif TONEMAPPING_TYPE == 3
	color = ToneMapDragoLogarithmic(color);
#elif TONEMAPPING_TYPE == 4
	color = ToneMapReinhard(color);
#elif TONEMAPPING_TYPE == 5
	color = ToneMapReinhardModified(color);
#elif TONEMAPPING_TYPE == 6
    color = ToneMapFilmicALU(color);
#elif TONEMAPPING_TYPE == 7
	color = ToneMapFilmicU2(color);
#endif
    return color;
}


// Uses a lower exposure to produce a value suitable for a bloom pass
float4 main(float2 texcoord : TEXCOORD0) : COLOR0
{
    float3 color = 0;

    color = tex2D(InputTextureA, texcoord).rgb;

	// Tone map it to threshold
    float avgLuminance = GetAvgLuminance(InputTextureA, texcoord);
    float exposure = 0;
    color = ToneMap(color, avgLuminance, BloomThreshold, exposure);

    return float4(color, 1);
    //float4 Color = 0;
    //float sCutoff = 0.08;
    //float sPower = 1.88;
    //float4 texel = tex2D(InputTextureA, texcoord);

    //float lum = (texel.r + texel.g + texel.b) / 3;

    //float adj = saturate(lum - sCutoff);

    //adj = adj / (1.01 - sCutoff);
    
    //texel = texel * adj;
    //texel = pow(texel, sPower);

    //Color = texel;

    //Color.a = 1;
    //return Color;

}
