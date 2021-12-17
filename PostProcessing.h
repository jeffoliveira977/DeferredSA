#pragma once
#include "CommonD.h"
#include "RenderTarget.h"
#include "PixelShader.h"

class PostProcessing
{
public:

	void Initialize();

	void RenderBloom(RenderTarget* screenRaster);
	void RenderFXAA(RenderTarget* screenRaster);
	void RenderDOF();
	void RenderLensFlare();
private:
	unique_ptr<RenderTarget> mDownFilter4x4RT;
	unique_ptr<RenderTarget> mBloomRT;
	unique_ptr<RenderTarget> mGaussianBlurXRT;
	unique_ptr<RenderTarget> mGaussianBlurYRT;
	unique_ptr<RenderTarget> mScreenRT;

	unique_ptr<PixelShader> mFXAAPS;
	unique_ptr<PixelShader> mGaussianBlurXPS;
	unique_ptr<PixelShader> mGaussianBlurYPS;
	unique_ptr<PixelShader> mBloomPS;
	unique_ptr<PixelShader> mDownFilter4PS;
	unique_ptr<PixelShader> mBloomCombinePS;
};

