static const float4 fxaaParams0 = { 0.08f, 0.16f, 0.75f, 0.25f };
static const float4 fxaaParams1 = { 4.f, 0.05f, 0.125f, 0.0f };
static float2 sPixelSize = float2(0.00125, 0.00166);

struct vtxOutFXAA
{
    float4 HPosition : POSITION0;
    float4 baseTC : TEXCOORD0;
    float4 baseTC1 : TEXCOORD1;
};

sampler2D sTex0;

float tex2Dlum(float4 texCoord)
{
    float3 texel = tex2Dlod(sTex0, texCoord).rgb;
    return max(texel.r, max(texel.g, texel.b));
}

//------------------------------------------------------------------------------------------
// PixelShaderFunction
//------------------------------------------------------------------------------------------
float4 main(vtxOutFXAA IN) : COLOR0
{
	// Pixel sizes.
    float2 vPixelSizes = sPixelSize.xy * 1.0;
	
	// Initial sample. Used on early-out.
    float4 cSampleCenter = tex2Dlod(sTex0, float4(IN.baseTC.xy, 0, 0));
    float4 OUT = cSampleCenter;

    float fLumCenter = cSampleCenter.r;
    float fLumBottom = tex2Dlum(float4(IN.baseTC.xy + float2(0, 1) * vPixelSizes.xy, 0, 0)).r;
    float fLumRight = tex2Dlum(float4(IN.baseTC.xy + float2(1, 0) * vPixelSizes.xy, 0, 0)).r;
    float fLumTop = tex2Dlum(float4(IN.baseTC.xy + float2(0, -1) * vPixelSizes.xy, 0, 0)).r;
    float fLumLeft = tex2Dlum(float4(IN.baseTC.xy + float2(-1, 0) * vPixelSizes.xy, 0, 0)).r;

    float fMaxRange = max(max(fLumTop, fLumLeft), max(fLumRight, max(fLumBottom, fLumCenter)));
    float fMinRange = min(min(fLumTop, fLumLeft), min(fLumRight, min(fLumBottom, fLumCenter)));
    float fRange = fMaxRange - fMinRange;
    
    // Early out.
    if (fRange < max(fxaaParams0.x, fMaxRange * fxaaParams0.y))
        return OUT;
		
    float fLumTopLeft = tex2Dlum(float4(IN.baseTC.xy + float2(-1, -1) * vPixelSizes.xy, 0, 0)).r;
    float fLumBottomRight = tex2Dlum(float4(IN.baseTC.xy + float2(1, 1) * vPixelSizes.xy, 0, 0)).r;
    float fLumTopRight = tex2Dlum(float4(IN.baseTC.xy + float2(1, -1) * vPixelSizes.xy, 0, 0)).r;
    float fLumBottomLeft = tex2Dlum(float4(IN.baseTC.xy + float2(-1, 1) * vPixelSizes.xy, 0, 0)).r;
    
    float fLumTopBottom = fLumTop + fLumBottom;
    float fLumLeftRight = fLumLeft + fLumRight;
    float fLumSubPixel = fLumTopBottom + fLumLeftRight;
    
    float fEdgeH1 = (-2.0 * fLumCenter) + fLumTopBottom;
    float fEdgeV1 = (-2.0 * fLumCenter) + fLumLeftRight;

    float fLumTopBottomRight = fLumTopRight + fLumBottomRight;
    float fLumTopLeftRight = fLumTopLeft + fLumTopRight;
    float fEdgeH2 = (-2.0 * fLumRight) + fLumTopBottomRight;
    float fEdgeV2 = (-2.0 * fLumTop) + fLumTopLeftRight;

    float fLumTopBottomLeft = fLumTopLeft + fLumBottomLeft;
    float fLumBottomLeftRight = fLumBottomLeft + fLumBottomRight;
    float fEdgeH4 = (abs(fEdgeH1) * 2.0) + abs(fEdgeH2);
    float fEdgeV4 = (abs(fEdgeV1) * 2.0) + abs(fEdgeV2);
    float fEdgeH3 = (-2.0 * fLumLeft) + fLumTopBottomLeft;
    float fEdgeV3 = (-2.0 * fLumBottom) + fLumBottomLeftRight;
    float fEdgeH = abs(fEdgeH3) + fEdgeH4;
    float fEdgeV = abs(fEdgeV3) + fEdgeV4;

    float fBlendSubPixel = fLumTopBottomLeft + fLumTopBottomRight;
    float fLengthSign = vPixelSizes.x;
    bool bHorizontalSpan = fEdgeH >= fEdgeV;
    float fSubPixelA = fLumSubPixel * 2.0 + fBlendSubPixel;

    if (!bHorizontalSpan)
        fLumTop = fLumLeft;
    if (!bHorizontalSpan)
        fLumBottom = fLumRight;
    if (bHorizontalSpan)
        fLengthSign = vPixelSizes.y;
    float fSubPixelB = (fSubPixelA * (1.0 / 12.0)) - fLumCenter;
        
    float fGradientN = fLumTop - fLumCenter;
    float fGradientS = fLumBottom - fLumCenter;
    float fLumTopCenter = fLumTop + fLumCenter;
    float fLumBottomCenter = fLumBottom + fLumCenter;
    bool fPairN = abs(fGradientN) >= abs(fGradientS);
    float fGradient = max(abs(fGradientN), abs(fGradientS));
    if (fPairN)
        fLengthSign = -fLengthSign;
    float fSubPixelC = saturate(abs(fSubPixelB) * (1.0 / fRange));
    
    float2 vPositionB;
    vPositionB.x = IN.baseTC.x;
    vPositionB.y = IN.baseTC.y;
    float2 vOffsetNP;
    vOffsetNP.x = (!bHorizontalSpan) ? 0.0 : vPixelSizes.x;
    vOffsetNP.y = (bHorizontalSpan) ? 0.0 : vPixelSizes.y;
    if (!bHorizontalSpan)
        vPositionB.x += fLengthSign * 0.5;
    if (bHorizontalSpan)
        vPositionB.y += fLengthSign * 0.5;
    
    float2 vPositionN;
    vPositionN.x = vPositionB.x - vOffsetNP.x;
    vPositionN.y = vPositionB.y - vOffsetNP.y;
    
    float2 vPositionP;
    vPositionP.x = vPositionB.x + vOffsetNP.x;
    vPositionP.y = vPositionB.y + vOffsetNP.y;
    
    float fSubPixelD = ((-2.0) * fSubPixelC) + 3.0;
    float fLumEndN = tex2Dlum(float4(vPositionN, 0, 0)).r;
    float fLumEndP = tex2Dlum(float4(vPositionP, 0, 0)).r;
    
    float fSubPixelE = (fSubPixelC * fSubPixelC);
    
    if (!fPairN)
        fLumTopCenter = fLumBottomCenter;
    float fGradientScaled = fGradient * 1.0 / 4.0;
    float fSubPixelF = fSubPixelD * fSubPixelE;
    bool bLumZero = (fLumCenter - fLumTopCenter * 0.5) < 0.0;
    
    fLumEndN -= fLumTopCenter * 0.5;
    fLumEndP -= fLumTopCenter * 0.5;
    bool bDoneN = abs(fLumEndN) >= fGradientScaled;
    bool bDoneP = abs(fLumEndP) >= fGradientScaled;
    if (!bDoneN)
        vPositionN.x -= vOffsetNP.x;
    if (!bDoneN)
        vPositionN.y -= vOffsetNP.y;
    bool bDoneNP = (!bDoneN) || (!bDoneP);
    if (!bDoneP)
        vPositionP.x += vOffsetNP.x;
    if (!bDoneP)
        vPositionP.y += vOffsetNP.y;
    
    static const half fSearchScale[11] = { 1.0, 1.0, 1.0, 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0 };


    // Search edges.
    for (int i = 0; i < 11; i++)
    {
        if (!bDoneN)
            fLumEndN = tex2Dlum(float4(vPositionN.xy, 0, 0)).r;
        if (!bDoneP)
            fLumEndP = tex2Dlum(float4(vPositionP.xy, 0, 0)).r;
        if (!bDoneN)
            fLumEndN = fLumEndN - fLumTopCenter * 0.5;
        if (!bDoneP)
            fLumEndP = fLumEndP - fLumTopCenter * 0.5;
        bDoneN = abs(fLumEndN) >= fGradientScaled;
        bDoneP = abs(fLumEndP) >= fGradientScaled;
        if (!bDoneN)
            vPositionN.x -= vOffsetNP.x * fSearchScale[i];
        if (!bDoneN)
            vPositionN.y -= vOffsetNP.y * fSearchScale[i];
        bDoneNP = (!bDoneN) || (!bDoneP);
        if (!bDoneP)
            vPositionP.x += vOffsetNP.x * fSearchScale[i];
        if (!bDoneP)
            vPositionP.y += vOffsetNP.y * fSearchScale[i];
    }
                    
    float fDestN = IN.baseTC.x - vPositionN.x;
    float fDestP = vPositionP.x - IN.baseTC.x;
    
    if (!bHorizontalSpan)
        fDestN = IN.baseTC.y - vPositionN.y;
    if (!bHorizontalSpan)
        fDestP = vPositionP.y - IN.baseTC.y;
    
    float fSpanLength = (fDestP + fDestN);
    bool bGoodSpanN = (fLumEndN < 0.0) != bLumZero;
    bool bGoodSpanP = (fLumEndP < 0.0) != bLumZero;

    bool bDirectionN = fDestN < fDestP;
    float fDest = min(fDestN, fDestP);
    bool bGoodSpan = bDirectionN ? bGoodSpanN : bGoodSpanP;
    float fSubPixelG = fSubPixelF * fSubPixelF;
    float fPixelOffset = (fDest * (-(1.0 / fSpanLength))) + 0.5;
    float fSubPixelH = fSubPixelG * fxaaParams0.z;

    float fPixelOffsetGood = bGoodSpan ? fPixelOffset : 0.0;
    float fPixelOffsetSubpix = max(fPixelOffsetGood, fSubPixelH);
    if (!bHorizontalSpan)
        IN.baseTC.x += fPixelOffsetSubpix * fLengthSign;
    if (bHorizontalSpan)
        IN.baseTC.y += fPixelOffsetSubpix * fLengthSign;
    
    OUT = tex2Dlod(sTex0, float4(IN.baseTC.xy, 0, 0));

    return OUT;
}
 