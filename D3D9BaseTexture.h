#pragma once
#include "CommonD.h"

using namespace DirectX;
using namespace std;

class D3D9BaseTexture 
{
public:
	RwRaster* mRaster;
	IDirect3DTexture9* mD3D9Texture;

	uint32_t mLevels;
	uint32_t mUsage;
	D3DFORMAT mFormat;
	D3DPOOL mPool;
	uint32_t mTextureType;


	IDirect3DTexture9* GetObject();

	uint32_t GetTextureType() { return mTextureType; }


	//D3D9BaseTexture(RwRaster* raster, uint32_t levels, uint32_t usage, D3DFORMAT format, D3DPOOL pool);

	
	//virtual ~D3D9BaseTexture();

	//virtual void Initialize();
	//virtual void Unitialize();

	//virtual void Lock(uint flags, uint level, void* pixelsIn);
	//virtual void Unlock();
};

