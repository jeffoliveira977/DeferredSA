#include "D3D9DepthStencil.h"

D3D9DepthStencil::D3D9DepthStencil(RwRaster* raster) :
    D3D9BaseTexture(raster)
{
    Initialize();
}

D3D9DepthStencil::~D3D9DepthStencil()
{
    Unitialize();
}

void D3D9DepthStencil::Initialize()
{
    if (mD3D9Texture)
        return;

    if (mRaster == nullptr)
        return;

    auto rasExt = RASTEREXTFROMRASTER(mRaster);
    auto hr = _RwD3DDevice->CreateDepthStencilSurface(mRaster->width, mRaster->height, rasExt->d3dFormat,
        Present.MultiSampleType, Present.MultiSampleQuality, false, (IDirect3DSurface9**)&mD3D9Texture, nullptr);

    if (FAILED(hr) || mD3D9Texture == nullptr)
    {
        Log::Fatal("D3D9DepthStencil::Initialize - failed to create d3d9 depth stencil surface");
        throw std::runtime_error("failed to create d3d9 depth stencil surface");
    }
}

void D3D9DepthStencil::Unitialize()
{
    SAFE_RELEASE(mD3D9Texture);
}
