#pragma once
#include "D3D9BaseTexture.h"

class D3D9Texture: public D3D9BaseTexture
{
	IDirect3DTexture9* mD3D9Texture;
public:
	D3D9Texture(RwRaster*raster);
	~D3D9Texture();

	void Initialize();
	void Unitialize();

	void Lock(uint flags, uint level, void* pixelsIn);
	void Unlock(void* rasterIn);
};

