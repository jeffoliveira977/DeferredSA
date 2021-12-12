#pragma once
#include "CommonD.h"

class PostProcessing
{
public:

	PostProcessing();
	~PostProcessing();

	void Initialize();

	void RenderBloom();
	void RenderFXAA();
	void RenderDOF();
	void RenderLensFlare();
private:
	RwRaster* mFXAARaster;
	RwRaster* mDownFilter4Raster;
	RwRaster* mBloomRaster;
	RwRaster* mGaussianBlurXRaster;
	RwRaster* mGaussianBlurYRaster;
	RwRaster* mScreenRaster;

	void* mFXAAVertexShader;
	void* mFXAAPixelShader;
	void* mGaussianBlurVertexShader;
	void* mGaussianBlurXPixelShader;
	void* mGaussianBlurYPixelShader;
	void* mBloomPixelShader;
	void* mDownFilter4PixelShader;
	void* mBloomCombinePixelShader;
};

