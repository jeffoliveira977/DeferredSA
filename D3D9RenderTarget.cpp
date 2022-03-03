#include "D3D9RenderTarget.h"

D3D9RenderTarget::D3D9RenderTarget(RwRaster* raster)  :
    D3D9BaseTexture(raster)
{
    Initialize();
}

D3D9RenderTarget::~D3D9RenderTarget()
{
    Unitialize();
}

void D3D9RenderTarget::Initialize()
{
    if (mD3D9Texture)
        return;

    if (mRaster == nullptr)
        return;

    HRESULT hr;
    auto rasExt = RASTEREXTFROMRASTER(mRaster);

    if (rasExt->cube)
    {     
        hr = _RwD3DDevice->CreateCubeTexture(mRaster->width, (mRaster->cFormat & rwRASTERFORMATMIPMAP) ? 0 : 1,
            (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0) | D3DUSAGE_RENDERTARGET, rasExt->d3dFormat, D3DPOOL_DEFAULT, (IDirect3DCubeTexture9**)&mD3D9Texture, nullptr);
    }
    else
    {
        hr = _RwD3DDevice->CreateTexture(mRaster->width, mRaster->height, (mRaster->cFormat & rwRASTERFORMATMIPMAP) ? 0 : 1, 
            (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0) | D3DUSAGE_RENDERTARGET,
            rasExt->d3dFormat, D3DPOOL_DEFAULT, &mD3D9Texture, nullptr);
    }

    if (FAILED(hr) || mD3D9Texture == nullptr)
    {
        Log::Fatal("D3D9RenderTarget::Initialize - failed to create d3d9 texture");
        throw std::runtime_error("failed to create d3d9 texture");
    }
}

void D3D9RenderTarget::Unitialize()
{
    SAFE_RELEASE(mD3D9Texture);
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
