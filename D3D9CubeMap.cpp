#include "D3D9CubeMap.h"
D3D9CubeMap::D3D9CubeMap(RwRaster* raster) :
    D3D9BaseTexture(raster)
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
        return;

    auto rasExt = RASTEREXTFROMRASTER(mRaster);
    auto hr = _RwD3DDevice->CreateCubeTexture(mRaster->width, (mRaster->cFormat & rwRASTERFORMATMIPMAP) ? 0 : 1, 
        (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0), rasExt->d3dFormat, D3DPOOL_MANAGED, (IDirect3DCubeTexture9**)&mD3D9Texture, nullptr);

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
