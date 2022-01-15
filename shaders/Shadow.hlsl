struct ShadowData
{
    float4 splitDistance;
    float4 params;
    float4 bias;
    row_major float4x4 viewProjection[4];
};

static int CascadeCount = 4;

int GetSplitByDepth(float fDepth, ShadowData shadowBuffer)
{
    int i = dot(float4(CascadeCount > 0, CascadeCount > 1, CascadeCount > 2, CascadeCount > 3),
					  float4(fDepth > shadowBuffer.splitDistance.y, fDepth > shadowBuffer.splitDistance.z, fDepth > shadowBuffer.splitDistance.w, fDepth > shadowBuffer.params.y));
    return i;
}

float4 PS_Unlit(sampler2D shadowmap, float2 shadowCoord, float Z) : COLOR0
{
   // 3x3 PCF Create 9 coordinates for the kernel
    float2 vTexCoords[9];
   // texel size
    float fTexelSize = 1.0f / 4096.0f;

   // 4 3 5
   // 1 0 2
   // 7 6 8
    vTexCoords[0] = shadowCoord;
    vTexCoords[1] = shadowCoord + float2(-fTexelSize, 0.0f);
    vTexCoords[2] = shadowCoord + float2(fTexelSize, 0.0f);
    vTexCoords[3] = shadowCoord + float2(0.0f, -fTexelSize);
    vTexCoords[6] = shadowCoord + float2(0.0f, fTexelSize);
    vTexCoords[4] = shadowCoord + float2(-fTexelSize, -fTexelSize);
    vTexCoords[5] = shadowCoord + float2(fTexelSize, -fTexelSize);
    vTexCoords[7] = shadowCoord + float2(-fTexelSize, fTexelSize);
    vTexCoords[8] = shadowCoord + float2(fTexelSize, fTexelSize);

    // Sampling while checking to see if the pixel being tested is in the shadow area
    float fShadowTerms[9];
    float fShadowTerm = 0.0f;
    for (int i = 0; i < 9; i++)
    {
        float A = tex2D(shadowmap, vTexCoords[i]).r;
        float B = Z;

        // Texel in shadow area
        fShadowTerms[i] = A < B ? 0.0f : 1.0f;
        fShadowTerm += fShadowTerms[i];
    }
    
    // average gain
    fShadowTerm /= 9.0f;
    return fShadowTerm;
}
//unpack the depth from a 32-bit rgba color
float getDepthFromARGB32(const float4 value)
{
    const float4 bitSh = float4(1.0 / (256.0 * 256.0 * 256.0), 1.0 / (256.0 * 256.0), 1.0 / 256.0, 1.0);
    return (dot(value, bitSh));
}

float tex2DShadow(sampler2D arrays[4], float2 texcoord, int count)
{
    float4 texels = 0;
    switch (count)
    {
        case 0:
            texels = tex2Dlod(arrays[0], float4(texcoord, 0, 1));
            break;
        case 1:
            texels = tex2Dlod(arrays[1], float4(texcoord, 0, 1));
            break;
        case 2:
            texels = tex2Dlod(arrays[2], float4(texcoord, 0, 1));
            break;
        case 3:
            texels = tex2Dlod(arrays[3], float4(texcoord, 0, 1));
            break;
    }
    return texels.r;
}

float4 tex2DShadow6x6(sampler2D arrays[6], float2 texcoord, int count)
{
 
    float4 texels = 0.0f;
    switch (count)
    {
        case 0:
            texels = tex2D(arrays[0], texcoord);
            break;
        case 1:
            texels = tex2D(arrays[1], texcoord);
            break;
        case 2:
            texels = tex2D(arrays[2], texcoord);
            break;
        case 3:
            texels = tex2D(arrays[3], texcoord);
            break;
        case 4:
            texels = tex2D(arrays[4], texcoord);
            break;
        case 5:
            texels = tex2D(arrays[5], texcoord);
            break;
    }
    return texels;
}


// -------------------------------------
// STEP 1: Search for potential blockers
// -------------------------------------
float findBlocker(float2 uv, float receiver, sampler2D ShadowMap[4], float searchWidth, float numSamples, int cascade)
{

        // divide filter width by number of samples to use
    float stepSize = 2 * searchWidth / numSamples;

        // compute starting point uv coordinates for search
    uv = uv - float2(searchWidth, searchWidth);

        // reset sum to zero
    float blockerSum = 0;
    float blockerCount = 0;
    float foundBlocker = 0;

    [loop]
        // iterate through search region and add up depth values
    for (int i = 0; i < numSamples; i++)
    {
            [loop]
        for (int j = 0; j < numSamples; j++)
        {
            float shadMapDepth = tex2DShadow(ShadowMap, uv +
                                                 float2(i * stepSize, j * stepSize), cascade).x;
                       // found a blocker
            if (shadMapDepth < receiver)
            {
                blockerSum += shadMapDepth;
                blockerCount++;
                foundBlocker = 1;
            }
        }
    }

    float result;
		
    if (foundBlocker == 0)
    {
			// set it to a unique number so we can check
			// later to see if there was no blocker
        result = 999;
    }
    else
    {
		    // return average depth of the blockers
        result = blockerSum / blockerCount;
    }
		
    return result;
}
static bool distanceControl = false;
// ------------------------------------------------
// STEP 2: Estimate penumbra based on
// blocker estimate, receiver depth, and light size
// ------------------------------------------------
float estimatePenumbra(float receiver, float lightSize,
                     float blocker)
{

       // estimate penumbra using parallel planes approximation
    float penumbra = (receiver - blocker) * lightSize / blocker;
       
    return penumbra;
}

// ----------------------------------------------------
// Step 3: Percentage-closer filter implementation with
// variable filter width and number of samples.
// This assumes a square filter with the same number of
// horizontal and vertical samples.
// ----------------------------------------------------

float PCF_Filter(float2 uv, float receiver, sampler2D ShadowMap[4], float filterWidth, float numSamples, int cascade)
{
       // compute step size for iterating through the kernel
    float stepSize = 2 * filterWidth / numSamples;

       // compute uv coordinates for upper-left corner of the kernel
    uv = uv - float2(filterWidth, filterWidth);

    float sum = 0; // sum of successful depth tests
      
    [loop]
       // now iterate through the kernel and filter
    for (int i = 0; i < numSamples; i++)
    {
        [loop]
        for (int j = 0; j < numSamples; j++)
        {
                       // get depth at current texel of the shadow map
            
            half shadMapDepth = tex2DShadow(ShadowMap, uv +
                                                 float2(i * stepSize, j * stepSize), cascade).x;

                       // test if the depth in the shadow map is closer than
                       // the eye-view point
            half shad = receiver < shadMapDepth;

                       // accumulate result
            sum += shad;
        }
    }
       
       // return average of the samples
    return sum / (numSamples * numSamples);

}

static int BLOCKER_SEARCH_NUM_SAMPLES = 16;
static int PCF_NUM_SAMPLES = 16;
static float NEAR_PLANE = 0.1;
static float LIGHT_WORLD_SIZE = 0.5;
static float LIGHT_FRUSTUM_WIDTH = 1.75;

// Assuming that LIGHT_FRUSTUM_WIDTH == LIGHT_FRUSTUM_HEIGHT
static float LIGHT_SIZE_UV =(LIGHT_WORLD_SIZE/ LIGHT_FRUSTUM_WIDTH);


   static float2 poissonDisk[16] =
    {
        float2(-0.94201624, -0.39906216),
        float2(0.94558609, -0.76890725),
        float2(-0.094184101, -0.92938870),
         float2(0.34495938, 0.29387760),
         float2(-0.91588581, 0.45771432),
         float2(-0.81544232, -0.87912464),
         float2(-0.38277543, 0.27676845),
         float2(0.97484398, 0.75648379),
         float2(0.44323325, -0.97511554),
         float2(0.53742981, -0.47373420),
         float2(-0.26496911, -0.41893023),
         float2(0.79197514, 0.19090188),
         float2(-0.24188840, 0.99706507),
         float2(-0.81409955, 0.91437590),
         float2(0.19984126, 0.78641367),
         float2(0.14383161, -0.14100790)
    };

float PenumbraSize(float zReceiver, float zBlocker) //Parallel plane estimation
{
    return (zReceiver - zBlocker) / zBlocker;
}

void FindBlocker(out float avgBlockerDepth,
 out float numBlockers, sampler2D ShadowMap[4],
 float2 uv, float zReceiver, int cascade, int numBlocker)
{
 //This uses similar triangles to compute what
 //area of the shadow map we should search
    float searchWidth = LIGHT_SIZE_UV * (zReceiver - NEAR_PLANE) / zReceiver;
    float blockerSum = 0;
    numBlockers = 0;

    for (int i = 0; i < numBlocker; ++i)
    {

        float shadowMapDepth = tex2DShadow(ShadowMap, uv +
                                                 poissonDisk[i] * searchWidth, cascade).x;
        if (shadowMapDepth < zReceiver)
        {
            blockerSum += shadowMapDepth;
            numBlockers++;
        }
    }
    avgBlockerDepth = blockerSum / numBlockers;
}

float PCF_Filter2(float2 uv, sampler2D ShadowMap[4], 
float zReceiver, float filterRadiusUV, int cascade, int numSamples)
{
    float sum = 0.0f;
    for (int i = 0; i < numSamples; ++i)
    {
        float2 offset = poissonDisk[i] * filterRadiusUV;
        sum += zReceiver < tex2DShadow(ShadowMap, uv +
                                                offset, cascade).x;
    }
    return sum / PCF_NUM_SAMPLES;
}

float PCSS(float2 uv, float z, sampler2D ShadowMap[4], int cascade,  int numBlocker, int numSamples)
{
    float zReceiver = z; // Assumed to be eye-space z in this code

 // STEP 1: blocker search
    float avgBlockerDepth = 0;
    float numBlockers = 0;
    FindBlocker(avgBlockerDepth, numBlockers, ShadowMap,
    uv, zReceiver, cascade, numBlocker);
    if (numBlockers < 1)
 //There are no occluders so early out (this saves filtering)
        return 1.0f;
 // STEP 2: penumbra size
    float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth);
    float filterRadiusUV = penumbraRatio * LIGHT_SIZE_UV * NEAR_PLANE / z;

 // STEP 3: filtering
    return PCF_Filter2(uv, ShadowMap,
    zReceiver, filterRadiusUV, cascade, numSamples);
}

void FadeOutShadow(inout float fShadow, float fDepth, float fFadeDist)
{
    if (fShadow < 1.0) // Only do fading if fragment is in shadow
    {
			// Start fadeout at 90% from the shadow border
        float l_FadeStart = fFadeDist * 0.9;
			// If the fragement is inside the fade area...
        if (fDepth > l_FadeStart)
        {
            //fShadow *= 3.8;
            fShadow = max((fDepth - l_FadeStart) / (fFadeDist - l_FadeStart), fShadow);
        }
    }
    fShadow = saturate(fShadow);
}



#define NUM_TAPS 12
static const float fScale = 1.0;
static const float2 fTaps_Poisson[NUM_TAPS] =
{
    { -.326, -.406 },
    { -.840, -.074 },
    { -.696, .457 },
    { -.203, .621 },
    { .962, -.195 },
    { .473, -.480 },
    { .519, .767 },
    { .185, -.893 },
    { .507, .064 },
    { .896, .412 },
    { -.322, -.933 },
    { -.792, -.598 }
};

float SampleBox(sampler2D shadowmap[4], float2 shadowCoord, float Z, float size, int c)
{
    float texels = 1.0 / (size);
    float fShadow = 0.0;
    for (int i = 0; i < NUM_TAPS; i++)
    {
        fShadow += (Z < tex2DShadow(shadowmap, shadowCoord + fTaps_Poisson[i] * fScale * texels, c).r);
    }
    fShadow *= 1.0 / NUM_TAPS;
    return fShadow;
    
    //int fStartOffset = 4;
    //float lit = 0.0f;
    //float weight = 0.0f;

    //float texels = 1.0 / (size / 2);
    //[loop]
    //for (int i = -fStartOffset; i < fStartOffset; ++i)
    //{
    //    [loop]
    //    for (int j = -fStartOffset; j < fStartOffset; ++j)
    //    {
    //        float2 uv = shadowCoord + float2(i, j) * texels;
    //       // lit += (Z < tex2D(shadowmap, uv).r);
    //        lit += (Z < tex2DShadow(shadowmap, uv, c).r);
    //        weight += 1.0;
    //    }
    //}
    //return lit / weight;
}

float GetShadowUnfiltered(sampler2D ShadowBuffer[4], int i, float4 LightViewPos, float Z, float size)
{
    
    //transform from RT space to texture space.
    float2 ShadowTexCoord = 0.5 * (LightViewPos.xy) + 0.5;
    ShadowTexCoord.y = 1.0f - ShadowTexCoord.y;
    // early fail
    if (ShadowTexCoord.x < 0 || ShadowTexCoord.x > 1 || ShadowTexCoord.y < 0 || ShadowTexCoord.y > 1)
        return 1.0f;
    
    return (Z < tex2DShadow(ShadowBuffer, ShadowTexCoord, i).r);
}

float GetShadow(sampler2D ShadowBuffer[4], int i, float4 LightViewPos, float Z, float size)
{ 
    
    //transform from RT space to texture space.
    float2 ShadowTexCoord = 0.5 * (LightViewPos.xy) + 0.5;
    ShadowTexCoord.y = 1.0f - ShadowTexCoord.y;
    // early fail
    if (ShadowTexCoord.x < 0 || ShadowTexCoord.x > 1 || ShadowTexCoord.y < 0 || ShadowTexCoord.y > 1)
        return 1.0f;
    
    return SampleBox(ShadowBuffer, ShadowTexCoord, Z, size, i);
    //float fShadow;
    //if (i == 3)
    //{
    //    fShadow = SampleBox(ShadowBuffer[3], ShadowTexCoord, Z, size);
    //}
    //else if (i == 2)
    //{
    //    fShadow = SampleBox(ShadowBuffer[2], ShadowTexCoord, Z, size);
    //}
    //else if (i == 1)
    //{
    //    fShadow = SampleBox(ShadowBuffer[1], ShadowTexCoord, Z, size);
    //}
    //else if (i == 0)
    //{
    //    fShadow = SampleBox(ShadowBuffer[0], ShadowTexCoord, Z, size);
    //}
			
    //return fShadow;
}

bool CheckIsOutOfBounds(float4 LightViewPos, float size)
{
    float2 ShadowTexCoord = 0.5 * LightViewPos.xy + 0.5;
    ShadowTexCoord.y = 1.0f - ShadowTexCoord.y;

    float blur_err = (10 * max(3, 2)) / size;

    // early fail
    return (ShadowTexCoord.x < blur_err || ShadowTexCoord.x > (1 - blur_err) ||
            ShadowTexCoord.y < blur_err || ShadowTexCoord.y > (1 - blur_err));
}

static const int CASCADE_SHADOW_NUM = 4;
static const int PCF_KERNEL_COUNT = CASCADE_SHADOW_NUM * CASCADE_SHADOW_NUM;
#define CASCADE_SHADOW_NUM 4

float2 ComputeMoments(float Depth)
{
    float2 Moments; 
    
    // First moment is the depth itself.   
    Moments.x = Depth;   
    
    // Compute partial derivatives of depth.    
    float dx = ddx(Depth);   
    float dy = ddy(Depth);   
    
    // Compute second moment over the pixel extents.  
    Moments.y = Depth*Depth +(dx*dx + dy*dy);   
    return Moments; 
}

float ChebyshevUpperBound(float2 Moments, float t)
{
    // One-tailed inequality valid if t > Moments.x     
    //if (t <= Moments.x)
    //{
    //    return  1;
    //}
    //else
    //{
        // Compute variance.    
       // float Variance = Moments.y - (Moments.x * Moments.x);
      //  Variance = max(Variance, 0.0001);
        
        float Variance = (Moments.y) - (Moments.x * Moments.x);
        Variance = min(1.0f, max(0.0f, Variance + 0.00001f));
    
        // Compute probabilistic upper bound.    
        float d = t - Moments.x;
        float p_max = Variance / (Variance + d * d);
        return pow(p_max, 5.1);
    //}
}


float linstep(float min, float max, float v)
{
    return clamp((v= min) / (max= min), 0, 1);
}

float ReduceLightBleeding(float p_max, float Amount)
{ 
    // Remove the [0, Amount] tail and linearly rescale (Amount, 1].    
    return linstep(Amount, 1, p_max); 
}

// Where to split the value. 8 bits works well for most situations.   
static float g_DistributeFactor = 256;
float4 DistributePrecision( float2 Moments)
{
    float FactorInv = 1 / g_DistributeFactor;
   
    // Split precision   
    float2 IntPart;
    float2 FracPart = modf(Moments* g_DistributeFactor, IntPart);
   
    // Compose outputs to make reconstruction cheap.   
    return float4(IntPart * FactorInv, FracPart);
}

float2 RecombinePrecision(float4 Value)
{
    float FactorInv = 1 / g_DistributeFactor;
    return (Value.zw * FactorInv + Value.xy);
}

static const float SHADOW_EPSILON = 0.0000f;
float ComputeShadowSamples(sampler2D samplerShadow, float shadowSize, float2 shadowTexCoord, float ourdepth)
{
    float ShadowMapPixelSize = 1.0f / shadowSize;
	// Get the current depth stored in the shadow map
    float samples[4];
    
    samples[0] = tex2D(samplerShadow, shadowTexCoord).r + SHADOW_EPSILON < ourdepth ? 0 : 1;
    samples[1] = tex2D(samplerShadow, shadowTexCoord + float2(0, ShadowMapPixelSize)).r + SHADOW_EPSILON < ourdepth ? 0 : 1;
    samples[2] = tex2D(samplerShadow, shadowTexCoord + float2(ShadowMapPixelSize, 0)).r + SHADOW_EPSILON < ourdepth ? 0 : 1;
    samples[3] = tex2D(samplerShadow, shadowTexCoord + float2(ShadowMapPixelSize, ShadowMapPixelSize)).r + SHADOW_EPSILON < ourdepth ? 0 : 1;
    
	// Determine the lerp amounts           
    float2 lerps = frac(shadowTexCoord * shadowSize);
    
	// lerp between the shadow values to calculate our light amount
    half shadow = lerp(lerp(samples[0], samples[1], lerps.y), lerp(samples[2], samples[3], lerps.y), lerps.x);
				
    return shadow;
}

float ComputeShadow4Samples(sampler2D samplerShadow[4], int cascade, float shadowSize, float2 shadowTexCoord, float ourdepth)
{
    float ShadowMapPixelSize = 1.0 / shadowSize;
	// Get the current depth stored in the shadow map
    float samples[4];
    
    samples[0] = tex2DShadow(samplerShadow, shadowTexCoord, cascade).r < ourdepth ? 0 : 1;
    samples[1] = tex2DShadow(samplerShadow, shadowTexCoord + float2(0, 2) * ShadowMapPixelSize, cascade).r < ourdepth ? 1 : 0;
    samples[2] = tex2DShadow(samplerShadow, shadowTexCoord + float2(2, 0) * ShadowMapPixelSize, cascade).r < ourdepth ? 1 : 0;
    samples[3] = tex2DShadow(samplerShadow, shadowTexCoord + float2(2, 2) * ShadowMapPixelSize, cascade).r < ourdepth ? 1 : 0;
    
	// Determine the lerp amounts           
    float2 lerps = frac(shadowTexCoord * shadowSize);
    
	// lerp between the shadow values to calculate our light amount
    half shadow = lerp(lerp(samples[0], samples[1], lerps.y), lerp(samples[2], samples[3], lerps.y), lerps.x);
				
    return shadow;
}

float Compute4Samples(sampler2D samplerShadow, float2 shadowTexCoord, float2 shadowSize, float ourdepth, float far)
{
    float2 ShadowMapPixelSize = 1.0 / shadowSize;
	// Get the current depth stored in the shadow map
    float samples[4];
    
    samples[0] = ourdepth < (1 - tex2D(samplerShadow, shadowTexCoord).r) * far ? 1 : 0.5;
    samples[1] = ourdepth < (1 - tex2D(samplerShadow, shadowTexCoord + float2(0, 2) * ShadowMapPixelSize).r) * far ? 1 : 0.5;
    samples[2] = ourdepth < (1 - tex2D(samplerShadow, shadowTexCoord + float2(2, 0) * ShadowMapPixelSize).r) * far ? 1 : 0.5;
    samples[3] = ourdepth < (1 - tex2D(samplerShadow, shadowTexCoord + float2(2, 2) * ShadowMapPixelSize).r) * far ? 1 : 0.5;
    
	// Determine the lerp amounts           
    float2 lerps = frac(shadowTexCoord * shadowSize);
    
	// lerp between the shadow values to calculate our light amount
    half shadow = lerp(lerp(samples[0], samples[1], lerps.y), lerp(samples[2], samples[3], lerps.y), lerps.x);
				
    return shadow;
}

float rand2dTo1d(float2 value, float2 dotDir = float2(12.9898, 78.233))
{
    float2 smallValue = sin(value);
    float random = dot(smallValue, dotDir);
    random = frac(sin(random) * 143758.5453);
    return random;
}
float3 rand2dTo3d(float2 value)
{
    return float3(
		rand2dTo1d(value, float2(12.989, 78.233)),
		rand2dTo1d(value, float2(39.346, 11.135)),
		rand2dTo1d(value, float2(73.156, 52.235))
	);
}
float2 rand2dTo2d(float2 value)
{
    return float2(
		rand2dTo1d(value, float2(12.989, 78.233)),
		rand2dTo1d(value, float2(39.346, 11.135))
	);
}


static const float2 irreg_kernel[8] =
{
    { -0.072, -0.516 },
    { -0.105, 0.989 },
    { 0.949, 0.258 },
    { -0.966, 0.216 },
    { 0.784, -0.601 },
    { 0.139, 0.230 },
    { -0.816, -0.516 },
    { 0.529, 0.779 }
};
float4 DrawShadow(sampler2D samplerShadow[4],float2 vpos, float depthTest, float3 worldPosition, ShadowData shadowBuffer)
{ 
    // early fail
    if (shadowBuffer.params.w <= 0)
        return 0.0f;
    
    float4 WorldPos = float4(worldPosition, 1.0);
    int nCascadeFound = 0;
    int nCascadeIndex = 0;
    float2 lightDepthMapTexSize = 1.0 / shadowBuffer.params.x;
    float2 lightDepthUV = float2(0.0, 0.0);
    float2 lightSpaceWSPos;
    float xyBiggerTexSize = max(lightDepthMapTexSize.x, lightDepthMapTexSize.y);
    float4 LightViewPos;
    for (int index = 0; index < CASCADE_SHADOW_NUM && 0 == nCascadeFound; ++index)
    {
        LightViewPos = mul(WorldPos, shadowBuffer.viewProjection[index]);
        
        
        lightDepthUV = 0.5 * LightViewPos.xy + 0.5;
        lightDepthUV.y = 1.0f - lightDepthUV.y;

        if (min(lightDepthUV.x, lightDepthUV.y) > xyBiggerTexSize && max(lightDepthUV.x, lightDepthUV.y) < 1.0 - xyBiggerTexSize)
        {
            nCascadeIndex = index;
            nCascadeFound = 1;
        }
    }
    
  //  float2 pcf_kernel[PCF_KERNEL_COUNT] =
  //  {
  //      float2(-1, -1), 
  //      float2(0, -1), 
  //      float2(1, -1),
		//float2(-1, 0), 
  //      float2(0, 0), 
  //      float2(1, 0),
		//float2(-1, 1), 
  //      float2(0, 1), 
  //      float2(1, 1),
  //  };

  //  float4 debugColor = ARRAY_DEBUG_COLOR[nCascadeIndex];
  //  //float4 debugColor = float4(1.0, 1.0, 1.0, 1.0);
  //  float4 color=0;
  //  for (int nKernelIndex = 0; nKernelIndex < PCF_KERNEL_COUNT; ++nKernelIndex)
  //  {
  //      float2 uv = float2(lightDepthUV.x, lightDepthUV.y)
		//+ pcf_kernel[nKernelIndex] * lightDepthMapTexSize;
  //      float nearestDepth = tex2DShadow(samplerShadow, uv, nCascadeIndex).r;
  //      float z = LightViewPos.z - shadowBuffer.bias[nCascadeIndex];
  //      bool isShadowed = z > nearestDepth;
  //      color += isShadowed ? float4(0.0, 0.0, 0.0, 1.0) : float4(1.0, 1.0, 1.0, 1.0);
  //  }
	
  //  color /= (float) PCF_KERNEL_COUNT;

  //  color = color * debugColor;
  //  return color;
    
     // Compute uv coordinates for the point being shaded
   // Saves some future recomputation.
   // float2 uv = lightDepthUV;
   // float lightSize = 0.03;
   // float sceneScale = 0.08;
   // float ShadBias = 0.1;
   // float LP = LightViewPos.z - shadowBuffer.bias[nCascadeIndex];
   //// ---------------------------------------------------------
   //// Step 1: Find blocker estimate
   // float searchSamples = 8; // how many samples to use for blocker search
   // float blocker = findBlocker(uv, LP, samplerShadow,
   //                           sceneScale * lightSize / LP, searchSamples, nCascadeIndex);
   
   ////return (blocker*0.3);  // uncomment to visualize blockers
   
   //// ---------------------------------------------------------
   //// Step 2: Estimate penumbra using parallel planes approximation
   // float penumbra;
   // penumbra = estimatePenumbra(LP, lightSize, blocker);

   ////return penumbra*32;  // uncomment to visualize penumbrae

   //// ---------------------------------------------------------
   //// Step 3: Compute percentage-closer filter
   //// based on penumbra estimate
   // float samples = 8; // reduce this for higher performance

   //// Now do a penumbra-based percentage-closer filter
   // float shadowed;

   // shadowed = PCF_Filter(uv, LP, samplerShadow, penumbra, samples, nCascadeIndex);
   
   //// If no blocker was found, just return 1.0
   //// since the point isn't occluded
   
   // if (blocker > 998) 
   //     shadowed = 1.0;
   // return shadowed;
    
    
   // return PCSS(uv, LP, samplerShadow, nCascadeIndex, searchSamples, samples);
    
   // // Variance shadow mapping code below from the variance shadow
   //  // mapping demo code @ http://www.punkuser.net/vsm/

     // early fail
    if (lightDepthUV.x < 0 || lightDepthUV.x > 1 || lightDepthUV.y < 0 || lightDepthUV.y > 1)
        return 1.0f;
    
    
    
    float z = LightViewPos.z - shadowBuffer.bias[nCascadeIndex];
    float fShadow = 0.0;
    //for (int i = 0; i < NUM_TAPS; i++)
    //{
    //    fShadow += (z < tex2DShadow(samplerShadow, lightDepthUV + fTaps_Poisson[i] * 1.2 * lightDepthMapTexSize, nCascadeIndex));
    //}
    //fShadow *= 1.0 / NUM_TAPS;
    //return fShadow;
    
    float2 stex = vpos.xy / 16.0f;
    float2 noise;
    float2 rotated;
    noise = rand2dTo2d(stex);
    noise = normalize(noise * 2.0f - 1.0f);
    float texelsize = 1.0f / shadowBuffer.params.x;
    float2x2 rotmat = { noise.x, -noise.y, noise.y, noise.x };

    for (int i = 0; i < 8; ++i)
    {
        rotated = irreg_kernel[i];
        rotated = mul(rotated, rotmat) * 2.0f;

        float sd = (z < tex2DShadow(samplerShadow, lightDepthUV + rotated * texelsize, nCascadeIndex));

        fShadow += sd;
    }

    fShadow = saturate(fShadow * 0.125f);
    
    if (LightViewPos.z > 1.0)
        fShadow = 0.0;
    return fShadow;
   // return GetShadow(samplerShadow, nCascadeIndex, LightViewPos, LightViewPos.z - shadowBuffer.bias[nCascadeIndex], shadowBuffer.params.x) * Jitter;
    
    //bool visualizeCascades = false;
    //float3 diffuse = 1;
    //float far = shadowBuffer.params.y;
    //depthTest = max(depthTest - 0.15f, 0.0);
    //int i = GetSplitByDepth(depthTest, shadowBuffer);
    //const float blendDistance = 16;
    //float4 WorldPos = float4(worldPosition, 1.0);
   
    //float4 LightViewPos = mul(WorldPos, shadowBuffer.viewProjection[i]);
    //float Z = LightViewPos.z;
    //float shadow = 1;
    //float shadowSize = shadowBuffer.params.x;
    //if (!CheckIsOutOfBounds(LightViewPos, shadowSize))
    //{
    //    shadow = GetShadow(samplerShadow, i, LightViewPos, Z - shadowBuffer.bias[i], shadowSize);
    //}
    //else if (i < CascadeCount)
    //{
    //    i = i + 1;
    //    LightViewPos = mul(WorldPos, shadowBuffer.viewProjection[i]);
    //    Z = LightViewPos.z;
    //    if (!CheckIsOutOfBounds(LightViewPos, shadowSize))
    //    {
    //        shadow = GetShadow(samplerShadow, i, LightViewPos, Z - shadowBuffer.bias[i], shadowSize);
    //    }
    //    else if (i < CascadeCount)
    //    {
    //        i = i + 1;
    //        LightViewPos = mul(WorldPos, shadowBuffer.viewProjection[i]);
    //        Z = LightViewPos.z;
    //        if (!CheckIsOutOfBounds(LightViewPos, shadowSize))
    //        {
    //            shadow = GetShadow(samplerShadow, i, LightViewPos, Z - shadowBuffer.bias[i], shadowSize);
    //        }
    //        else if (i < CascadeCount)
    //        {
    //            i = i + 1;
    //            LightViewPos = mul(WorldPos, shadowBuffer.viewProjection[i]);
    //            Z = LightViewPos.z;
    //            shadow = GetShadow(samplerShadow, i, LightViewPos, Z - shadowBuffer.bias[i], shadowSize);
                
    //        }
    //    }
    //}
    //float blendFactor = min(max(depthTest - far, 0) / blendDistance, 1.0);
    //return max(lerp(shadow, 1.0, blendFactor), 1 - shadowBuffer.params.w);
}

// Samples cascade shadow maps, blended 
float SampleShadowCascadesUnfiltered(sampler2D samplerShadow[4], float depthTest, float3 worldPosition, ShadowData shadowBuffer)
{
    
	// early fail, should return 1.0 here
    if (shadowBuffer.params.w <= 0)
        return 1.0f;
    
	// TODO: make blend distance customizible, perhaps different for each cascade.
    const float blendDistance = 6;
	// Shift view depth by blend distance, optimization for static blend distance
    depthTest += blendDistance;

    float Shadow = 1;
    float blendFactor;
    float far, near;
    float4 WorldPos = float4(worldPosition.xyz, 1);
    int i = GetSplitByDepth(depthTest, shadowBuffer);
    float4 LightViewPos = mul(WorldPos, shadowBuffer.viewProjection[i]);
    far = shadowBuffer.params.y;
    Shadow = GetShadowUnfiltered(samplerShadow, i, LightViewPos, LightViewPos.z - shadowBuffer.bias[i], shadowBuffer.params.x);

    blendFactor = min(max(depthTest - far, 0) / blendDistance, 1.0);

    return max(lerp(Shadow, 1.0, blendFactor), 1 - shadowBuffer.params.w);
}