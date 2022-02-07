#include "RenderTarget.h"
#include "RenderTargetManager.h"

RenderTarget::RenderTarget(D3DFORMAT format):mFormat(format)
{
	mSurface = nullptr;
	mRaster = nullptr;
}

RenderTarget::~RenderTarget()
{
	RwRasterDestroy(mRaster);
	RenderTargetManager::Remove(this);
}

void RenderTarget::Initialize()
{
	if(mRaster)
		return;

	mRaster = RwD3D9RasterCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight, mFormat, rwRASTERTYPECAMERATEXTURE);

	if(mRaster == nullptr)
		throw std::runtime_error("RenderTarget::Initialize");

	RenderTargetManager::Add(this);
}

void RenderTarget::Release()
{
	RwRasterDestroy(mRaster);
	mRaster = nullptr;
}

void RenderTarget::CopyFromSurface(LPSURFACE surface)
{
	auto texture = RASTEREXTFROMRASTER(mRaster)->texture;
	texture->GetSurfaceLevel(0, &mSurface);

	RwD3DDevice->StretchRect(surface == nullptr ? RwD3D9RenderSurface : surface, NULL, mSurface, NULL, D3DTEXF_NONE);
	
	mSurface->Release();
}

RwRaster* RenderTarget::GetRaster()
{
	return mRaster;
}

LPSURFACE RenderTarget::GetSurface()
{
	auto texture = RASTEREXTFROMRASTER(mRaster)->texture;
	texture->GetSurfaceLevel(0, &mSurface);

	// Release after GetSurfaceLevel to decrease reference count.
	mSurface->Release();

	return mSurface;
}

void RenderTarget::SetAsTexture(SamplerState* sampler, uint32_t stage)
{
	if (sampler)
		sampler->Apply(stage);

	_rwD3D9RWSetRasterStage(mRaster, stage);
}

void RenderTarget::SetAsRenderTarget(uint32_t index)
{
	RwD3D9SetRenderTarget(index, mRaster);

	// Keeps original surface
	CurrentRenderSurface[0] = RwD3D9RenderSurface;
}
