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

// ===============================================================================================
// http://graphicrants.blogspot.com.au/2013/08/specular-brdf-reference.html
// ===============================================================================================
float GGX(float NdotV, float a)
{
    float k = a / 2;
    return NdotV / (NdotV * (1.0f - k) + k);
}

// ===============================================================================================
// Geometry with Smith approximation:
// http://blog.selfshadow.com/publications/s2013-shading-course/rad/s2013_pbs_rad_notes.pdf
// http://graphicrants.blogspot.fr/2013/08/specular-brdf-reference.html
// ===============================================================================================
float G_Smith(float a, float nDotV, float nDotL)
{
    return GGX(nDotL, a * a) * GGX(nDotV, a * a);
}

// ================================================================================================
// Fresnel with Schlick's approximation:
// http://blog.selfshadow.com/publications/s2013-shading-course/rad/s2013_pbs_rad_notes.pdf
// http://graphicrants.blogspot.fr/2013/08/specular-brdf-reference.html
// ================================================================================================
float3 Schlick_Fresnel(float3 f0, float3 h, float3 l)
{
    return f0 + (1.0f - f0) * pow((1.0f - dot(l, h)), 5.0f);
}

// ================================================================================================
// Lambertian BRDF
// http://en.wikipedia.org/wiki/Lambertian_reflectance
// ================================================================================================
float3 DirectDiffuseBRDF(float3 diffuseAlbedo, float nDotL)
{
    return (diffuseAlbedo * nDotL) / PI;
}

// ================================================================================================
// Cook-Torrence BRDF
float3 DirectSpecularBRDF(float3 camerPosition, float3 specularAlbedo, float3 positionWS, float3 normalWS, float3 lightDir, float roughness)
{
    float3 viewDir = normalize(camerPosition - positionWS);
    float3 halfVec = normalize(viewDir + lightDir);

    float nDotH = saturate(dot(normalWS, halfVec));
    float nDotL = saturate(dot(normalWS, lightDir));
    float nDotV = max(dot(normalWS, viewDir), 0.0001f);

    float alpha2 = roughness * roughness;

	// Normal distribution term with Trowbridge-Reitz/GGX.
    float D = alpha2 / (PI * pow(nDotH * nDotH * (alpha2 - 1) + 1, 2.0f));
 
	// Fresnel term with Schlick's approximation.
    float3 F = Schlick_Fresnel(specularAlbedo, halfVec, lightDir);

	// Geometry term with Smith's approximation.
    float G = G_Smith(roughness, nDotV, nDotL);

    return D * F * G;
}

float3 DirectLighting(float3 camerPosition, float3 normalWS, float3 lightColor, float3 lightPos, float3 diffuseAlbedo,
	float3 specularAlbedo, float3 positionWS, float roughness, float attenuation)
{
    float3 lighting = 0.0f;

    float3 pixelToLight = lightPos - positionWS;
    float lightDist = length(pixelToLight);
    float3 lightDir = pixelToLight / lightDist;

    float nDotL = saturate(dot(normalWS, lightDir));

    if (nDotL > 0.0f)
    {
        lighting = DirectDiffuseBRDF(diffuseAlbedo, nDotL) * attenuation + DirectSpecularBRDF(camerPosition, specularAlbedo, positionWS, normalWS, lightDir, roughness) * attenuation;
    }

    return max(lighting, 0.0f) * lightColor;
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
