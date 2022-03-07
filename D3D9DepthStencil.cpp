#include "D3D9DepthStencil.h"

D3D9DepthStencil::D3D9DepthStencil(RwRaster* raster, uint32_t levels, uint32_t usage, D3DFORMAT format, D3DPOOL pool) /*:
    D3D9BaseTexture(raster, 1, 0, format, D3DPOOL_DEFAULT)*/
{
    Initialize();
    mTextureType = rwRASTERTYPEZBUFFER;
}

D3D9DepthStencil::~D3D9DepthStencil()
{
    Log::Debug("D3D9DepthStencil::~D3D9DepthStencil");
    Unitialize();
}

void D3D9DepthStencil::Initialize()
{
    if (mD3D9Texture)
    {
        Log::Warn("D3D9DepthStencil::Initialize - texture not released");
        return;
    }

    if (mRaster == nullptr)
    {
        Log::Warn("D3D9DepthStencil::Initialize - invalid raster pointer");
        return;
    }

    auto rasExt = RASTEREXTFROMRASTER(mRaster);
    auto hr = CheckError(_RwD3DDevice->CreateDepthStencilSurface(mRaster->width, mRaster->height, mFormat,
        Present.MultiSampleType, Present.MultiSampleQuality, false, (IDirect3DSurface9**)&mD3D9Texture, nullptr), 
        "D3D9DepthStencil::Initialize - failed to create d3d9 depth stencil surface");

    if (FAILED(hr) || mD3D9Texture == nullptr)
    {
        Log::Fatal("D3D9DepthStencil::Initialize - failed to create d3d9 depth stencil surface");
        throw std::runtime_error("failed to create d3d9 depth stencil surface");
    }

    Log::Debug("D3D9DepthStencil::Initialize");
}

void D3D9DepthStencil::Unitialize()
{
    auto surface = (IDirect3DSurface9*)mD3D9Texture;
    SAFE_RELEASE(surface);
    Log::Debug("D3D9DepthStencil::Unitialize");
}

void D3D9DepthStencil::Lock(uint flags, uint level, void* pixelsIn)
{
}

void D3D9DepthStencil::Unlock(void* rasterIn)
{
}
