#include "Utilities.hlsl"

sampler2D txScreen : register(s0);
sampler2D txVolumetric : register(s1);

float4 FogData : register(c12);

#define FogStart FogData.x
#define FarClip FogData.y

float4 main(float4 pos : POSITION, float2 tex : TEXCOORD0) : COLOR
{
    float4 OutLighting;
    
    float4 ScreenColor = tex2D(txScreen, tex);
    float4 clouds = tex2D(txVolumetric, tex);
	
    //Fill up the gap due to low render scale
    if (ScreenColor.a <= 0.0)
    {
        const float r = 2.5;
        float3 res_vec = 0.0;
        float3 avg_col = float3(0.0, 0.0, 0.0);
        for (float a = 0.0; a < 6.27; a += 1.57)
        {
            float2 vec = float2(cos(a), sin(a)) * r;
            float2 samp_point = tex + vec / float2(1 / 1920.0f, 1 / 1080.0f);
            float4 samp_color = tex2D(txScreen, samp_point);
            if (samp_color.a > 0.0f)
            {
                res_vec.xy -= vec;
                res_vec.z += 1.0;
				
                avg_col += samp_color.rgb;
            }
        }
		
        if (res_vec.z >= 4.0)
        {
            ScreenColor.rgb = avg_col / 4.0f;
            ScreenColor.a = 1.0;
			
            clouds = float4(0.0, 0.0, 0.0, 1.0);
        }
        else if (res_vec.z > 0.0)
        {
            res_vec.xy = normalize(res_vec.xy) * r / float2(1 / 1920.0f, 1 / 1080.0f);
            clouds = tex2D(txVolumetric, tex + res_vec.xy);
        }
    }
    
    clouds.rgb += ScreenColor.rgb * clouds.a;
    clouds.a = ScreenColor.a;
    
    return clouds;
}