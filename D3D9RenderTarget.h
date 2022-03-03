#pragma once
#include "D3D9BaseTexture.h"

class D3D9RenderTarget : public D3D9BaseTexture
{
public:
	D3D9RenderTarget(RwRaster* raster, uint32_t levels, uint32_t usage, D3DFORMAT format, D3DPOOL pool);
	~D3D9RenderTarget();

	void Initialize();
	void Unitialize();

	void Lock(uint flags, uint level, void* pixelsIn);
	void Unlock(void* rasterIn);
};

