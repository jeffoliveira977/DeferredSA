#include "RenderTarget.h"
#include "RenderTargetManager.h"

RenderTarget::RenderTarget(D3DFORMAT format):mFormat(format)
{
	mSurface = nullptr;
	mRaster = nullptr;
}

RenderTarget::~RenderTarget()
{
  
			std::ofstream myfile;
			myfile.open("example2.txt");
			myfile << "Writing this to a file.\n";
			myfile << "Writing this to a file.\n";
			myfile << "Writing this to a file.\n";
			myfile << "Writing this to a file.\n";
			myfile.close();

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
