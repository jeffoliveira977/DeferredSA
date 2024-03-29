#include "D3D9RenderTarget.h"

D3D9RenderTarget::D3D9RenderTarget(RwRaster* raster, uint32_t levels, uint32_t usage, D3DFORMAT format, D3DPOOL pool)  /*:
    D3D9BaseTexture(raster, levels, usage, format, pool)*/
{
    Initialize();
    mTextureType = rwRASTERTYPECAMERATEXTURE;
}

D3D9RenderTarget::~D3D9RenderTarget()
{
    Log::Debug("D3D9RenderTarget::~D3D9RenderTarget");
    Unitialize();
}

void D3D9RenderTarget::Initialize()
{
    if (mD3D9Texture)
    {
        Log::Warn("D3D9RenderTarget::Initialize - texture not released");
        return;
    }

    if (mRaster == nullptr)
    {
        Log::Warn("D3D9RenderTarget::Initialize - invalid raster pointer");
        return;
    }

    HRESULT hr;
    auto rasExt = RASTEREXTFROMRASTER(mRaster);

    if (rasExt->cube)
    {
        hr = CheckError(_RwD3DDevice->CreateCubeTexture(mRaster->width, mLevels,
            mUsage, mFormat, mPool, (IDirect3DCubeTexture9**)&mD3D9Texture, nullptr),
            "D3D9RenderTarget::Initialize - failed to create d3d9 texture");
    }
    else
    {
        hr = CheckError(_RwD3DDevice->CreateTexture(mRaster->width, mRaster->height,mLevels,
           mUsage,
            mFormat, mPool, &mD3D9Texture, nullptr),
            "D3D9RenderTarget::Initialize - failed to create d3d9 texture");
    }

    if (FAILED(hr) || mD3D9Texture == nullptr)
    {
        Log::Fatal("D3D9RenderTarget::Initialize - failed to create d3d9 texture");
        throw std::runtime_error("failed to create d3d9 texture");
    }
    Log::Debug("D3D9RenderTarget::Initialize");
}

void D3D9RenderTarget::Unitialize()
{
    Log::Debug("D3D9RenderTarget::Unitialize");
    if (mRaster == nullptr)
    {
        Log::Warn("D3D9RenderTarget::Unitialize - invalid raster pointer");
        return;
    }
    auto rasExt = RASTEREXTFROMRASTER(mRaster);

    if (rasExt->cube)
    {
        auto cubemap = (IDirect3DCubeTexture9*)mD3D9Texture;
        SAFE_RELEASE(cubemap);
    }
    else
        SAFE_RELEASE(mD3D9Texture);
   // Log::Debug("D3D9RenderTarget::Initialize");
}

void D3D9RenderTarget::Lock(uint flags, uint level, void* pixelsIn)
{
    if (mRaster == nullptr)
        return;

    LPSURFACE       surface;
    HRESULT hr;
    auto rasExt = RASTEREXTFROMRASTER(mRaster);

    if (rasExt->cube)
        hr = reinterpret_cast<IDirect3DCubeTexture9*>(mD3D9Texture)->GetCubeMapSurface((D3DCUBEMAP_FACES)rasExt->face, level, &surface);
    else
        hr = mD3D9Texture->GetSurfaceLevel(level, &surface);
   
    D3DSURFACE_DESC d3dDesc;
    surface->GetDesc(&d3dDesc);

    hr = _RwD3DDevice->CreateOffscreenPlainSurface(d3dDesc.Width, d3dDesc.Height, d3dDesc.Format, D3DPOOL_SYSTEMMEM, &(rasExt->lockedSurface), nullptr);
    if (SUCCEEDED(hr))
    {
        _RwD3DDevice->GetRenderTargetData(surface, rasExt->lockedSurface);

        if (SUCCEEDED(hr))
        {
            hr = rasExt->lockedSurface->LockRect(&rasExt->lockedRect, nullptr, flags);

            if (FAILED(hr))
                rasExt->lockedSurface->Release();
        }
    }
}

void D3D9RenderTarget::Unlock(void* rasterIn)
{
    if (mRaster == nullptr)
        return;

    auto rasExt = RASTEREXTFROMRASTER(mRaster);
    rasExt->lockedSurface->UnlockRect();
    rasExt->lockedSurface->Release();
}
