#include "PostProcessing.h"
#include "Quad.h"

void PostProcessing::Initialize()
{
	mGaussianBlurXRT = unique_ptr<RenderTarget>(new RenderTarget(D3DFMT_A16B16G16R16F));
	mGaussianBlurXRT->Initialize();

	mGaussianBlurYRT = unique_ptr<RenderTarget>(new RenderTarget(D3DFMT_A16B16G16R16F));
	mGaussianBlurYRT->Initialize();

	mBloomRT = unique_ptr<RenderTarget>(new RenderTarget(D3DFMT_A16B16G16R16F));
	mBloomRT->Initialize();

	mDownFilter4x4RT = unique_ptr<RenderTarget>(new RenderTarget(D3DFMT_A16B16G16R16F));
	mDownFilter4x4RT->Initialize();

	mFXAAPS = unique_ptr<PixelShader>(new PixelShader());
	mFXAAPS->CreateFromBinary("FXAA_PS");

	mGaussianBlurXPS = unique_ptr<PixelShader>(new PixelShader());
	mGaussianBlurXPS->CreateFromBinary("GaussianBlurX");

	mGaussianBlurYPS = unique_ptr<PixelShader>(new PixelShader());
	mGaussianBlurYPS->CreateFromBinary("GaussianBlurY");

	mBloomPS = unique_ptr<PixelShader>(new PixelShader());
	mBloomPS->CreateFromBinary("BloomPS");

	mDownFilter4PS = unique_ptr<PixelShader>(new PixelShader());
	mDownFilter4PS->CreateFromBinary("DownFilter4");

	mBloomCombinePS = unique_ptr<PixelShader>(new PixelShader());
	mBloomCombinePS->CreateFromBinary("BloomCombine");
}

void PostProcessing::RenderBloom(RenderTarget* screenRaster)
{

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);


	mBloomPS->Apply();
	screenRaster->CopyFromSurface(nullptr);
	rwD3D9RWSetRasterStage(screenRaster->GetRaster(), 0);
	RwD3D9SetRenderTarget(0, mBloomRT->GetRaster());
	Quad::Render();


	mDownFilter4PS->Apply();
	rwD3D9RWSetRasterStage(mBloomRT->GetRaster(), 0);
	RwD3D9SetRenderTarget(0, mDownFilter4x4RT->GetRaster());
	Quad::Render();

	mDownFilter4PS->Apply();
	rwD3D9RWSetRasterStage(mDownFilter4x4RT->GetRaster(), 0);
	RwD3D9SetRenderTarget(0, mBloomRT->GetRaster());
	Quad::Render();

	/*RwV2d samplesOffsets[15];
	float sampleWeights[15];
	GetGaussianOffsets(true, width / height, samplesOffsets, sampleWeights);*/
	//_rwD3D9SetVertexShaderConstant(0, samplesOffsets, sizeof(samplesOffsets));
	//_rwD3D9SetVertexShaderConstant(19, sampleWeights, sizeof(sampleWeights) );
	mGaussianBlurXPS->Apply();
	rwD3D9RWSetRasterStage(mBloomRT->GetRaster(), 0);
	RwD3D9SetRenderTarget(0, mGaussianBlurXRT->GetRaster());
	Quad::Render();

	//GetGaussianOffsets(false, width / height, samplesOffsets, sampleWeights);
	//_rwD3D9SetVertexShaderConstant(0, samplesOffsets, sizeof(samplesOffsets));
	//_rwD3D9SetVertexShaderConstant(19, sampleWeights, sizeof(sampleWeights) );

	mGaussianBlurYPS->Apply();
	rwD3D9RWSetRasterStage(mGaussianBlurXRT->GetRaster(), 0);
	RwD3D9SetRenderTarget(0, mGaussianBlurYRT->GetRaster());
	Quad::Render();

	mBloomCombinePS->Apply();
	rwD3D9RWSetRasterStage(mGaussianBlurYRT->GetRaster(), 0);
	rwD3D9RWSetRasterStage(screenRaster->GetRaster(), 1);
	__rwD3D9SetRenderTarget(0, RwD3D9RenderSurface);
	Quad::Render();
}

void PostProcessing::RenderFXAA(RenderTarget* screenRaster)
{
	screenRaster->CopyFromSurface(nullptr);

	_rwD3D9SetPixelShader(mFXAAPS->GetObject());
	rwD3D9RWSetRasterStage(screenRaster->GetRaster(), 0);
	__rwD3D9SetRenderTarget(0, RwD3D9RenderSurface);
	Quad::Render();
}

void PostProcessing::RenderDOF()
{}

void PostProcessing::RenderLensFlare()
{}
