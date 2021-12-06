#include "Texture.h"
#include <filesystem>

Texture::Texture()
{
	mRaster = nullptr;
	mSurface = nullptr;
}

Texture::~Texture()
{
	SAFE_RELEASE(mSurface);
	RwRasterDestroy(mRaster);
	mRaster = nullptr;
}

void Texture::Initialize(RwUInt32 width, RwUInt32 height, D3DFORMAT format, RwUInt32 flags)
{
	RwBool validFormat = FALSE;
	
	if(flags & rwRASTERTYPEZBUFFER)
		validFormat = _rwD3D9CheckValidZBufferTextureFormat(format);
	else
		validFormat = _rwD3D9CheckValidCameraTextureFormat(format);
	
	if(!validFormat)
		throw std::invalid_argument("Texture::Initialize() - Invalid texture format");

	mRaster = RwD3D9RasterCreate(width, height, format, flags);

	if(mRaster == nullptr)
		throw std::runtime_error("Texture::Initialize");

	auto rasterExt = RASTEREXTFROMRASTER(mRaster);
	rasterExt->texture->GetSurfaceLevel(0, &mSurface);
}

void Texture::Initialize(RwChar* file)
{
	/*RwInt32 width, height, depth, flags;

	RwImage* image = nullptr;

	std::filesystem::path filepath = file;

	std::string name = file;
	name.find_last_of(".dds");

	if(filepath.extension() == ".dds")
		RwD3D9DDSTextureRead("DeferredSA/wave1");

	RwImage* image = RtPNGImageRead(file);
	RwImage* image = RtBMPImageRead(file);

	RwImageFindRasterFormat(image, 4, &width, &height, &depth, &flags);
	RwRaster* raster = RwRasterCreate(width, height, depth, flags);
	RwRasterSetFromImage(raster, image);*/
}

RwRaster* Texture::GetRaster()
{
	return mRaster;
}

IDirect3DSurface9* Texture::GetSurface()
{
	return mSurface;
}
