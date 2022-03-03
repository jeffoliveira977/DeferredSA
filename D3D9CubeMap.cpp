#include "D3D9CubeMap.h"
D3D9CubeMap::D3D9CubeMap(RwRaster* raster, uint32_t levels, uint32_t usage, D3DFORMAT format, D3DPOOL pool) :
    D3D9BaseTexture(raster, levels, usage, format, pool)
{
    Initialize();
}

D3D9CubeMap::~D3D9CubeMap()
{
    Unitialize();
}

void D3D9CubeMap::Initialize()
{
    if (mD3D9Texture)
    {
        Log::Warn("D3D9CubeMap::Initialize - texture not released");
        return;
    }

    auto rasExt = RASTEREXTFROMRASTER(mRaster);
    auto hr = _RwD3DDevice->CreateCubeTexture(mRaster->width, mLevels, 
        mUsage, mFormat, mPool, (IDirect3DCubeTexture9**)&mD3D9Texture, nullptr);

    if (FAILED(hr) || mD3D9Texture == nullptr)
    {
        Log::Fatal("D3D9CubeMap::Initialize - failed to create d3d9 cube texture");
        throw std::runtime_error("failed to create d3d9 cube texture");
    }
}

void D3D9CubeMap::Unitialize()
{
    SAFE_RELEASE(mD3D9Texture);
}

void D3D9CubeMap::Lock(uint flags, uint level, void* pixelsIn)
{
    if (mRaster == nullptr || mD3D9Texture == nullptr)
        return;

    auto rasExt = RASTEREXTFROMRASTER(mRaster);
    auto hr = ((IDirect3DCubeTexture9*)mD3D9Texture)->GetCubeMapSurface((D3DCUBEMAP_FACES)rasExt->face, level, &(rasExt->lockedSurface));

    if (SUCCEEDED(hr))
        hr = rasExt->lockedSurface->LockRect(&rasExt->lockedRect, nullptr, flags);
}

void D3D9CubeMap::Unlock(void* rasterIn)
{
    if (mRaster == nullptr || mD3D9Texture == nullptr)
        return;

    auto rasExt = RASTEREXTFROMRASTER(mRaster);

    rasExt->lockedSurface->UnlockRect();
    rasExt->lockedSurface->Release();
}
