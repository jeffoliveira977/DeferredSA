row_major float4x4 gWorldViewProjection : register(c0);

static float2 sPixelSize = float2(0.00125, 0.00166);

struct vtxInFXAA
{
    float4 Position : POSITION0;
    float4 baseTC : TEXCOORD0;
};


struct vtxOutFXAA
{
    float4 HPosition : POSITION0;
    float4 baseTC : TEXCOORD0;
    float4 baseTC1 : TEXCOORD1;
};


vtxOutFXAA main(vtxInFXAA IN)
{
    vtxOutFXAA OUT = (vtxOutFXAA) 0;

    float4 Pos = float4(IN.Position.xyz, 1);
    Pos.xy *= 0.125;
    OUT.HPosition = mul(Pos, gWorldViewProjection);
    OUT.baseTC.xy = IN.baseTC.xy;
  
    // Output with subpixel offset into wz
    OUT.baseTC1.xy = IN.baseTC.xy - 0.5 * sPixelSize.xy;
    OUT.baseTC1.zw = IN.baseTC.xy + 0.5 * sPixelSize.xy;

    return OUT;
}
