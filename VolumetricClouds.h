#pragma once
#include "PixelShader.h"

class VolumetricClouds
{
public:
	VolumetricClouds();
	~VolumetricClouds();

	void Initialize();
	void Render(RwRaster* screenSource);

private:
	PixelShader* mPixelShader;
	PixelShader* mCombinePixelShader;
	RwRaster* mVolumetricCloudRaster;
};