float4 g_vSampleOffsets[15] : register(c8);

sampler Sampler : register(s4);

float4 main(float2 texCoord: TEXCOORD0) : COLOR0
{
    float vAccum = 0.0f;

    for (int i = 0; i < 15; i++)
    {
        vAccum += tex2D(Sampler, texCoord + g_vSampleOffsets[i].xy).r * g_vSampleOffsets[i].z;
    }
    return vAccum;
    
  //  float TextelScale = 1.0 / 512.0;
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
  //      Value.rg += tex2D(Sampler, float2(texCoord.x, texCoord.y + (Index - 10) * TextelScale)).rg * Coefficients[Index];
  //  }
  //  return Value;
}