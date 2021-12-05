#define PI 3.14

inline float pow5(float v)
{
    return v * v * v * v * v;
}

inline float LambertDiffuse(in float3 vNormal, in float3 vLightDir)
{
    float fCosA = dot(vNormal, vLightDir); // cosine of angle between normal and light vector
    return max(fCosA, 0.0f);
}

float MicrofacetNDF_GGX(in float3 vNormal, in float3 vHalfWay, in float fRoughness)
{
    float fRoughnessSqr = fRoughness * fRoughness;

    float fCosAlphaSqr = saturate(dot(vNormal, vHalfWay)); // Alpha is angle between normal and half-way vector
	//fCosAlphaSqr *= fCosAlphaSqr;		// need to check if can be optimized to use less instructions
    float fDiv = ((fRoughnessSqr - 1) * fCosAlphaSqr + 1);
    float fNDCoeff = fRoughnessSqr;
    fNDCoeff /= PI * (fDiv * fDiv + 1e-7f);

    half d = (fCosAlphaSqr * fRoughnessSqr - fCosAlphaSqr) * fCosAlphaSqr + 1.0f; // 2 mad
    return fRoughnessSqr / (d * d + 1e-7f);
	//return fNDCoeff;
	//float 
}

float GGX_PartialGeometry(float cosThetaN, float alpha)
{
    float cosTheta_sqr = saturate(cosThetaN * cosThetaN);
    float tan2 = (1 - cosTheta_sqr) / cosTheta_sqr;
    float GP = 2 / (1 + sqrt(1 + alpha * alpha * tan2));
    return GP;
}

float MicrofacetFresnel(in float3 LightDir, in float3 Normal, in float fRoughness)
{
    float IOR = 1.5f;
    float f0 = (1 - IOR) / (1 + IOR);
    f0 *= f0;
    // Cosine between light and normal
    float CosPhi = max(dot(LightDir, Normal), 0);

    return f0 + (1 - f0) * pow5(1 - CosPhi);
}

float GGX_Distribution(float cosThetaNH, float alpha)
{
    float alpha2 = alpha * alpha;
    float NH_sqr = saturate(cosThetaNH * cosThetaNH);
    float den = NH_sqr * alpha2 + (1.0 - NH_sqr);
    return alpha2 / (PI * den * den);
}

inline float SmithJointGGXVisibilityTerm(float NdotL, float NdotV, float roughness)
{
	// Original formulation:
	//	lambda_v	= (-1 + sqrt(a2 * (1 - NdotL2) / NdotL2 + 1)) * 0.5f;
	//	lambda_l	= (-1 + sqrt(a2 * (1 - NdotV2) / NdotV2 + 1)) * 0.5f;
	//	G			= 1 / (1 + lambda_v + lambda_l);

	// Reorder code to be more optimal
    float a = roughness;
    float a2 = a * a;

    float lambdaV = NdotL * sqrt((-NdotV * a2 + NdotV) * NdotV + a2);
    float lambdaL = NdotV * sqrt((-NdotL * a2 + NdotL) * NdotL + a2);

	// Simplify visibility term: (2.0f * NdotL * NdotV) /  ((4.0f * NdotL * NdotV) * (lambda_v + lambda_l + 1e-5f));
    return 0.5f / (lambdaV + lambdaL + 1e-5f);
}

void CalculateSpecularTerm(in float3 vNormal, in float3 vLightDir, in float3 vViewDir, in float fRoughness, out float fSpecularTerm)
{
    float3 vHalfWay = normalize(vLightDir + vViewDir);
    float fresnelTerm = MicrofacetFresnel(vLightDir, vHalfWay, fRoughness);
    float roug_sqr = fRoughness * fRoughness;
    float G = GGX_PartialGeometry(dot(vNormal, vViewDir), roug_sqr) * GGX_PartialGeometry(dot(vNormal, vLightDir), roug_sqr);
    float D = GGX_Distribution(dot(vNormal, vHalfWay), roug_sqr);
    float ndfTerm = MicrofacetNDF_GGX(vNormal, vHalfWay, fRoughness * fRoughness);
    float ndl = saturate(dot(vNormal, vLightDir));
    float ndv = saturate(dot(vNormal, vViewDir));
    float visibilityTerm = SmithJointGGXVisibilityTerm(ndl, ndv, fRoughness * fRoughness);
    /*if (ndv <= 0.0 || ndl <= 0.0)
    {
        fSpecularTerm = 0.0f;
        return;
    }*/
	//MicrofacetGeometricShadow(vNormal, vViewDir, fRoughness);
#define USE_PBR 1 
#if USE_PBR==1
    fSpecularTerm = ndfTerm * 0.25f;
#else
    fSpecularTerm = PhongSpecular(vNormal, vLightDir, -vViewDir, fRoughness) * 9.0f;
#endif
    //
    //ndfTerm * fresnelTerm;
}

void CalculateDiffuseTerm_ViewDependent(in float3 vNormal, in float3 vLightDir, in float3 vViewDir, out float fDiffuseTerm, in float fRoughness)
{
    float fLambert = LambertDiffuse(vNormal, vLightDir);
    float fFL = pow5(1 - saturate(dot(vNormal, vLightDir)));
    float fFV = pow5(1 - saturate(dot(vNormal, vViewDir)));
    float3 vHalfWay = normalize(vLightDir + vViewDir);
	
    float LdotH = saturate(dot(vHalfWay, vLightDir));

    float fd90 = 0.5 + 2 * LdotH * LdotH * fRoughness;
	// Two schlick fresnel term
    float lightScatter = (1 + (fd90 - 1) * fFL);
    float viewScatter = (1 + (fd90 - 1) * fFV);
    fDiffuseTerm = (lightScatter * viewScatter) * fLambert;
}
