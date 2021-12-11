float2 g_vSampleOffsets[15] : register(c0);
float g_fSampleWeights[15] : register(c19);
sampler Sampler : register(s0);

static const float Kernel[13] = { -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6 };
static const float Weights[13] = { 0.002216, 0.008764, 0.026995, 0.064759, 0.120985, 0.176033, 0.199471, 0.176033, 0.120985, 0.064759, 0.026995, 0.008764, 0.002216 };
static float sBloom = 1.7;
static float sBlur = 0.9f;

float4 main(float2 texCoord: TEXCOORD0) : COLOR0
{
 // 	// Accumulated color
 //   float4 vAccum = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//// Sample the taps (g_vSampleOffsets holds the texel offsets and g_fSampleWeights holds the texel weights)
 //   for (int i = 0; i < 15; i++)
 //   {
 //       vAccum += tex2D(Sampler, texCoord + g_vSampleOffsets[i]) * g_fSampleWeights[i];
 //   }
    
 //   return vAccum;

    float4 Color=0;
    float2 coord;
    coord.x = texCoord.x;

    for (int i = 0; i < 13; ++i)
    {
        coord.y = texCoord.y + (sBlur * Kernel[i]) / 1080.0f;
        Color += tex2D(Sampler, coord.xy) * Weights[i] * sBloom;
    }
    Color.a = 1;
    return Color;
}