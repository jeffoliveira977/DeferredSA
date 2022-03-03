#include "D3D9Texture.h"

D3D9Texture::D3D9Texture(RwRaster* raster, uint32_t levels, uint32_t usage, D3DFORMAT format, D3DPOOL pool) :
    D3D9BaseTexture(raster, levels, usage, format, pool)
{
    Initialize();
}

D3D9Texture::~D3D9Texture()
{
    Unitialize();
}

void D3D9Texture::Initialize()
{
    if (mD3D9Texture)
    {
        Log::Warn("D3D9Texture::Initialize - texture not released");
        return;
    }
    
    if (mRaster == nullptr)
        return;

    auto rasExt = RASTEREXTFROMRASTER(mRaster);

    HRESULT hr;
    if (IS_D3DFORMAT_ZBUFFER(rasExt->d3dFormat))
    {
        hr = _RwD3DDevice->CreateTexture(mRaster->width, mRaster->height, 1, D3DUSAGE_DEPTHSTENCIL, rasExt->d3dFormat, D3DPOOL_DEFAULT, &mD3D9Texture, nullptr);
    }
    else
    {
        hr = _RwD3DDevice->CreateTexture(mRaster->width, mRaster->height, mLevels, 
            mUsage, mFormat, mPool, &mD3D9Texture, nullptr);
    }

    if (FAILED(hr) || mD3D9Texture == nullptr)
    {
        Log::Fatal("D3D9Texture::Initialize - failed to create d3d9 texture");
        throw std::runtime_error("failed to create d3d9 texture");
    }
}

void D3D9Texture::Unitialize()
{
    SAFE_RELEASE(mD3D9Texture);
}

void D3D9Texture::Lock(uint flags, uint level, void* pixelsIn)
{
    if (mRaster == nullptr || mD3D9Texture == nullptr)
        return;

    auto rasExt = RASTEREXTFROMRASTER(mRaster);
    auto hr = mD3D9Texture->GetSurfaceLevel(level, &(rasExt->lockedSurface));

    if (SUCCEEDED(hr))
        hr = rasExt->lockedSurface->LockRect(&rasExt->lockedRect, NULL, flags);
}

void D3D9Texture::Unlock(void* rasterIn)
{
    if (mRaster == nullptr || mD3D9Texture == nullptr)
        return;

    auto rasExt = RASTEREXTFROMRASTER(mRaster);

    rasExt->lockedSurface->UnlockRect();
    rasExt->lockedSurface->Release();
}
