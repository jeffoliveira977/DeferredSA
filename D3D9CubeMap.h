#pragma once
#include "D3D9BaseTexture.h"

class D3D9CubeMap : public D3D9BaseTexture
{	
public:
	D3D9CubeMap(RwRaster* raster, uint32_t levels, uint32_t usage, D3DFORMAT format, D3DPOOL pool);
	~D3D9CubeMap();

	void Initialize();
	void Unitialize();

	void Lock(uint flags, uint level, void* pixelsIn);
	void Unlock(void* rasterIn);
};

