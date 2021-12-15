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
	stageCascadeShadow
}extern gRenderState;

class DeferredRendering
{
public:

	DeferredRendering();
	~DeferredRendering();

	PostProcessing* mPostProcessing;


	RenderTarget* mScreenRaster;
	RenderTarget* mGraphicsLight;
	RenderTarget* mGraphicsBuffer[4];

	VolumetricClouds* mVolumetricClouds;
	VolumetricLight* mVolumetricLight;
	AmbientOcclusion* mAmbientOcclusion;

	void Start();
	void Stop();
	void RenderPostProcessing();
	void Initialize();

	void UpdateImgui();
	void UpdateTextures();
private:
	void* PS_DirectLight;
	void* PS_PointAndSpotLight;
	void* PS_TargetLight;
	void* PS_ShadowScreen;
	void* PS_AtmosphereScattering;
	void* PS_VolumetricLight;
	void* PS_VolumetricLightCombine;

	void* VS_Quad;

	PixelShader* mPointSpotLightPixelShader;
	void DirectLight();
	void PointAndSpotLight();
	void FinalPass();
	void CascadedShadow();
	void AtmosphericScattering();
};

extern DeferredRendering *DeferredContext;