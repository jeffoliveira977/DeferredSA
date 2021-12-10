sampler2D SrcColor : register(s0);
sampler2D Scene : register(s1);

float4 main(float2 Tex : TEXCOORD0) : COLOR0
{
    float3 ColorOrig = tex2D(Scene, Tex);

    ColorOrig += tex2D(SrcColor, Tex);

    return float4(ColorOrig, 1.0f);
    float exposure = 1.00;
    const float gamma = 1.1;
    float3 hdrColor = tex2D(Scene, Tex);
    float3 bloomColor = tex2D(SrcColor, Tex);
    hdrColor += bloomColor; // additive blending
    
    // tone mapping
    float3 result = 1.0f - exp(-hdrColor * exposure);
   
    // also gamma correct while we're at it       
    result = pow(result, 1.0f / gamma);
    return float4(result, 1.0);
}

