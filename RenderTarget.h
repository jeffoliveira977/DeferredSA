#pragma once
#include "CommonD.h"
#include "SamplerState.h"
class RenderTarget
{
public:
	RenderTarget(D3DFORMAT format);
	~RenderTarget();

	void Initialize();
	void Release();
	void CopyFromSurface(LPSURFACE surface);

	RwRaster* GetRaster();
	LPSURFACE GetSurface();

	void SetAsTexture(SamplerState*sampler, uint32_t stage);
	void SetAsRenderTarget(uint32_t index);

private:
	D3DFORMAT mFormat;
	RwRaster* mRaster;
	LPSURFACE mSurface;
};

