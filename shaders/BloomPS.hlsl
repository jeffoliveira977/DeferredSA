sampler2D Scene : register(s0);
static float sCutoff = 0.08; 
static float sPower = 1.28;

// Uses a lower exposure to produce a value suitable for a bloom pass
float4 main(float2 texcoord : TEXCOORD0) : COLOR0
{
    float4 Color = 0;

    float4 texel = tex2D(Scene, texcoord);

    float lum = (texel.r + texel.g + texel.b) / 3;

    float adj = saturate(lum - sCutoff);

    adj = adj / (1.01 - sCutoff);
    
    texel = texel * adj;
    texel = pow(texel, sPower);

    Color = texel;
    float brightness = dot(texel.rgb, float3(0.2126, 0.7152, 0.0722));
    Color = brightness * texel;
    Color.a = 1;
    return Color;

}
