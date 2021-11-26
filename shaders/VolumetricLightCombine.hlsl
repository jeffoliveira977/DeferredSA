float3 FogData : register(c12);

sampler2D ScreenSampler : register(s4);
sampler2D VolumetricSampler : register(s5);

float4 main(float2 texCoord : TEXCOORD) : COLOR
{
    float4 OutLighting;
    
    float4 ScreenColor = tex2D(ScreenSampler, texCoord);
    float4 VolumetricLight = tex2D(VolumetricSampler, texCoord);
    
    float intesity = saturate(length(VolumetricLight.rgb));
   
    OutLighting.rgb = lerp(ScreenColor.rgb, VolumetricLight.rgb, intesity * FogData.z);
    OutLighting.a = 1;

    return OutLighting;
}