#pragma once
#include "CommonD.h"

class Texture
{
	RwRaster* mRaster;
	IDirect3DSurface9* mSurface;

	RwUInt32 mWidth;
	RwUInt32 mHeight;
	D3DFORMAT mFormat; 
	RwUInt32 mFlags;
	string mFileName;

public:
	Texture(RwUInt32 width, RwUInt32 height, D3DFORMAT format, RwUInt32 flags); 
	Texture(string file);
	~Texture();

	void Initialize();
	void Release();
	RwRaster* GetRaster();
	IDirect3DSurface9* GetSurface();
};

