float2 g_vSampleOffsets[15] : register(c0);
float g_fSampleWeights[15] : register(c19);
sampler Sampler : register(s0);

// Calculates the gaussian blur weight for a given distance and sigmas
float CalcGaussianWeight(int sampleDist, float sigma)
{
    float g = 1.0f / sqrt(2.0f * 3.14159 * sigma * sigma);
    return (g * exp(-(sampleDist * sampleDist) / (2 * sigma * sigma)));
}

// Performs a gaussian blue in one direction
float4 BlurF(float2 TexCoord, float2 texScale, float sigma)
{
    float4 color = 0;
    for (int i = -6; i < 6; i++)
    {
        float weight = CalcGaussianWeight(i, sigma);
        float2 texCoord = TexCoord;
        texCoord += ((float) i / 512.0f) * texScale;
        float4 Sample = tex2D(Sampler, texCoord);
        color += Sample * weight;
    }

    return color;
}
static const float Kernel[13] = { -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6 };
static const float Weights[13] = { 0.002216, 0.008764, 0.026995, 0.064759, 0.120985, 0.176033, 0.199471, 0.176033, 0.120985, 0.064759, 0.026995, 0.008764, 0.002216 };


float4 main(float2 texCoord: TEXCOORD0) : COLOR0
{
 //   // Accumulated color
 //   float4 vAccum = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//// Sample the taps (g_vSampleOffsets holds the texel offsets and g_fSampleWeights holds the texel weights)
 //   for (int i = 0; i < 15; i++)
 //   {
 //       vAccum += tex2D(Sampler, texCoord + g_vSampleOffsets[i]) * g_fSampleWeights[i];
 //   }
 //   return vAccum;
    float BloomBlurSigma = 0.6;
    return BlurF(texCoord, float2(0, 1), BloomBlurSigma);
    //float4 Color = 0;

    //float2 coord;
    //coord.y = texCoord.y;
    //float sBloom = 1.7;
    //float sBlur = 0.9;
    //for (int i = 0; i < 13; ++i)
    //{
    //    coord.x = texCoord.x + (sBlur * Kernel[i]) / 1920.0f;
    //    Color += tex2D(Sampler, coord.xy) * Weights[i] * sBloom;
    //}
    //return Color;
    
  //  float TextelScale = 1.0 /  1920.0;
  //  float4 Value = 0.0;
  //  float Coefficients[21] =
  //  {
  //      0.000272337, 0.00089296, 0.002583865, 0.00659813, 0.014869116,
	 //0.029570767, 0.051898313, 0.080381679, 0.109868729, 0.132526984,
	 //0.14107424,
	 //0.132526984, 0.109868729, 0.080381679, 0.051898313, 0.029570767,
	 //0.014869116, 0.00659813, 0.002583865, 0.00089296, 0.000272337
  //  };
    
  //  for (int Index = 0; Index < 21; Index++)
  //  {
  //      Value += tex2D(Sampler, float2(texCoord.x, texCoord.y + (Index - 10) * TextelScale)) * Coefficients[Index];
  //  }
  //  return Value;
}