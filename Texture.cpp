#include "Texture.h"
#include <filesystem>

Texture::Texture(string file)
{
	mRaster = nullptr;
	mSurface = nullptr;

	mWidth = 0;
	mHeight = 0;
	mFormat = D3DFMT_A8R8G8B8;
	mFlags = 0;
	mFileName = file;
}

Texture::Texture(RwUInt32 width, RwUInt32 height, D3DFORMAT format, RwUInt32 flags)
{
	mRaster = nullptr;
	mSurface = nullptr;
	mWidth = width;
	mHeight = height;
	mFormat = format;
	mFlags = flags;
}

Texture::~Texture()
{
	SAFE_RELEASE(mSurface);
	RwRasterDestroy(mRaster);
	mRaster = nullptr;
}

void Texture::Initialize()
{
	if(mFileName.size())
	{
		RwInt32 width, height, depth, flags;

		RwImage* image = nullptr;

		std::filesystem::path filepath = mFileName;

		if(mFileName.find(".dds") != string::npos)
			RwD3D9DDSTextureRead(mFileName.c_str(), nullptr);
		else if(mFileName.find(".png") != string::npos)
			image = RtPNGImageRead(mFileName.c_str());
		else if(mFileName.find(".bmp") != string::npos)
			image = RtBMPImageRead(mFileName.c_str());

		RwImageFindRasterFormat(image, 4, &width, &height, &depth, &flags);
		mRaster = RwRasterCreate(width, height, depth, flags);
		RwRasterSetFromImage(mRaster, image);
	}
	else
	{
		RwBool validFormat = FALSE;

		if(mFlags & rwRASTERTYPEZBUFFER)
			validFormat = _rwD3D9CheckValidZBufferTextureFormat(mFormat);
		else
			validFormat = _rwD3D9CheckValidCameraTextureFormat(mFormat);

		if(!validFormat)
			throw std::invalid_argument("Texture::Initialize() - Invalid texture format");

		if(mFlags == rwRASTERTYPEZBUFFER)
			mRaster = RwRasterCreate(mWidth, mHeight, 32, rwRASTERTYPEZBUFFER);
		else
			mRaster = RwD3D9RasterCreate(mWidth, mHeight, mFormat, mFlags);

	}

	if(mRaster == nullptr)
		throw std::runtime_error("Texture::Initialize");

	//auto rasterExt = RASTEREXTFROMRASTER(mRaster);

	//if(mFlags == rwRASTERTYPEZBUFFER)
	//	mSurface = (LPSURFACE)rasterExt->texture;
	//else
	//	rasterExt->texture->GetSurfaceLevel(0, &mSurface);
}

void Texture::Release()
{
	//SAFE_RELEASE(mSurface);
	RwRasterDestroy(mRaster);
	mRaster = nullptr;
}

RwRaster* Texture::GetRaster()
{
	return mRaster;
}

IDirect3DSurface9* Texture::GetSurface()
{
	return mSurface;
}

void Texture::Apply(SamplerState* sampler, uint32_t stage)
{
	if (sampler)
		sampler->Apply(stage);

	_rwD3D9RWSetRasterStage(mRaster, stage);
}
