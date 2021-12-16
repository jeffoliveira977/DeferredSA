#pragma once
#include "CommonD.h"
#include "RenderTarget.h"
class VolumetricLight
{
public:
	VolumetricLight();
	~VolumetricLight();

	void Initialize();
	void Render(RenderTarget* screenSource);
	void UpdateImgui();
private:
	void* PS_VolumetricLight;
	void* PS_VolumetricLightCombine;

	RenderTarget* mVolumetricLightRaster;
};

