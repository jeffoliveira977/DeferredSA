sampler2D Sampler : register(s0);

float4 main(float2 texCoord : TEXCOORD0) : COLOR0
{
    return tex2D(Sampler, texCoord);
}