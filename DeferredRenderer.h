#pragma once
#include "CommonD.h"
#include "VolumetricClouds.h"
#include "PostProcessing.h"
#include "PixelShader.h"
#include "VolumetricLight.h"
#include "AmbientOcclusion.h"
#include "RenderTarget.h"

enum RenderingStage
{
	stageDeferred,
	stageForward,
	stageDualParaboloidMap,
	stageSphereMap,
	stageReflectionCubemap,
	stageReflection,
	stageRefraction,
	stageCascadeShadow,
	stagePointShadow
}extern gRenderState;

class DeferredRendering
{
public:

	unique_ptr<PostProcessing> mPostProcessing;

	unique_ptr<RenderTarget> mScreenRT;
	unique_ptr<RenderTarget> mGraphicsLight;
	unique_ptr<RenderTarget> mGraphicsBuffer[5];

	void Start();
	void Stop();
	void RenderPostProcessing();
	void Initialize();

	void UpdateImgui();
	void UpdateTextures();
private:
	unique_ptr<PixelShader> mDirectLightPS;
	unique_ptr<PixelShader> mCombineLightPS;
	unique_ptr<PixelShader> mAtmosphereScaterringPS;
	unique_ptr<PixelShader> mPointLightPS;
	unique_ptr<PixelShader> mSpotLightPS;

	unique_ptr<VolumetricClouds> mVolumetricClouds;
	unique_ptr<VolumetricLight> mVolumetricLight;
	unique_ptr<AmbientOcclusion> mAmbientOcclusion;

	void RenderLights();
	void RenderPointAndSpotLight();
	void RenderFinalPass();
	void AtmosphericScattering();
};

extern DeferredRendering *DeferredContext;