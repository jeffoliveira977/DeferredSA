#pragma once
#include "D3D9BaseTexture.h"

class D3D9DepthStencil : public D3D9BaseTexture
{
public:
	D3D9DepthStencil(RwRaster*raster, uint32_t levels, uint32_t usage, D3DFORMAT format, D3DPOOL pool);
	~D3D9DepthStencil();

	void Initialize();
	void Unitialize();

	void Lock(uint flags, uint level, void* pixelsIn);
	void Unlock(void* rasterIn);
};

