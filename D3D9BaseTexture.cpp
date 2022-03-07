#include "D3D9BaseTexture.h"

IDirect3DTexture9* D3D9BaseTexture::GetObject()
{
	return mD3D9Texture;
}
//
//D3D9BaseTexture::D3D9BaseTexture(RwRaster* raster, uint32_t levels, uint32_t usage, D3DFORMAT format, D3DPOOL pool)
//{
//	mLevels = levels;
//	mUsage = usage;
//	mFormat = format;
//	mPool = pool;
//	mRaster = raster;
//	mD3D9Texture = nullptr;
//}

//D3D9BaseTexture::~D3D9BaseTexture()
//{
//	Log::Debug("D3D9BaseTexture::~D3D9BaseTexture");
//	mRaster = nullptr;
//}
//
//void D3D9BaseTexture::Initialize()
//{
//	Log::Debug("D3D9BaseTexture::Initialize");
//}
//
//void D3D9BaseTexture::Unitialize()
//{
//	Log::Debug("D3D9BaseTexture::Unitialize");
//}
//
//void D3D9BaseTexture::Lock(uint flags, uint level, void* pixelsIn)
//{
//	Log::Debug("D3D9BaseTexture::Lock");
//}
//
//void D3D9BaseTexture::Unlock()
//{
//	Log::Debug("D3D9BaseTexture::Unlock");
//}
