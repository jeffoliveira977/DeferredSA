#pragma once
#include "RWCommon.h"

class D3D9BaseTexture
{
protected:
	RwRaster* mRaster;
	IDirect3DTexture9* mD3D9Texture;
public:

	IDirect3DTexture9* GetObject();

	D3D9BaseTexture(RwRaster* raster);
	~D3D9BaseTexture();

	virtual void Initialize();
	virtual void Unitialize();

	virtual void Lock(uint flags, uint level, void* pixelsIn);
	virtual void Unlock();
};

