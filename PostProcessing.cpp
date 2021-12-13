#include "PostProcessing.h"
#include "Quad.h"


PostProcessing::PostProcessing()
{
	mFXAAPixelShader = nullptr;
	mGaussianBlurXRaster = nullptr;
	mGaussianBlurYRaster = nullptr;
	mGaussianBlurXPixelShader = nullptr;
	mGaussianBlurYPixelShader = nullptr;

	mDownFilter4Raster = nullptr;
	mBloomRaster = nullptr;
	mBloomPixelShader = nullptr;
	mDownFilter4PixelShader = nullptr;
	mBloomCombinePixelShader = nullptr;
}

PostProcessing::~PostProcessing()
{}

void PostProcessing::Initialize()
{
	int width, height;

	width = RsGlobal.maximumWidth;
	height = RsGlobal.maximumHeight;

	//mFXAARaster = RwD3D9RasterCreate(width, height, D3DFMT_A16B16G16R16F, rwRASTERTYPECAMERATEXTURE);
	mGaussianBlurXRaster = RwD3D9RasterCreate(width, height, D3DFMT_A16B16G16R16F, rwRASTERTYPECAMERATEXTURE);
	mGaussianBlurYRaster = RwD3D9RasterCreate(width, height, D3DFMT_A16B16G16R16F, rwRASTERTYPECAMERATEXTURE);
	mBloomRaster = RwD3D9RasterCreate(width, height, D3DFMT_A16B16G16R16F, rwRASTERTYPECAMERATEXTURE);
	mDownFilter4Raster = RwD3D9RasterCreate(width, height, D3DFMT_A16B16G16R16F, rwRASTERTYPECAMERATEXTURE);
	mScreenRaster = RwD3D9RasterCreate(width, height, D3DFMT_A16B16G16R16F, rwRASTERTYPECAMERATEXTURE);

	mFXAAPixelShader = RwCreateCompiledPixelShader("FXAA_PS");

	mGaussianBlurXPixelShader = RwCreateCompiledPixelShader("GaussianBlurX");
	mGaussianBlurYPixelShader = RwCreateCompiledPixelShader("GaussianBlurY");

	mBloomPixelShader = RwCreateCompiledPixelShader("BloomPS");
	mDownFilter4PixelShader = RwCreateCompiledPixelShader("DownFilter4");
	mBloomCombinePixelShader = RwCreateCompiledPixelShader("BloomCombine");

}

void PostProcessing::RenderBloom()
{
	IDirect3DSurface9* screenSurface;
	auto screenExt = RASTEREXTFROMRASTER(mScreenRaster);
	screenExt->texture->GetSurfaceLevel(0, &screenSurface);

	RwD3DDevice->StretchRect(RwD3D9RenderSurface, NULL, screenSurface, NULL, D3DTEXF_NONE);
	screenSurface->Release();

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	_rwD3D9SetVertexShader(mGaussianBlurVertexShader);

	int width, height;
	width = RsGlobal.maximumWidth;
	height = RsGlobal.maximumHeight;

	_rwD3D9SetPixelShader(mBloomPixelShader);
	rwD3D9RWSetRasterStage(mScreenRaster, 0);
	RwD3D9SetRenderTarget(0, mBloomRaster);
	Quad::Render();

	_rwD3D9SetPixelShader(mDownFilter4PixelShader);
	rwD3D9RWSetRasterStage(mBloomRaster, 0);
	RwD3D9SetRenderTarget(0, mDownFilter4Raster);
	Quad::Render();

	_rwD3D9SetPixelShader(mDownFilter4PixelShader);
	rwD3D9RWSetRasterStage(mDownFilter4Raster, 0);
	RwD3D9SetRenderTarget(0, mBloomRaster);
	Quad::Render();

	/*RwV2d samplesOffsets[15];
	float sampleWeights[15];
	GetGaussianOffsets(true, width / height, samplesOffsets, sampleWeights);*/
	//_rwD3D9SetVertexShaderConstant(0, samplesOffsets, sizeof(samplesOffsets));
	//_rwD3D9SetVertexShaderConstant(19, sampleWeights, sizeof(sampleWeights) );
	_rwD3D9SetPixelShader(mGaussianBlurXPixelShader);
	rwD3D9RWSetRasterStage(mBloomRaster, 0);
	RwD3D9SetRenderTarget(0, mGaussianBlurXRaster);
	Quad::Render();

	//GetGaussianOffsets(false, width / height, samplesOffsets, sampleWeights);
	//_rwD3D9SetVertexShaderConstant(0, samplesOffsets, sizeof(samplesOffsets));
	//_rwD3D9SetVertexShaderConstant(19, sampleWeights, sizeof(sampleWeights) );

	_rwD3D9SetPixelShader(mGaussianBlurYPixelShader);
	rwD3D9RWSetRasterStage(mGaussianBlurXRaster, 0);
	RwD3D9SetRenderTarget(0, mGaussianBlurYRaster);
	Quad::Render();

	_rwD3D9SetPixelShader(mBloomCombinePixelShader);
	rwD3D9RWSetRasterStage(mGaussianBlurYRaster, 0);
	rwD3D9RWSetRasterStage(mScreenRaster, 1);
	__rwD3D9SetRenderTarget(0, RwD3D9RenderSurface);
	Quad::Render();
}

void PostProcessing::RenderFXAA()
{
	IDirect3DSurface9* screenSurface;
	auto screenExt = RASTEREXTFROMRASTER(mScreenRaster);
	screenExt->texture->GetSurfaceLevel(0, &screenSurface);
	RwD3DDevice->StretchRect(RwD3D9RenderSurface, NULL, screenSurface, NULL, D3DTEXF_NONE);
	screenSurface->Release();

	_rwD3D9SetPixelShader(mFXAAPixelShader);
	rwD3D9RWSetRasterStage(mScreenRaster, 0);
	__rwD3D9SetRenderTarget(0, RwD3D9RenderSurface);
	Quad::Render();
}

void PostProcessing::RenderDOF()
{}

void PostProcessing::RenderLensFlare()
{}
