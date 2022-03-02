#pragma once
#include "D3D9BaseTexture.h"

class D3D9CubeMap : public D3D9BaseTexture
{	
IDirect3DCubeTexture9* mD3D9Texture;
public:
	D3D9CubeMap(RwRaster* raster);
	~D3D9CubeMap();

	void Initialize();
	void Unitialize();

	void Lock(uint flags, uint level, void* pixelsIn);
	void Unlock(void* rasterIn);
};

