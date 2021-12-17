#pragma once
#include "PixelShader.h"
#include "RenderTarget.h"

class VolumetricClouds
{
public:
	VolumetricClouds();
	~VolumetricClouds();

	void Initialize();
	void Render(RenderTarget* screenSource);

private:
	PixelShader* mPixelShader;
	PixelShader* mCombinePixelShader;
	RwRaster* mVolumetricCloudRaster;
};