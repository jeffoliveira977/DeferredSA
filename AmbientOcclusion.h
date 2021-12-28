#pragma once
#include "CommonD.h"
#include "PixelShader.h"
#include "VertexShader.h"
class AmbientOcclusion
{
public:
	AmbientOcclusion();
	~AmbientOcclusion();

	void Initialize();
	void Render();

	void UpdateImgui();
	RwRaster* mSSAORaster;
	RwRaster* mDownSampled[2];
private:

	unique_ptr<PixelShader> mSSAOPixelShader;
	unique_ptr<PixelShader> mSSAOCombinePixelShader;
	unique_ptr<VertexShader> mSSAOVertexShader;
	unique_ptr<VertexShader> mSSAOBlurVertexShader;

	RwTexture* mSSAORandom;

};

