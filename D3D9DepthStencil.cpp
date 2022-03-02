#include "D3D9DepthStencil.h"

D3D9DepthStencil::D3D9DepthStencil(RwRaster* raster) :
    D3D9BaseTexture(raster),
    mSurface(nullptr)
{
    Initialize();
}

D3D9DepthStencil::~D3D9DepthStencil()
{
    Unitialize();
}

void D3D9DepthStencil::Initialize()
{
    if (mSurface)
        return;

    if (mRaster == nullptr)
        return;

    auto rasExt = RASTEREXTFROMRASTER(mRaster);
    auto hr = _RwD3DDevice->CreateDepthStencilSurface(mRaster->width, mRaster->height, rasExt->d3dFormat,
        Present.MultiSampleType, Present.MultiSampleQuality, false, &mSurface, nullptr);

    if (FAILED(hr) || mSurface == nullptr)
    {
        Log::Fatal("D3D9DepthStencil::Initialize - failed to create d3d9 depth stencil surface");
        throw std::runtime_error("failed to create d3d9 depth stencil surface");
    }
}

void D3D9DepthStencil::Unitialize()
{
    SAFE_RELEASE(mSurface);
}
