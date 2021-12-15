#include "RenderTarget.h"

RenderTarget::RenderTarget(D3DFORMAT format)
{
	mSurface = nullptr;
	mRaster = nullptr;
	mFormat = format;
}

RenderTarget::~RenderTarget()
{
	RwRasterDestroy(mRaster);
}

void RenderTarget::Initialize()
{
	if(mRaster)
		return;

	mRaster = RwD3D9RasterCreate(RsGlobal.maximumWidth, RsGlobal.maximumHeight, mFormat, rwRASTERTYPECAMERATEXTURE);
	
	if(mRaster == nullptr)
		throw std::runtime_error("RenderTarget::Initialize");
	
	auto texture = RASTEREXTFROMRASTER(mRaster)->texture;
	texture->GetSurfaceLevel(0, &mSurface);

	// we need to release to decrease reference count
	mSurface->Release(); 
}

void RenderTarget::Release()
{
	RwRasterDestroy(mRaster);
	mRaster = nullptr;
}

RwRaster* RenderTarget::GetRaster()
{
	return mRaster;
}

LPSURFACE RenderTarget::GetSurface()
{
	return mSurface;
}
