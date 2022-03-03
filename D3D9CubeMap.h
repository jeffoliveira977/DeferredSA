#pragma once
#include "D3D9BaseTexture.h"

class D3D9CubeMap : public D3D9BaseTexture
{	
public:
	D3D9CubeMap(RwRaster* raster);
	~D3D9CubeMap();

	void Initialize();
	void Unitialize();

	void Lock(uint flags, uint level, void* pixelsIn);
	void Unlock(void* rasterIn);
};

