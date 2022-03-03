#pragma once
#include "D3D9BaseTexture.h"

class D3D9DepthStencil : public D3D9BaseTexture
{
public:
	D3D9DepthStencil(RwRaster*raster, D3DFORMAT format);
	~D3D9DepthStencil();

	void Initialize();
	void Unitialize();
};

