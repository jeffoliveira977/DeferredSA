#pragma once
#include "CommonD.h"

class Texture
{
	RwRaster* mRaster;
	IDirect3DSurface9* mSurface;
public:
	Texture();
	~Texture();

	void Initialize(RwUInt32 width, RwUInt32 height, D3DFORMAT format, RwUInt32 flags);
	void Initialize(RwChar* file);
	RwRaster* GetRaster();
	IDirect3DSurface9* GetSurface();
};

