#pragma once
#include "CommonD.h"

class VolumetricLight
{
public:
	VolumetricLight();
	~VolumetricLight();

	void Initialize();
	void Render();
	void UpdateImgui();
	void UpdateTextures();
private:
	void* PS_VolumetricLight;
	void* PS_VolumetricLightCombine;
	RwRaster* m_volumetricLight;
	RwRaster* m_screenRaster;
};

