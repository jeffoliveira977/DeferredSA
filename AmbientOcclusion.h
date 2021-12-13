#pragma once
#include "CommonD.h"
#include "PixelShader.h"

class AmbientOcclusion
{
public:
	AmbientOcclusion();
	~AmbientOcclusion();

	void Initialize();
	void Render();

	void UpdateImgui();
	RwRaster* mSSAORaster;

private:

	PixelShader* mSSAOPixelShader;
	PixelShader* mSSAOCombinePixelShader;
};

