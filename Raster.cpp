#include "Raster.h"
#include "GTADef.h"

#undef RwD3D9RasterCreate

#define SETFORMATINFO(_format, _alpha, _depth, _rwformat)   \
    _rwD3D9PixelFormatInfo[_format].alpha = _alpha; \
    _rwD3D9PixelFormatInfo[_format].depth = _depth; \
    _rwD3D9PixelFormatInfo[_format].rwFormat = _rwformat


_rwD3D9FormatInfo _rwD3D9PixelFormatInfo[MAX_PIXEL_FORMATS];

std::list<RwRaster*> RasterList;

RwInt32 _rwD3D9FindMSB(RwInt32 num)
{
    auto pos = -1;

    while (num)
    {
        pos++;
        num >>= 1;
    }

    return pos;
}

RwInt32& _RwD3D9RasterExtOffset = *(RwInt32*)0xB4E9E0;    /* Raster extension offset */

RwBool Raster::D3D9RasterLock(void* pixelsIn, void* rasterIn, RwInt32 accessMode)
{
    RwUInt8** pixels = (RwUInt8**)pixelsIn;
    RwRaster* raster = (RwRaster*)rasterIn;
    RwRaster* topRaster;
    _rwD3D9RasterExt* topRasExt;
    RwUInt8         mipLevel;
    RwUInt32        flags;
    HRESULT         hr = D3D_OK;

    /* Can't rasters that have already been locked */
    if (raster->cpPixels != NULL)
        return FALSE;

    auto rasExt = RASTEREXTFROMRASTER(raster);

    /* Get the top level raster as this is the only one with a valid texture */
    topRaster = raster;
    //while ((topRaster = topRaster->parent) != topRaster->parent) {};

    topRasExt = RASTEREXTFROMRASTER(topRaster);

    /* Prepare lock info */
    mipLevel = (RwUInt8)((accessMode & (RwInt32)0xFF00) >> 8);

    flags = D3DLOCK_NOSYSLOCK;

    if (!(accessMode & rwRASTERLOCKWRITE))
        flags |= D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_READONLY;

    switch (raster->cType & rwRASTERTYPEMASK)
    {
    case rwRASTERTYPENORMAL:
    case rwRASTERTYPETEXTURE:
    {
        if (topRasExt->cube)
            hr = reinterpret_cast<LPDIRECT3DCUBETEXTURE9>(topRasExt->texture)->GetCubeMapSurface((D3DCUBEMAP_FACES)topRasExt->face, mipLevel, &(rasExt->lockedSurface));
        else
            hr = topRasExt->texture->GetSurfaceLevel(mipLevel, &(rasExt->lockedSurface));

        if (SUCCEEDED(hr))
        {
           // if (topRaster == raster)
                hr = rasExt->lockedSurface->LockRect(&rasExt->lockedRect, NULL, flags);
           /* else
            {
                RECT rect;

                rect.left = raster->nOffsetX;
                rect.top = raster->nOffsetY;
                rect.right = raster->nOffsetX + raster->width;
                rect.bottom = raster->nOffsetY + raster->height;
                hr = rasExt->lockedSurface->LockRect(&rasExt->lockedRect, &rect, flags);
            }*/
        }

       /* if (SUCCEEDED(hr))
        {
            if (accessMode & rwRASTERLOCKREAD)
                raster->privateFlags |= rwRASTERPIXELLOCKEDREAD;

            if (accessMode & rwRASTERLOCKWRITE)
                raster->privateFlags |= rwRASTERPIXELLOCKEDWRITE;
        }
        else
            return FALSE;*/
    }
    break;

    case rwRASTERTYPECAMERATEXTURE:
    case rwRASTERTYPECAMERA:
    {
        LPSURFACE       surface;
        D3DSURFACE_DESC d3dDesc;

        if ((raster->cType & rwRASTERTYPEMASK) == rwRASTERTYPECAMERATEXTURE)
        {
            if (topRasExt->cube)
                hr = reinterpret_cast<LPDIRECT3DCUBETEXTURE9>(topRasExt->texture)->GetCubeMapSurface((D3DCUBEMAP_FACES)topRasExt->face, mipLevel, &surface);
            else
                hr = topRasExt->texture->GetSurfaceLevel(mipLevel, &surface);
        }
        else
        {
            if (topRasExt->swapChain)
                surface = (LPSURFACE)topRasExt->texture;
            else
                surface = RwD3D9RenderSurface;

            hr = D3D_OK;
        }

        if (SUCCEEDED(hr))
        {
            surface->GetDesc(&d3dDesc);

            /* Create a system memory surface */
            hr = _RwD3DDevice->CreateOffscreenPlainSurface(d3dDesc.Width, d3dDesc.Height, d3dDesc.Format, D3DPOOL_SYSTEMMEM, &(rasExt->lockedSurface), NULL);

            if (SUCCEEDED(hr))
            {
                /* Copy the contents if needed */
                if (accessMode & rwRASTERLOCKREAD)
                    _RwD3DDevice->GetRenderTargetData(surface, rasExt->lockedSurface);

                if (SUCCEEDED(hr))
                {
                    if (topRaster == raster)
                        hr = rasExt->lockedSurface->LockRect(&rasExt->lockedRect, NULL, flags);
                    else
                    {
                        RECT rect;

                        rect.left = raster->nOffsetX;
                        rect.top = raster->nOffsetY;
                        rect.right = raster->nOffsetX + raster->width;
                        rect.bottom = raster->nOffsetY + raster->height;

                        hr = rasExt->lockedSurface->LockRect(&rasExt->lockedRect, &rect, flags);
                    }

                    if (SUCCEEDED(hr))
                    {
                        if (accessMode & rwRASTERLOCKREAD)
                            raster->privateFlags |= rwRASTERPIXELLOCKEDREAD;

                        if (accessMode & rwRASTERLOCKWRITE)
                            raster->privateFlags |= rwRASTERPIXELLOCKEDWRITE;
                    }
                }

                if (FAILED(hr))
                    rasExt->lockedSurface->Release();
            }

            if ((raster->cType & rwRASTERTYPEMASK) == rwRASTERTYPECAMERATEXTURE)
                surface->Release();
        }

        if (FAILED(hr))
        {
            return FALSE;
        }
    }
    break;

    case rwRASTERTYPEZBUFFER:
    default:
        return FALSE;
        break;
    }

    /* Pixels */
    raster->cpPixels = (RwUInt8*)rasExt->lockedRect.pBits;

    /* Cache original width, height & stride */
    raster->originalWidth = raster->width;
    raster->originalHeight = raster->height;

    /* Mip level width, height & stride */
    raster->width = raster->width >> mipLevel;
    raster->height = raster->height >> mipLevel;

    /* Clamp width and height to 1 */
    if (raster->width == 0)
        raster->width = 1;

    if (raster->height == 0)
        raster->height = 1;

    /* Set the stride */
    raster->stride = rasExt->lockedRect.Pitch;

    /* Cache the mip level locked */
    rasExt->lockedMipLevel = mipLevel;

    /* Save off the pixel pointer */
    (*pixels) = raster->cpPixels;

    return TRUE;
}

RwBool Raster::D3D9RasterUnlock(void* unused1, void* rasterIn, RwInt32 unused3)
{
   auto raster = (RwRaster*)rasterIn;

    /* Can't unlock rasters that have not been locked */
    if (raster->cpPixels == NULL)
        return FALSE;

    auto rasExt = RASTEREXTFROMRASTER(raster);

    switch (raster->cType & rwRASTERTYPEMASK)
    {
    case rwRASTERTYPENORMAL:
    case rwRASTERTYPETEXTURE:
    {
        rasExt->lockedSurface->UnlockRect();
        rasExt->lockedSurface->Release();

        /* Restore the original width, height & stride */
        raster->width = raster->originalWidth;
        raster->height = raster->originalHeight;

        raster->stride = 0;
        raster->cpPixels = NULL;

        //if (/*(raster->privateFlags & rwRASTERPIXELLOCKEDWRITE) &&*/
        //    (raster->cFormat & (rwRASTERFORMATAUTOMIPMAP >> 8)) &&
        //    (rasExt->lockedMipLevel == 0) &&
        //    (rasExt->automipmapgen == 0))
        //{
        //    RwRaster* topRaster;

        //    raster->privateFlags = (RwUInt8)(raster->privateFlags & ~rwRASTERPIXELLOCKED);
        //    rasExt->lockedMipLevel = (RwUInt8)-1;

        //    /* Get the top level raster as this is the only one with a valid texture */
        //    topRaster = raster;
        //   // while ((topRaster = topRaster->parent) != topRaster->parent) {};

        //    RwTextureRasterGenerateMipmaps(topRaster, NULL);
        //}
        //else
        //    raster->privateFlags = (RwUInt8)(raster->privateFlags & ~rwRASTERPIXELLOCKED);
    }
    break;

    case rwRASTERTYPECAMERATEXTURE:
    case rwRASTERTYPECAMERA:
    {
        /* Restore the original width, height & stride */
        raster->width = raster->originalWidth;
        raster->height = raster->originalHeight;

        raster->stride = 0;
        raster->cpPixels = NULL;

        /* Unlock system memory surface */
        rasExt->lockedSurface->UnlockRect();

        /* Update video memory surface if needed */
        //if (raster->privateFlags & rwRASTERPIXELLOCKEDWRITE)
        //{
        //    LPSURFACE           surface;
        //    HRESULT             hr;
        //    RwRaster* topRaster;
        //    _rwD3D9RasterExt* topRasExt;


        //    /* Get the top level raster as this is the only one with a valid texture */
        //    topRaster = raster;
        //    while ((topRaster = topRaster->parent) != topRaster->parent) {};

        //    topRasExt = RASTEREXTFROMRASTER(topRaster);

        //    if ((raster->cType & rwRASTERTYPEMASK) == rwRASTERTYPECAMERATEXTURE)
        //    {
        //        if (topRasExt->cube)
        //            hr = reinterpret_cast<LPDIRECT3DCUBETEXTURE9>(topRasExt->texture)->GetCubeMapSurface((D3DCUBEMAP_FACES)topRasExt->face, rasExt->lockedMipLevel, &surface);
        //        else
        //            hr = topRasExt->texture->GetSurfaceLevel(rasExt->lockedMipLevel, &surface);
        //    }
        //    else
        //    {
        //        if (topRasExt->swapChain)
        //            surface = (LPSURFACE)topRasExt->texture;
        //        else
        //            surface = RwD3D9RenderSurface;

        //        hr = D3D_OK;
        //    }

        //    if (SUCCEEDED(hr))
        //    {
        //        if (topRaster == raster)
        //        {
        //            hr = _RwD3DDevice->UpdateSurface(rasExt->lockedSurface, NULL, surface, NULL);
        //        }
        //        else
        //        {
        //            RECT rect;
        //            POINT pt;

        //            rect.left = raster->nOffsetX;
        //            rect.top = raster->nOffsetY;
        //            rect.right = rect.left + raster->width;
        //            rect.bottom = rect.top + raster->height;

        //            pt.x = raster->nOffsetX;
        //            pt.y = raster->nOffsetY;

        //            hr = _RwD3DDevice->UpdateSurface(rasExt->lockedSurface, &rect, surface, &pt);
        //        }

        //        if ((raster->cType & rwRASTERTYPEMASK) == rwRASTERTYPECAMERATEXTURE)
        //            surface->Release();
        //    }

        //    rasExt->lockedSurface->Release();

        //    raster->privateFlags = (RwUInt8)(raster->privateFlags & ~rwRASTERPIXELLOCKED);

        //    if ((raster->cType & rwRASTERTYPEMASK) == rwRASTERTYPECAMERATEXTURE)
        //    {
        //        if ((raster->cFormat & (rwRASTERFORMATAUTOMIPMAP >> 8)) &&
        //            (rasExt->lockedMipLevel == 0))
        //        {
        //            rasExt->lockedMipLevel = (RwUInt8)-1;

        //            RwTextureRasterGenerateMipmaps(topRaster, NULL);
        //        }
        //    }
        //}
        //else
        {
            rasExt->lockedSurface->Release();
            raster->privateFlags = (RwUInt8)(raster->privateFlags & ~rwRASTERPIXELLOCKED);
        }
    }
    break;

    case rwRASTERTYPEZBUFFER:
    default:
        return FALSE;
        break;
    }

    return TRUE;
}

RwInt32 Raster::D3D9FindCorrectRasterFormat(RwRasterType type, RwInt32 flags)
{
    RwUInt32    format = flags & rwRASTERFORMATMASK;

    switch (type)
    {
    case rwRASTERTYPENORMAL:
    case rwRASTERTYPETEXTURE:
        if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATDEFAULT)
        {
            if ((format & rwRASTERFORMATPAL8) == 0)
            {
                /*
                 * By default textures are 8888, but make mipmap generation flag and
                 * palletized flags persist
                 */
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_A8R8G8B8))
                    format |= rwRASTERFORMAT8888;
                else
                {
                    if (_rwD3D9CheckValidTextureFormat(D3DFMT_A4R4G4B4))
                        format |= rwRASTERFORMAT4444;
                    else
                        format |= rwRASTERFORMAT1555;

                }
            }
        }
        else
        {
            if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT8888)
            {
                if (!_rwD3D9CheckValidTextureFormat(D3DFMT_A8R8G8B8))
                {
                    format &= (~rwRASTERFORMAT8888);

                    if (_rwD3D9CheckValidTextureFormat(D3DFMT_A4R4G4B4))
                        format |= rwRASTERFORMAT4444;
                    else
                        format |= rwRASTERFORMAT1555;
                }
            }
            else if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT888)
            {
                if (!_rwD3D9CheckValidTextureFormat(D3DFMT_X8R8G8B8))
                {
                    format &= (~rwRASTERFORMAT888);
                    format |= rwRASTERFORMAT565;
                }
            }
            else if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT4444)
            {
                if (!_rwD3D9CheckValidTextureFormat(D3DFMT_A4R4G4B4))
                {
                    format &= (~rwRASTERFORMAT4444);
                    format |= rwRASTERFORMAT1555;
                }
            }
            else if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATLUM8)
            {
                if (!_rwD3D9CheckValidTextureFormat(D3DFMT_L8))
                {
                    format &= (~rwRASTERFORMATLUM8);

                    if (_rwD3D9CheckValidTextureFormat(D3DFMT_X8R8G8B8))
                        format |= D3DFMT_X8R8G8B8;
                    else
                        format |= rwRASTERFORMAT565;
                }
            }
        }
        break;

    case rwRASTERTYPECAMERATEXTURE:
        if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATDEFAULT)
        {
            if (_rwD3D9CheckValidCameraTextureFormat(D3DFMT_X8R8G8B8))
                format |= rwRASTERFORMAT888;
            else
                format |= rwRASTERFORMAT565;
        }
        else
        {
            const RwInt32 depth = _rwRasterConvTable[FMT2TBL(format)].depth;

            if (depth == 32)
            {
                if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT8888)
                {
                    format &= (~rwRASTERFORMAT8888);
                    format |= rwRASTERFORMAT4444;
                }
                else if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT888)
                {
                    format &= (~rwRASTERFORMAT888);
                    format |= rwRASTERFORMAT565;
                }
                else
                    format = rwRASTERFORMAT565;
            }

            if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT4444)
            {
                if (!_rwD3D9CheckValidCameraTextureFormat(D3DFMT_A4R4G4B4))
                {
                    format &= (~rwRASTERFORMAT4444);
                    format |= rwRASTERFORMAT1555;
                }
            }

            if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT1555)
            {
                if (!_rwD3D9CheckValidCameraTextureFormat(D3DFMT_A1R5G5B5))
                {
                    format &= (~rwRASTERFORMAT1555);
                    format |= rwRASTERFORMAT565;
                }
            }

            if (depth == 16)
            {
                if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT4444)
                {
                    format &= (~rwRASTERFORMAT4444);
                    format |= rwRASTERFORMAT8888;
                }
                else if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT1555)
                {
                    format &= (~rwRASTERFORMAT1555);
                    format |= rwRASTERFORMAT8888;
                }
                else if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT565)
                {
                    format &= (~rwRASTERFORMAT565);
                    format |= rwRASTERFORMAT888;
                }
                else
                    format = rwRASTERFORMAT888;
            }

            if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT8888)
            {
                if (!_rwD3D9CheckValidCameraTextureFormat(D3DFMT_A8R8G8B8))
                {
                    format &= (~rwRASTERFORMAT8888);
                    format |= rwRASTERFORMAT888;
                }
            }
        }
        break;

    case rwRASTERTYPECAMERA:
        if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATDEFAULT)
            format = rwD3D9FindRwFormat(Present.BackBufferFormat);
        break;

    case rwRASTERTYPEZBUFFER:
        if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATDEFAULT)
        {
            format = rwRASTERFORMAT32;
        }
        break;

    default:
        break;
    }

    return format;
}

RwBool Raster::D3D9CheckRasterFormat(void* rasterIn, RwInt32 flags)
{
    auto raster = (RwRaster*)rasterIn;
    auto format = flags & rwRASTERFORMATMASK;

    /* Copy over types */
    raster->cType = (RwUInt8)(flags & rwRASTERTYPEMASK);
    raster->cFlags = (RwUInt8)(flags & ~rwRASTERTYPEMASK);

    format = (RwRasterType)D3D9FindCorrectRasterFormat((RwRasterType)raster->cType, flags);

    raster->cFormat = (RwUInt8)(format >> 8);
    raster->depth = _rwRasterConvTable[FMT2TBL(format)].depth;

    return TRUE;
}

RwBool Raster::rwD3D9SetRasterFormat(void* rasterIn, RwInt32 flags)
{
    auto raster = (RwRaster*)rasterIn;
    auto format = flags & rwRASTERFORMATMASK;

    /* Copy over types */
    raster->cType = (RwUInt8)(flags & rwRASTERTYPEMASK);
    raster->cFlags = (RwUInt8)(flags & ~rwRASTERTYPEMASK);

    switch (raster->cType & rwRASTERTYPEMASK)
    {
    case rwRASTERTYPENORMAL:
    case rwRASTERTYPETEXTURE:
        if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATDEFAULT)
        {
            if (_rwD3D9CheckValidTextureFormat(D3DFMT_A8R8G8B8))
            {
                format |= rwRASTERFORMAT8888;
                raster->depth = 32;
            }
            else
            {
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_A4R4G4B4))
                    format |= rwRASTERFORMAT4444;
                else
                    format |= rwRASTERFORMAT1555;

                raster->depth = 16;
            }
        }
        else
        {
            /* No support for 4 bits palettes */
            if (format & rwRASTERFORMATPAL4)
                return FALSE;
            else
            {
                auto d3dFormat = _rwRasterConvTable[FMT2TBL(format)].format;

                if (IS_D3DFORMAT_ZBUFFER(d3dFormat))
                {
                    if (_rwD3D9CheckValidZBufferTextureFormat(d3dFormat))
                        raster->depth = _rwRasterConvTable[FMT2TBL(format)].depth;
                    else
                        return FALSE;
                }
                else
                {
                    if (_rwD3D9CheckValidTextureFormat(d3dFormat))
                        raster->depth = _rwRasterConvTable[FMT2TBL(format)].depth;
                    else
                        return FALSE;
                }
            }
        }

        /* check automipmapgen */
        if ((format & rwRASTERFORMATPAL8) == 0 &&
            (flags & (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP)) == (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP))
        {
            if (_rwD3D9CheckAutoMipmapGenTextureFormat(_rwRasterConvTable[FMT2TBL(format)].format))
                RASTEREXTFROMRASTER(raster)->automipmapgen = 1;
        }
        break;

    case rwRASTERTYPECAMERATEXTURE:
        if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATDEFAULT)
        {
            if (_rwD3D9CheckValidCameraTextureFormat(D3DFMT_X8R8G8B8))
            {
                format |= rwRASTERFORMAT888;
                raster->depth = 32;
            }
            else
            {
                format |= rwRASTERFORMAT565;
                raster->depth = 16;
            }
        }
        else
        {
            auto depth = _rwRasterConvTable[FMT2TBL(format)].depth;

            if (_rwD3D9CheckValidCameraTextureFormat(_rwRasterConvTable[FMT2TBL(format)].format))
                raster->depth = depth;
            else
                return FALSE;
        }

        /* check automipmapgen */
        if ((flags & (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP)) == (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP))
        {
            if (_rwD3D9CheckAutoMipmapGenCameraTextureFormat(_rwRasterConvTable[FMT2TBL(format)].format))
                RASTEREXTFROMRASTER(raster)->automipmapgen = 1;
        }
        break;

    case rwRASTERTYPECAMERA:
        if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATDEFAULT)
            format = rwD3D9FindRwFormat(Present.BackBufferFormat);
        else
        {
            if (rwD3D9FindRwFormat(Present.BackBufferFormat) != format)
                return FALSE;
        }

        raster->depth = _RwD3D9AdapterInformation.displayDepth;
        break;

    case rwRASTERTYPEZBUFFER:
            raster->depth = 32;
            format = rwRASTERFORMAT32;
        break;

    default:
        return FALSE;
        break;
    }

    raster->cFormat = (RwUInt8)(format >> 8);

    return TRUE;
}

RwUInt32 Raster::_rwD3D9ImageFindFormat(RwImage* image)
{
    RwUInt32    format;

    auto depth = RwImageGetDepth(image);

    const RwInt32   width = RwImageGetWidth(image);
    const RwInt32   height = RwImageGetHeight(image);
    const RwUInt8* cpIn = image->cpPixels;
    RwInt32         y;
    RwUInt32        alphaBits = 0;

    for (y = 0; y < height; y++)
    {
        const RwRGBA* rpInCur = (const RwRGBA*)cpIn;
        RwInt32         x;

        for (x = 0; x < width; x++)
        {
            if (rpInCur->alpha < 0xff)
            {
                /* lower 4 bits of the alpha channel are discarded in 4444 */
                if (rpInCur->alpha > 0xf)
                {
                    alphaBits = 8;
                    break;
                }
                else
                    alphaBits = 1;
            }

            /* Next pixel */
            rpInCur++;
        }

        if (alphaBits >= 8)
            break;

        cpIn += RwImageGetStride(image);
    }

    if (alphaBits >= 8)
    {
        if (_rwD3D9CheckValidTextureFormat(D3DFMT_A8R8G8B8))
            format = rwRASTERFORMAT8888;
        else if (_rwD3D9CheckValidTextureFormat(D3DFMT_A4R4G4B4))
            format = rwRASTERFORMAT4444;
        else
            format = rwRASTERFORMAT1555;
    }
    else if (alphaBits)
    {
        if (_rwD3D9CheckValidTextureFormat(D3DFMT_A8R8G8B8))
            format = rwRASTERFORMAT8888;
        else
            format = rwRASTERFORMAT1555;
    }
    else
    {
        if (_rwD3D9CheckValidTextureFormat(D3DFMT_X8R8G8B8))
            format = rwRASTERFORMAT888;
        else
            format = rwRASTERFORMAT565;
    }
    return format;
}

RwUInt32 Raster::rwD3D9FindRwFormat(D3DFORMAT d3dFormat)
{
    return _rwD3D9PixelFormatInfo[d3dFormat].rwFormat;
}

RwBool Raster::_rwD3D9CheckValidTextureFormat(RwInt32 format)
{
    return SUCCEEDED(_RwDirect3DObject->CheckDeviceFormat(_RwD3DAdapterIndex, _RwD3DAdapterType, _RwD3D9AdapterInformation.mode.Format, 0, 
        D3DRTYPE_TEXTURE, (D3DFORMAT)format));
}

RwBool Raster::_rwD3D9CheckValidCameraTextureFormat(RwInt32 format)
{
    auto adapterFormat = _RwD3D9AdapterInformation.mode.Format;
    auto hr = _RwDirect3DObject->CheckDeviceFormat(_RwD3DAdapterIndex, _RwD3DAdapterType, adapterFormat, D3DUSAGE_RENDERTARGET, 
        D3DRTYPE_TEXTURE, (D3DFORMAT)format);

    if (SUCCEEDED(hr))
        hr = _RwDirect3DObject->CheckDepthStencilMatch(_RwD3DAdapterIndex, _RwD3DAdapterType, adapterFormat, 
        (D3DFORMAT)format, Present.AutoDepthStencilFormat);
    
    return SUCCEEDED(hr);
}

RwBool Raster::_rwD3D9CheckValidZBufferTextureFormat(RwInt32 format)
{
    auto  adapterFormat = _RwD3D9AdapterInformation.mode.Format;

    auto hr = _RwDirect3DObject->CheckDeviceFormat(_RwD3DAdapterIndex, _RwD3DAdapterType, adapterFormat, D3DUSAGE_DEPTHSTENCIL, 
        D3DRTYPE_TEXTURE, (D3DFORMAT)format);

    if (SUCCEEDED(hr))
        hr = _RwDirect3DObject->CheckDepthStencilMatch(_RwD3DAdapterIndex, _RwD3DAdapterType,  adapterFormat, adapterFormat, (D3DFORMAT)format);
    
    return SUCCEEDED(hr);
}

RwBool Raster::_rwD3D9CheckValidCubeTextureFormat(RwInt32 format)
{
    return SUCCEEDED(_RwDirect3DObject->CheckDeviceFormat(_RwD3DAdapterIndex, _RwD3DAdapterType, _RwD3D9AdapterInformation.mode.Format, 0,
        D3DRTYPE_CUBETEXTURE, (D3DFORMAT)format));
}

RwBool Raster::_rwD3D9CheckValidCameraCubeTextureFormat(RwInt32 format)
{
    auto adapterFormat = _RwD3D9AdapterInformation.mode.Format;

    auto hr = _RwDirect3DObject->CheckDeviceFormat(_RwD3DAdapterIndex, _RwD3DAdapterType, adapterFormat, D3DUSAGE_RENDERTARGET,
        D3DRTYPE_CUBETEXTURE, (D3DFORMAT)format);

    if (SUCCEEDED(hr))
        hr = _RwDirect3DObject->CheckDepthStencilMatch(_RwD3DAdapterIndex, _RwD3DAdapterType, adapterFormat,
        (D3DFORMAT)format, Present.AutoDepthStencilFormat);


    return SUCCEEDED(hr);
}

RwBool Raster::_rwD3D9CheckAutoMipmapGenCubeTextureFormat(RwInt32 format)
{
    if (_RwD3D9DeviceCaps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP)
    {
        auto hr = _RwDirect3DObject->CheckDeviceFormat(_RwD3DAdapterIndex, _RwD3DAdapterType, _RwD3D9AdapterInformation.mode.Format, D3DUSAGE_AUTOGENMIPMAP, 
            D3DRTYPE_CUBETEXTURE, (D3DFORMAT)format);

        return hr == D3D_OK;
    }
    else
        return FALSE;
}

RwBool Raster::_rwD3D9CheckAutoMipmapGenCameraTextureFormat(RwInt32 format)
{
    if (_RwD3D9DeviceCaps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP)
    {
        auto hr = _RwDirect3DObject->CheckDeviceFormat(_RwD3DAdapterIndex, _RwD3DAdapterType, _RwD3D9AdapterInformation.mode.Format,
            D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, (D3DFORMAT)format);

        return hr == D3D_OK;
    }
    else
        return FALSE;
}

RwBool Raster::_rwD3D9CheckValidZBufferFormat(RwInt32 format)
{
    auto adapterFormat = _RwD3D9AdapterInformation.mode.Format;

    auto hr = _RwDirect3DObject->CheckDeviceFormat(_RwD3DAdapterIndex, _RwD3DAdapterType, adapterFormat, D3DUSAGE_DEPTHSTENCIL, 
        D3DRTYPE_SURFACE, (D3DFORMAT)format);

    if (SUCCEEDED(hr))
        hr = _RwDirect3DObject->CheckDepthStencilMatch(_RwD3DAdapterIndex, _RwD3DAdapterType, adapterFormat, adapterFormat, (D3DFORMAT)format);

    return SUCCEEDED(hr);
}


RwBool  Raster::_rwD3D9CheckAutoMipmapGenTextureFormat(RwInt32 format)
{
    if (_RwD3D9DeviceCaps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP)
    {
        auto adapterFormat = _RwD3D9AdapterInformation.mode.Format;

        auto hr = _RwDirect3DObject->CheckDeviceFormat(_RwD3DAdapterIndex, _RwD3DAdapterType, adapterFormat, D3DUSAGE_AUTOGENMIPMAP, 
            D3DRTYPE_TEXTURE, (D3DFORMAT)format);
       
        return hr == D3D_OK;
    }
    else
        return FALSE;
}

RwBool Raster::_rwD3D9RasterHasAlpha(const RwRaster* raster)
{
    if (raster == nullptr)
        return false;

    return RASTEREXTFROMCONSTRASTER(raster)->alpha;
}

RwBool Raster::_rwD3D9TextureHasAlpha(const RwTexture* texture)
{
    if (texture->raster == nullptr)
        return false;

    return RASTEREXTFROMCONSTRASTER(texture->raster)->alpha;
}

RwBool Raster::_rwD3D9NativeTextureRead(void* streamIn, void* textureIn, RwInt32 unused3 __RWUNUSED__)
{
    RwStream* stream = (RwStream*)streamIn;
    RwRaster* raster = (RwRaster*)NULL;
    RwTexture* texture;
    RwUInt32            length, version;
    _rwD3D9NativeRaster   nativeRaster;
    _rwD3D9NativeTexture  nativeTexture;
    _rwD3D9RasterExt* rasExt;

    if (!RwStreamFindChunk(stream, rwID_STRUCT, &length, &version))
        return FALSE;

    if (!((version >= rwLIBRARYBASEVERSION) &&
        (version <= rwLIBRARYCURRENTVERSION)))
        return FALSE;

    /*
     * Don't have to worry about endianness as this is platform specific
     */

     /* ID, filter & addressing modes */
    if (RwStreamRead(stream, (void*)&nativeTexture, sizeof(_rwD3D9NativeTexture)) != sizeof(_rwD3D9NativeTexture))
        return FALSE;

    if (rwID_PCD3D9 != nativeTexture.id)
        return FALSE;

    /* Native raster data structure */
    if (RwStreamRead(stream, (void*)&nativeRaster, sizeof(_rwD3D9NativeRaster)) != sizeof(_rwD3D9NativeRaster))
        return FALSE;

    if (nativeRaster.flags & IS_COMPRESSED)
    {
        RwUInt32 dxtFormat;

        /* Get compressed format */
        dxtFormat = nativeRaster.d3dFormat;

        if (!_rwD3D9CheckValidTextureFormat(dxtFormat))
            return FALSE;

        RwUInt32 levels;

        /* Create a raster */
        raster = RwRasterCreate(nativeRaster.width, nativeRaster.height,
            (RwInt32)nativeRaster.depth, nativeRaster.type | nativeRaster.format | rwRASTERDONTALLOCATE);

        if (raster == nullptr)
            return FALSE;

        /* Get the raster extension */
        rasExt = RASTEREXTFROMRASTER(raster);

        levels = ((RwRasterGetFormat(raster) & rwRASTERFORMATMIPMAP) ? nativeRaster.numMipLevels : 1);

        if ((nativeRaster.flags & IS_CUBE) != 0)
        {
            rasExt->cube = TRUE;

            /* check automipmapgen */
            if ((nativeRaster.format & (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP)) == (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP))
            {
                if (_rwD3D9CheckAutoMipmapGenCubeTextureFormat(dxtFormat))
                    rasExt->automipmapgen = 1;
            }

            if (FAILED(_RwD3DDevice->CreateCubeTexture(raster->width,
                levels, (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0), (D3DFORMAT)dxtFormat, D3DPOOL_MANAGED, (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture), NULL)))
            {
                RwRasterDestroy(raster);

                return FALSE;
            }
            else
            {
                /* Remove the rwRASTERDONTALLOCATE flag so it will get destroyed */
                raster->cFlags &= ~rwRASTERDONTALLOCATE;

                /* Copy alpha flag */
                rasExt->alpha = (nativeRaster.flags & HAS_ALPHA) != 0;

                /* Mark as cube texture */
                rasExt->cube = TRUE;

                /* Mark as compressed texture */
                rasExt->compressed = TRUE;

                rasExt->d3dFormat = (D3DFORMAT)dxtFormat;
            }
        }
        else
        {
            /* check automipmapgen */
            if ((nativeRaster.format & (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP)) == (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP))
            {
                if (_rwD3D9CheckAutoMipmapGenTextureFormat(dxtFormat))
                {
                    rasExt->automipmapgen = 1;
                }
            }

            /* Create the texture, if mipped 0 gets all the mip levels */
            if (FAILED(_RwD3DDevice->CreateTexture(raster->width, raster->height,
                levels, (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0), (D3DFORMAT)dxtFormat, D3DPOOL_MANAGED, &(rasExt->texture), NULL)))
            {
                RwRasterDestroy(raster);

                return FALSE;
            }
            else
            {
                /* Remove the rwRASTERDONTALLOCATE flag so it will get destroyed */
                raster->cFlags &= ~rwRASTERDONTALLOCATE;

                /* Copy alpha flag */
                rasExt->alpha = (nativeRaster.flags & HAS_ALPHA) != 0;

                /* Mark as compressed texture */
                rasExt->compressed = TRUE;
                rasExt->d3dFormat = (D3DFORMAT)dxtFormat;
            }
        }
    }
    else if ((nativeRaster.flags & IS_CUBE) != 0)
    {
        RwUInt32 levels;

        /* Create a raster */
        raster = RwRasterCreate(nativeRaster.width, nativeRaster.height, (RwInt32)nativeRaster.depth, nativeRaster.type | nativeRaster.format | rwRASTERDONTALLOCATE);

        if (raster == nullptr)
            return FALSE;

        /* Get the raster extension */
        rasExt = RASTEREXTFROMRASTER(raster);

        levels = ((RwRasterGetFormat(raster) & rwRASTERFORMATMIPMAP) ? nativeRaster.numMipLevels : 1);

        /* Mark as cube texture */
        rasExt->cube = TRUE;

        /* check automipmapgen */
        if ((nativeRaster.format & (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP)) == (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP))
        {
            if (_rwD3D9CheckAutoMipmapGenCubeTextureFormat(nativeRaster.d3dFormat))
                rasExt->automipmapgen = 1;
        }

        if (FAILED(_RwD3DDevice->CreateCubeTexture(raster->width, levels,
            (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0), nativeRaster.d3dFormat, D3DPOOL_MANAGED, (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture), NULL)))
        {
            RwRasterDestroy(raster);

            return FALSE;
        }
        else
        {
            /* Remove the rwRASTERDONTALLOCATE flag so it will get destroyed */
            raster->cFlags &= ~rwRASTERDONTALLOCATE;

            /* Copy alpha flag */
            rasExt->alpha = (nativeRaster.flags & HAS_ALPHA) != 0;
        }
    }
    else if (nativeRaster.format & ~(rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP)) /* Is an standard RW format? */
    {
        /* Create a raster */
        raster = RwRasterCreate(nativeRaster.width, nativeRaster.height, (RwInt32)nativeRaster.depth, nativeRaster.type | nativeRaster.format);

        if (!raster)
            return FALSE;

        /* Get the raster extension */
        rasExt = RASTEREXTFROMRASTER(raster);

        rasExt->compressed = 0;
    }
    else
    {
        /* Create a raster */
        raster = RwD3D9RasterCreate(nativeRaster.width, nativeRaster.height, nativeRaster.d3dFormat, 
            nativeRaster.type | (nativeRaster.format & (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP)));

        if (!raster)
            return FALSE;

        /* Get the raster extension */
        rasExt = RASTEREXTFROMRASTER(raster);
    }

    /* check automipmap support */
    if (nativeRaster.flags & USE_AUTOMIPMAPGEN)
    {
        if (rasExt->automipmapgen == 0)
        {
            RwRasterDestroy(raster);

            return FALSE;
        }
    }

    /* Check format and size */
    if (nativeRaster.format == (RwUInt32)RwRasterGetFormat(raster) &&
        nativeRaster.d3dFormat == rasExt->d3dFormat &&
        nativeRaster.width == raster->width &&
        nativeRaster.height == raster->height)
    {
        RwUInt32    autoMipmap, face, numMipLevels, i;

        /* Remove AUTOMIPMAP flag to avoid unlock invoking mipmap create */
        autoMipmap = raster->cFormat & (rwRASTERFORMATAUTOMIPMAP >> 8);
        raster->cFormat &= ~autoMipmap;

        /* Prepare support for cube textures */
        if (rasExt->cube)
            face = 6;
        else
            face = 1;

        if (rasExt->automipmapgen)
            numMipLevels = 1; /* Read only first level */
        else
            numMipLevels = nativeRaster.numMipLevels;

        rasExt->face = 0;

        /* Load mips into raster */
        do
        {
            for (i = 0; i < numMipLevels; i++)
            {
                RwUInt8* pixels;
                RwUInt32    size;

                pixels = RwRasterLock(raster, (RwUInt8)i, rwRASTERLOCKWRITE);

                /* Size in bytes of mip to load */
                if (RwStreamRead(stream, (void*)&size, sizeof(RwUInt32)) != sizeof(RwUInt32))
                {
                    RwRasterUnlock(raster);
                    RwRasterDestroy(raster);

                    return FALSE;
                }

                if (size == 0)
                {
                    auto rasterExt = RASTEREXTFROMRASTER(raster);

                    // simply copy the data from previous level
                    if (raster->cType == rwRASTERTYPETEXTURE && rasterExt->compressed && rasterExt->texture)
                    {
                        D3DLOCKED_RECT rect;
                        rasterExt->texture->LockRect(i - 1, &rect, NULL, D3DLOCK_READONLY);

                        RwUInt32 sz = 16;
                        if (rasterExt->d3dFormat == D3DFMT_DXT1)
                            sz = 8;

                        memcpy(pixels, rect.pBits, sz);

                        rasterExt->texture->UnlockRect(i - 1);
                    }
                }

                /* Read the mip level */
                if (RwStreamRead(stream, (void*)pixels, size) != size)
                {
                    RwRasterUnlock(raster);
                    RwRasterDestroy(raster);

                    return FALSE;
                }

                RwRasterUnlock(raster);
            }

            if (rasExt->cube)
                rasExt->face += 1;
        } while (--face);

        /* Restore auto mip flag */
        raster->cFormat |= autoMipmap;
    }
    else
    {
        RwRasterDestroy(raster);
        return FALSE;
    }

    texture = RwTextureCreate(raster);
    if (!texture)
    {
        RwRasterDestroy(raster);
        return FALSE;
    }

    RwTextureSetFilterMode(texture, nativeTexture.filterAndAddress & 0xFF);
    RwTextureSetAddressingU(texture, (nativeTexture.filterAndAddress >> 8) & 0x0F);
    RwTextureSetAddressingV(texture, (nativeTexture.filterAndAddress >> 12) & 0x0F);
    RwTextureSetName(texture, nativeTexture.name);
    RwTextureSetMaskName(texture, nativeTexture.mask);

    *((RwTexture**)textureIn) = texture;

    return TRUE;
}

RwBool Raster::_rwD3D9CubeRasterCreate(RwRaster* raster, RwUInt32 d3dformat, RwInt32 mipmapLevels)
{
    _rwD3D9RasterExt* rasExt;
    RwUInt32 levels;
    HRESULT hr;

    rasExt = RASTEREXTFROMRASTER(raster);
    rasExt->cube = TRUE;
    rasExt->d3dFormat = (D3DFORMAT)d3dformat;

    if (_RwD3D9DeviceCaps.CubeTextureFilterCaps & (D3DPTFILTERCAPS_MIPFPOINT | D3DPTFILTERCAPS_MIPFLINEAR))
        levels = mipmapLevels;
    else
        levels = 1;

    if ((raster->cType & rwRASTERTYPEMASK) != rwRASTERTYPECAMERATEXTURE)
    {
        hr =
            _RwD3DDevice->CreateCubeTexture(raster->width, levels,
            (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0), rasExt->d3dFormat, D3DPOOL_MANAGED, (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture), NULL);
    }
    else
    {
        hr =
            _RwD3DDevice->CreateCubeTexture(raster->width, levels,
            (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0) | D3DUSAGE_RENDERTARGET, rasExt->d3dFormat, D3DPOOL_DEFAULT, (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture), NULL);

        if (SUCCEEDED(hr))
            rxD3D9VideoMemoryRasterListAdd(raster);
    }

    if (FAILED(hr))
        return FALSE;

    /* Remove the rwRASTERDONTALLOCATE flag so it will get destroyed */
    raster->cFlags &= ~rwRASTERDONTALLOCATE;

    return TRUE;
}

void  Raster::_rwD3D9CubeRasterSelectFace(RwRaster* raster, RwUInt32 face)
{
    if (raster == nullptr)
        return;

    RASTEREXTFROMRASTER(raster)->face = face;
}

RwBool Raster::_rwD3D9RasterIsCubeRaster(const RwRaster* raster)
{
    if (raster == nullptr)
        return false;

    return RASTEREXTFROMCONSTRASTER(raster)->cube;
}

void Raster::_rwD3D9CheckRasterSize(RwInt32* width, RwInt32* height, RwUInt32 rasterFlags)
{

    /* Clamp the raster to the max texture size */
    if (*width > (RwInt32)_RwD3D9DeviceCaps.MaxTextureWidth)
        *width = _RwD3D9DeviceCaps.MaxTextureWidth;

    if (*height > (RwInt32)_RwD3D9DeviceCaps.MaxTextureHeight)
        *height = _RwD3D9DeviceCaps.MaxTextureHeight;

    if ((rasterFlags & rwRASTERTYPEMASK) == rwRASTERTYPENORMAL ||
        (rasterFlags & rwRASTERFORMATMIPMAP) == 0)
    {
        if ((_RwD3D9DeviceCaps.TextureCaps & D3DPTEXTURECAPS_POW2) != 0 &&
            (_RwD3D9DeviceCaps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) == 0)
        {
            /* Make it a power of two (round down) */
            *width = 1 << _rwD3D9FindMSB(*width);
            *height = 1 << _rwD3D9FindMSB(*height);
        }
    }
    else
    {
        if (_RwD3D9DeviceCaps.TextureCaps & D3DPTEXTURECAPS_POW2)
        {
            /* Make it a power of two (round down) */
            *width = 1 << _rwD3D9FindMSB(*width);
            *height = 1 << _rwD3D9FindMSB(*height);
        }
    }

    if (_RwD3D9DeviceCaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
        *width = min(*width, *height);  

    return;
}

RwBool  Raster::rwD3D9CreateTextureRaster(RwRaster* raster, _rwD3D9RasterExt* rasExt)
{
    RwUInt32    rasFormat;
    RwUInt32    pixelFormat;
    HRESULT     hr;


    rasFormat = RwRasterGetFormat(raster);
    pixelFormat = rasFormat & rwRASTERFORMATPIXELFORMATMASK;


    rasExt->d3dFormat = _rwRasterConvTable[FMT2TBL(rasFormat)].format;

    /* Does this raster format have an alpha component */
    rasExt->alpha = _rwRasterConvTable[FMT2TBL(rasFormat)].alpha;

    /* Check the size of the texture */
    _rwD3D9CheckRasterSize(&(raster->width), &(raster->height), raster->cType | rasFormat);

    /* Create the texture, if mipped 0 gets all the mip levels */
    if (rasExt->cube)
    {
        hr = _RwD3DDevice->CreateCubeTexture(raster->width,
            (rasFormat & rwRASTERFORMATMIPMAP) ? 0 : 1, (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0),
            rasExt->d3dFormat, D3DPOOL_MANAGED, (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture), NULL);
    }
    else
    {
        if (IS_D3DFORMAT_ZBUFFER(rasExt->d3dFormat))
        {
            hr = _RwD3DDevice->CreateTexture(raster->width, raster->height, 1,
                D3DUSAGE_DEPTHSTENCIL, rasExt->d3dFormat, D3DPOOL_DEFAULT, &(rasExt->texture), NULL);

            if (SUCCEEDED(hr))
                rxD3D9VideoMemoryRasterListAdd(raster);
        }
        else
        {
            hr = _RwD3DDevice->CreateTexture(raster->width, raster->height,
                (rasFormat & rwRASTERFORMATMIPMAP) ? 0 : 1, (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0),
                rasExt->d3dFormat, D3DPOOL_MANAGED, &(rasExt->texture), NULL);
        }
    }

    if (FAILED(hr))
        return FALSE;

    return TRUE;
}

RwBool Raster::_rwD3D9RasterCreate(void* unused1 __RWUNUSED__, void* rasterIn, RwInt32 flags)
{
    RwRaster* raster = (RwRaster*)rasterIn;
    _rwD3D9RasterExt* rasExt = RASTEREXTFROMRASTER(raster);

    /* Initialise structure to something sensible */
    raster->cType = (RwUInt8)(flags & rwRASTERTYPEMASK);
    raster->cFlags = (RwUInt8)(flags & ~rwRASTERTYPEMASK);
    raster->cpPixels = NULL;
    raster->palette = NULL;

    rasExt->texture = NULL;
    rasExt->palette = NULL;
    rasExt->alpha = 0;
    rasExt->cube = 0;
    rasExt->automipmapgen = 0;
    rasExt->compressed = 0;
    rasExt->face = 0;
    rasExt->lockedMipLevel = (RwUInt8)-1;
    rasExt->lockedSurface = NULL;
    rasExt->d3dFormat = D3DFMT_UNKNOWN;

    /* Set up the raster format and depth */
    if (!rwD3D9SetRasterFormat(raster, flags))
        return FALSE;

    /*
     * If it is not a camera buffer, then we need to allocate real memory for it
     */
    if ((raster->width) && (raster->height))
    {
        switch (raster->cType & rwRASTERTYPEMASK)
        {
        case rwRASTERTYPENORMAL:
        {
            if (!(raster->cFlags & rwRASTERDONTALLOCATE))
            {
                if (!rwD3D9CreateTextureRaster(raster, rasExt))
                    return FALSE;
            }
        }
        break;

        case rwRASTERTYPETEXTURE:
        {
            if (!(raster->cFlags & rwRASTERDONTALLOCATE))
            {
                if (!rwD3D9CreateTextureRaster(raster, rasExt))
                    return FALSE;
            }
        }
        break;

        case rwRASTERTYPECAMERATEXTURE:
        {
            if (!(raster->cFlags & rwRASTERDONTALLOCATE))
            {
                if (!rwD3D9CreateCameraTextureRaster(raster, rasExt))
                    return FALSE;

                rxD3D9VideoMemoryRasterListAdd(raster);
            }
        }
        break;

        case rwRASTERTYPEZBUFFER:
        {
            if (!(raster->cFlags & rwRASTERDONTALLOCATE))
            {
                if (!rwD3D9CreateZBufferRaster(raster, rasExt))
                    return FALSE;

                rxD3D9VideoMemoryRasterListAdd(raster);
            }
        }
        break;

        case rwRASTERTYPECAMERA:
        {
            RECT    rect;

            GetClientRect(*WindowHandle, &rect);

            if ((raster->cFlags & rwRASTERDONTALLOCATE) != 0 ||
                (rect.right >= raster->width && rect.bottom >= raster->height))
            {

                /* Find the depth of the backbuffer */
                if (Present.BackBufferFormat == D3DFMT_A8R8G8B8 ||
                    Present.BackBufferFormat == D3DFMT_X8R8G8B8 ||
                    Present.BackBufferFormat == D3DFMT_A2R10G10B10)
                    raster->depth = 32;
                else if (Present.BackBufferFormat == D3DFMT_R5G6B5 ||
                    Present.BackBufferFormat == D3DFMT_X1R5G5B5 ||
                    Present.BackBufferFormat == D3DFMT_A1R5G5B5)
                    raster->depth = 16;
                else
                    raster->depth = 0;

                /* Very little to do here - the camera is global */
                raster->stride = 0;
                raster->cpPixels = NULL;
                raster->cFlags = (RwUInt8)rwRASTERDONTALLOCATE;

                raster->originalWidth = raster->width;
                raster->originalHeight = raster->height;

                rasExt->d3dFormat = Present.BackBufferFormat;

                rxD3D9VideoMemoryRasterListAdd(raster);

                return TRUE;
            }
            else
                return FALSE;
        }
        break;

        }
    }
    else
    {
        raster->cFlags = rwRASTERDONTALLOCATE;   /* Not allocated */

        raster->stride = 0;
        if (raster->depth == 0)
            raster->depth = 16;

        /* Camera Sub rasters need to be added */
        if (raster->cType == rwRASTERTYPECAMERA)
            rxD3D9VideoMemoryRasterListAdd(raster);

        return TRUE;
    }

    return TRUE;
}

RwBool Raster::rwD3D9CreateZBufferRaster(RwRaster* raster, _rwD3D9RasterExt* rasExt)
{
    RECT        rect;

    if (_rwD3D9CheckValidZBufferFormat(D3DFMT_D24S8))
        rasExt->d3dFormat = D3DFMT_D24S8;
    else if (_rwD3D9CheckValidZBufferFormat(D3DFMT_D24X4S4))
        rasExt->d3dFormat = D3DFMT_D24X4S4;
    else if (_rwD3D9CheckValidZBufferFormat(D3DFMT_D32))
        rasExt->d3dFormat = D3DFMT_D32;
    else
        rasExt->d3dFormat = D3DFMT_D24X8;

    rasExt->alpha = 0;

    GetClientRect(*WindowHandle, &rect);

    if (rect.right != raster->width || rect.bottom != raster->height)
    {
        HRESULT hr;

        /* Remove managed resources from memory when creating a video memory resource */
        _RwD3DDevice->EvictManagedResources();

        hr = _RwD3DDevice->CreateDepthStencilSurface(raster->width, raster->height, rasExt->d3dFormat,
            Present.MultiSampleType, Present.MultiSampleQuality, FALSE, (LPSURFACE*)&(rasExt->texture), NULL);

        if (hr == D3DERR_OUTOFVIDEOMEMORY)
            return FALSE;
        else if (FAILED(hr))
            return FALSE;
    }
    else
        rasExt->texture = (LPDIRECT3DTEXTURE9)RwD3D9DepthStencilSurface;

    return TRUE;
}

RwBool Raster::rwD3D9CreateCameraTextureRaster(RwRaster* raster, _rwD3D9RasterExt* rasExt)
{
    RwUInt32    rasFormat;
    RwUInt32    pixelFormat;
    HRESULT     hr;

    rasFormat = RwRasterGetFormat(raster);
    pixelFormat = rasFormat & rwRASTERFORMATPIXELFORMATMASK;

    /* rwRASTERTYPECAMERATEXTURE can not be palletized */
    if ((rasFormat & rwRASTERFORMATPAL4) || (rasFormat & rwRASTERFORMATPAL8))
        return FALSE;

    rasExt->d3dFormat = _rwRasterConvTable[FMT2TBL(rasFormat)].format;

    /* Does this raster format have an alpha component */
    rasExt->alpha = _rwRasterConvTable[FMT2TBL(rasFormat)].alpha;

    /* Check the size of the texture */
    _rwD3D9CheckRasterSize(&(raster->width), &(raster->height), raster->cType | rasFormat);

    /* Remove managed resources from memory when creating a video memory resource */
    _RwD3DDevice->EvictManagedResources();

    /* Create the texture, if mipped 0 gets all the mip levels */
    if (rasExt->cube)
    {
        hr = _RwD3DDevice->CreateCubeTexture(raster->width, (rasFormat & rwRASTERFORMATMIPMAP) ? 0 : 1,
            (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0) | D3DUSAGE_RENDERTARGET, rasExt->d3dFormat, D3DPOOL_DEFAULT, (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture), NULL);
    }
    else
    {
        hr = _RwD3DDevice->CreateTexture(raster->width, raster->height,
            (rasFormat & rwRASTERFORMATMIPMAP) ? 0 : 1,
            (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0) | D3DUSAGE_RENDERTARGET,
            rasExt->d3dFormat, D3DPOOL_DEFAULT, &(rasExt->texture), NULL);
    }

    return SUCCEEDED(hr);
}

RwRaster* Raster::RwD3D9RasterCreate(RwUInt32 width, RwUInt32 height, RwUInt32 d3dFormat, RwUInt32 flags)
{
    RwRaster* raster = NULL;
    _rwD3D9RasterExt* rasExt;
    HRESULT             hr;
    RwUInt32            usage, pool;

    /* Remove mipmap if raster type is normal or zbuffer */
    if ((flags & rwRASTERTYPEMASK) == rwRASTERTYPENORMAL ||
        (flags & rwRASTERTYPEMASK) == rwRASTERTYPEZBUFFER)
    {
        flags &= ~(rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP);
    }

    /* Create a raster */
    raster = RwRasterCreate(width, height, 0,  flags | rwRASTERDONTALLOCATE);
    if (raster == nullptr)
        return nullptr;

    /* Get the raster extension */
    rasExt = RASTEREXTFROMRASTER(raster);

    /* check automipmapgen */
    if ((flags & (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP)) == (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP))
    {
        if (_rwD3D9CheckAutoMipmapGenTextureFormat(d3dFormat))
        {
            rasExt->automipmapgen = 1;
        }
    }

    hr = E_FAIL;

    usage = (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0);

    if ((flags & rwRASTERTYPECAMERATEXTURE) == rwRASTERTYPECAMERATEXTURE)
    {
        pool = D3DPOOL_DEFAULT;

        if (_rwD3D9CheckValidCameraTextureFormat(d3dFormat))
        {
            usage |= D3DUSAGE_RENDERTARGET;

            hr = D3D_OK;
        }
    }
    else
    {
        if (IS_D3DFORMAT_ZBUFFER(d3dFormat))
        {
            pool = D3DPOOL_DEFAULT;

            if (_rwD3D9CheckValidZBufferTextureFormat(d3dFormat))
            {
                usage |= D3DUSAGE_DEPTHSTENCIL;

                hr = D3D_OK;
            }
        }
        else
        {
            pool = D3DPOOL_MANAGED;

            if (_rwD3D9CheckValidTextureFormat(d3dFormat))
            {
                hr = D3D_OK;
            }
        }
    }

    if (FAILED(hr))
    {
        RwRasterDestroy(raster);
       return nullptr;
    }

    hr = _RwD3DDevice->CreateTexture(raster->width, raster->height, (flags & rwRASTERFORMATMIPMAP) ? 0 : 1,
        usage, (D3DFORMAT)d3dFormat, (D3DPOOL)pool, &(rasExt->texture), NULL);

    /* Create the texture, if mipped 0 gets all the mip levels */
    if (FAILED(hr))
    {
        RwRasterDestroy(raster);

       return nullptr;
    }
    else
    {
        if (pool == D3DPOOL_DEFAULT)
            rxD3D9VideoMemoryRasterListAdd(raster);

        /* Remove the rwRASTERDONTALLOCATE flag so it will get destroyed */
        raster->cFlags &= ~rwRASTERDONTALLOCATE;

        /* Remove any raster pixel format */
        raster->cFormat &= ~(rwRASTERFORMATPIXELFORMATMASK >> 8);

        rasExt->d3dFormat = (D3DFORMAT)d3dFormat;
        if (d3dFormat >= D3DFMT_DXT1 && d3dFormat <= D3DFMT_DXT5)
        {
            rasExt->compressed = TRUE;

            if (d3dFormat == D3DFMT_DXT1)
            {
                rasExt->alpha = FALSE;
                raster->cFormat |= (rwRASTERFORMAT565 >> 8);
            }
            else
            {
                rasExt->alpha = TRUE;
                raster->cFormat |= (rwRASTERFORMAT4444 >> 8);
            }

            raster->depth = 16;
        }
        else
        {
            rasExt->compressed = FALSE;

            if (d3dFormat < MAX_PIXEL_FORMATS)
            {
                rasExt->alpha = _rwD3D9PixelFormatInfo[d3dFormat].alpha;
                raster->depth = _rwD3D9PixelFormatInfo[d3dFormat].depth;
                raster->cFormat |= (_rwD3D9PixelFormatInfo[d3dFormat].rwFormat >> 8);
            }
        }
    }

    return raster;
}


RwBool  Raster::_rwD3D9RasterDestroy(void* unused1 __RWUNUSED__, void* raster, RwInt32 unused3 __RWUNUSED__)
{
    RwRaster* ras = (RwRaster*)raster;
    _rwD3D9RasterExt* rasExt;
    RwUInt32            n;

    rasExt = RASTEREXTFROMRASTER(ras);

    /* Check if it's the current rendering raster */
    for (n = 0; n < RWD3D9_MAX_TEXTURE_STAGES; n++)
    {
        if (_rwD3D9RWGetRasterStage(n) == ras)
            _rwD3D9RWSetRasterStage(NULL, n);
    }

    /*
     * Only free up the surface if this raster owns it (ie, it's not a sub
     * raster and it's not created with dontallocate flag)
     */
    if ((ras->parent == ras))
    {
        switch (ras->cType)
        {
        case (rwRASTERTYPENORMAL):
        {
            if (!(ras->cFlags & rwRASTERDONTALLOCATE))
            {
                SAFE_RELEASE(rasExt->texture);
               
                if (IS_D3DFORMAT_ZBUFFER(rasExt->d3dFormat))
                    rxD3D9VideoMemoryRasterListRemove(ras);
            }
        }
        break;

        case (rwRASTERTYPETEXTURE):
        {
            if (!(ras->cFlags & rwRASTERDONTALLOCATE))
            {
                SAFE_RELEASE(rasExt->texture);

                if (IS_D3DFORMAT_ZBUFFER(rasExt->d3dFormat))
                    rxD3D9VideoMemoryRasterListRemove(ras);

            }
        }
        break;

        case (rwRASTERTYPECAMERATEXTURE):
        {
            if (!(ras->cFlags & rwRASTERDONTALLOCATE))
            {
                SAFE_RELEASE(rasExt->texture);
                rxD3D9VideoMemoryRasterListRemove(ras);

            }
        }
        break;

        case (rwRASTERTYPEZBUFFER):
        {
            if (!(ras->cFlags & rwRASTERDONTALLOCATE))
            {
                if ((LPSURFACE)rasExt->texture != RwD3D9DepthStencilSurface)
                    SAFE_RELEASE(rasExt->texture);

                rxD3D9VideoMemoryRasterListRemove(ras);

            }
        }
        break;

        case (rwRASTERTYPECAMERA):
        {
            rxD3D9VideoMemoryRasterListRemove(ras);
        }
        break;

        default:
            return FALSE;
            break;
        }
    }
    else
    {
        /* Camera Sub rasters need to be removed */
        if (ras->cType == rwRASTERTYPECAMERA)
            rxD3D9VideoMemoryRasterListRemove(ras);
    }

    return TRUE;
}

void* Raster::rwD3D9RasterCtor(void* object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__)
{
    _rwD3D9RasterExt* rasExt = RASTEREXTFROMRASTER(object);

    /* These are used to detect when we are using a camera for the first time */
    rasExt->swapChain = NULL;

    return(object);
}

void* Raster::rwD3D9RasterDtor(void* object, RwInt32 offsetInObject __RWUNUSED__,  RwInt32 sizeInObject __RWUNUSED__)
{
    _rwD3D9RasterExt* rasExt = RASTEREXTFROMRASTER(object);

    if (rasExt->swapChain)
    {
        IDirect3DSwapChain9_Release(rasExt->swapChain);
        rasExt->swapChain = NULL;
    }

    /* Phew! */
    return(object);
}

RwBool Raster::_rwD3D9RasterPluginAttach()
{
    _RwD3D9RasterExtOffset = RwRasterRegisterPlugin(sizeof(_rwD3D9RasterExt), rwID_DEVICEMODULE, rwD3D9RasterCtor, rwD3D9RasterDtor, NULL);

    if (0 > _RwD3D9RasterExtOffset)
        return FALSE;

    /* Fill format info table */
    memset(_rwD3D9PixelFormatInfo, 0, MAX_PIXEL_FORMATS * sizeof(_rwD3D9FormatInfo));

    SETFORMATINFO(D3DFMT_R8G8B8, FALSE, 24, 0);
    SETFORMATINFO(D3DFMT_A8R8G8B8, TRUE, 32, rwRASTERFORMAT8888);
    SETFORMATINFO(D3DFMT_X8R8G8B8, FALSE, 32, rwRASTERFORMAT888);
    SETFORMATINFO(D3DFMT_R5G6B5, FALSE, 16, rwRASTERFORMAT565);
    SETFORMATINFO(D3DFMT_X1R5G5B5, FALSE, 16, rwRASTERFORMAT555);
    SETFORMATINFO(D3DFMT_A1R5G5B5, TRUE, 16, rwRASTERFORMAT1555);
    SETFORMATINFO(D3DFMT_A4R4G4B4, TRUE, 16, rwRASTERFORMAT4444);
    SETFORMATINFO(D3DFMT_R3G3B2, FALSE, 8, 0);
    SETFORMATINFO(D3DFMT_A8, TRUE, 8, 0);
    SETFORMATINFO(D3DFMT_A8R3G3B2, TRUE, 16, 0);
    SETFORMATINFO(D3DFMT_X4R4G4B4, FALSE, 16, 0);
    SETFORMATINFO(D3DFMT_A2B10G10R10, TRUE, 32, 0);
    SETFORMATINFO(D3DFMT_A8B8G8R8, TRUE, 32, 0);
    SETFORMATINFO(D3DFMT_X8B8G8R8, FALSE, 32, 0);
    SETFORMATINFO(D3DFMT_G16R16, FALSE, 32, 0);
    SETFORMATINFO(D3DFMT_A2R10G10B10, TRUE, 32, 0);
    SETFORMATINFO(D3DFMT_A16B16G16R16, TRUE, 64, 0);
    SETFORMATINFO(D3DFMT_A8P8, TRUE, 16, 0);
    SETFORMATINFO(D3DFMT_P8, FALSE, 8, rwRASTERFORMATPAL8);
    SETFORMATINFO(D3DFMT_L8, FALSE, 8, rwRASTERFORMATLUM8);
    SETFORMATINFO(D3DFMT_A8L8, TRUE, 16, 0);
    SETFORMATINFO(D3DFMT_A4L4, TRUE, 8, 0);
    SETFORMATINFO(D3DFMT_V8U8, FALSE, 16, 0);
    SETFORMATINFO(D3DFMT_L6V5U5, FALSE, 16, 0);
    SETFORMATINFO(D3DFMT_X8L8V8U8, FALSE, 32, 0);
    SETFORMATINFO(D3DFMT_Q8W8V8U8, FALSE, 32, 0);
    SETFORMATINFO(D3DFMT_V16U16, FALSE, 32, 0);
    SETFORMATINFO(D3DFMT_A2W10V10U10, TRUE, 32, 0);
    SETFORMATINFO(D3DFMT_D16_LOCKABLE, FALSE, 16, rwRASTERFORMAT16);
    SETFORMATINFO(D3DFMT_D32, FALSE, 32, rwRASTERFORMAT32);
    SETFORMATINFO(D3DFMT_D15S1, FALSE, 16, rwRASTERFORMAT16);
    SETFORMATINFO(D3DFMT_D24S8, FALSE, 32, rwRASTERFORMAT32);
    SETFORMATINFO(D3DFMT_D24X8, FALSE, 32, rwRASTERFORMAT32);
    SETFORMATINFO(D3DFMT_D24X4S4, FALSE, 32, rwRASTERFORMAT32);
    SETFORMATINFO(D3DFMT_D16, FALSE, 16, rwRASTERFORMAT16);
    SETFORMATINFO(D3DFMT_D32F_LOCKABLE, FALSE, 32, rwRASTERFORMAT32);
    SETFORMATINFO(D3DFMT_D24FS8, FALSE, 32, rwRASTERFORMAT32);
    SETFORMATINFO(D3DFMT_L16, FALSE, 16, 0);
    SETFORMATINFO(D3DFMT_Q16W16V16U16, FALSE, 64, 0);
    SETFORMATINFO(D3DFMT_R16F, FALSE, 16, 0);
    SETFORMATINFO(D3DFMT_G16R16F, FALSE, 32, 0);
    SETFORMATINFO(D3DFMT_A16B16G16R16F, TRUE, 64, 0);
    SETFORMATINFO(D3DFMT_R32F, FALSE, 32, 0);
    SETFORMATINFO(D3DFMT_G32R32F, FALSE, 64, 0);
    SETFORMATINFO(D3DFMT_A32B32G32R32F, TRUE, 128, 0);
    SETFORMATINFO(D3DFMT_CxV8U8, FALSE, 16, 0);

    return TRUE;
}

void Raster::_rwD3D9RasterOpen(void)
{
    rxD3D9VideoMemoryRasterListCreate();
}

void Raster::_rwD3D9RasterClose()
{
    rxD3D9VideoMemoryRasterListDestroy();
}

void Raster::rxD3D9VideoMemoryRasterListDestroy()
{
    RasterList.clear();
}

void Raster::_rxD3D9VideoMemoryRasterListRelease()
{
    for (auto raster : RasterList)
    {
        _rwD3D9RasterExt* rasExt = RASTEREXTFROMRASTER(raster);

        if (raster->parent != raster)
            continue;

        switch (raster->cType)
        {
        case rwRASTERTYPENORMAL:
        case rwRASTERTYPETEXTURE:
        {
            if (rasExt->texture)
            {
                if (IS_D3DFORMAT_ZBUFFER(rasExt->d3dFormat))
                {
                    SAFE_RELEASE(rasExt->texture);
                }
            }
        }
        break;

        case rwRASTERTYPECAMERATEXTURE:
        {
            if (rasExt->texture)
            {
                SAFE_RELEASE(rasExt->texture);
            }
        }
        break;

        case rwRASTERTYPEZBUFFER:
        {
            if (rasExt->texture)
            {
                if ((LPSURFACE)rasExt->texture != RwD3D9DepthStencilSurface)
                {
                    SAFE_RELEASE(rasExt->texture);
                }
            }
        }
        break;

        case rwRASTERTYPECAMERA:
        {
            SAFE_RELEASE(rasExt->swapChain);
        }
        break;

        default:
            break;
        }
    }
}

RwBool Raster::_rxD3D9VideoMemoryRasterListRestore()
{
    RwBool                  result;
    HRESULT                 hr;
    result = TRUE;
    hr = D3D_OK;

    for (auto raster : RasterList)
    {
        _rwD3D9RasterExt* rasExt = RASTEREXTFROMRASTER(raster);

        if (raster->parent != raster)
            continue;

        switch (raster->cType)
        {
        case rwRASTERTYPENORMAL:
        case rwRASTERTYPETEXTURE:
        {
            if (IS_D3DFORMAT_ZBUFFER(rasExt->d3dFormat))
            {
                hr = _RwD3DDevice->CreateTexture(raster->width, raster->height, 1,
                    D3DUSAGE_DEPTHSTENCIL, rasExt->d3dFormat, D3DPOOL_DEFAULT, &(rasExt->texture), NULL);
            }
        }
        break;

        case rwRASTERTYPECAMERATEXTURE:
        {
            if (rasExt->texture == nullptr)
            {
                auto rasFormat = RwRasterGetFormat(raster);
                auto levels = ((rasFormat & rwRASTERFORMATMIPMAP) ? 0 : 1);
                auto mipmap = (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0);

                if (rasExt->cube)
                {
                    hr = _RwD3DDevice->CreateCubeTexture(raster->width, levels,
                        mipmap | D3DUSAGE_RENDERTARGET, rasExt->d3dFormat, D3DPOOL_DEFAULT, (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture), NULL);
                }
                else
                {
                    hr = _RwD3DDevice->CreateTexture(raster->width, raster->height, levels,
                        mipmap | D3DUSAGE_RENDERTARGET, rasExt->d3dFormat, D3DPOOL_DEFAULT, &(rasExt->texture), NULL);
                }
            }
        }
        break;

        case rwRASTERTYPEZBUFFER:
        {
            if (rasExt->texture == nullptr)
            {
                if (_rwD3D9CheckValidZBufferFormat(D3DFMT_D24S8))
                    rasExt->d3dFormat = D3DFMT_D24S8;
                else if (_rwD3D9CheckValidZBufferFormat(D3DFMT_D24X4S4))
                    rasExt->d3dFormat = D3DFMT_D24X4S4;
                else if (_rwD3D9CheckValidZBufferFormat(D3DFMT_D32))
                    rasExt->d3dFormat = D3DFMT_D32;
                else
                    rasExt->d3dFormat = D3DFMT_D24X8;

                hr = _RwD3DDevice->CreateDepthStencilSurface( max(1, raster->width), max(1, raster->height),
                    rasExt->d3dFormat, Present.MultiSampleType, Present.MultiSampleQuality, FALSE, (LPSURFACE*)&(rasExt->texture), NULL);
            }
            else
            {
                RECT rect;
                GetClientRect(*WindowHandle, &rect);

                raster->width = rect.right;
                raster->height = rect.bottom;

                rasExt->texture = (LPDIRECT3DTEXTURE9)RwD3D9DepthStencilSurface;
                rasExt->d3dFormat = Present.AutoDepthStencilFormat;
            }
        }
        break;

        case rwRASTERTYPECAMERA:
        {
            if (rasExt->swapChain)
            {
                D3DPRESENT_PARAMETERS   present;
                RECT rect;

                present = Present;
                present.hDeviceWindow = (HWND)rasExt->window;

                GetClientRect(present.hDeviceWindow, &rect);

                if (rect.right)
                    present.BackBufferWidth = 0;
                else
                    present.BackBufferWidth = 1;

                if (rect.bottom)
                    present.BackBufferHeight = 0;
                else
                    present.BackBufferHeight = 1;

                present.EnableAutoDepthStencil = FALSE;

                rasExt->swapChain = NULL;

                if (D3D_OK == _RwD3DDevice->CreateAdditionalSwapChain(&present, &(rasExt->swapChain)))
                {
                    IDirect3DSurface9** ppBackBuffer = (IDirect3DSurface9**)&(rasExt->texture);

                    hr = IDirect3DSwapChain9_GetBackBuffer(rasExt->swapChain, 0, D3DBACKBUFFER_TYPE_MONO, ppBackBuffer);
                    IDirect3DSurface9_Release((LPSURFACE)rasExt->texture);

                    rasExt->d3dFormat = present.BackBufferFormat;
                }
            }
            else
            {
                if (_RwD3D9AdapterInformation.mode.Width && (RwUInt32)raster->width != Present.BackBufferWidth)
                    raster->width = (raster->width * Present.BackBufferWidth) / _RwD3D9AdapterInformation.mode.Width;

                if (_RwD3D9AdapterInformation.mode.Height && (RwUInt32)raster->height != Present.BackBufferHeight)
                    raster->height = (raster->height * Present.BackBufferHeight) / _RwD3D9AdapterInformation.mode.Height;

                raster->depth = _rwD3D9PixelFormatInfo[Present.BackBufferFormat].depth;
                raster->cFormat = (RwUInt8)(_rwD3D9PixelFormatInfo[Present.BackBufferFormat].rwFormat >> 8);
                rasExt->d3dFormat = Present.BackBufferFormat;
                rasExt->alpha = _rwD3D9PixelFormatInfo[Present.BackBufferFormat].alpha;
            }
        }
        break;

        default:
            break;
        }
    }

    if (FAILED(hr))
        result = FALSE;

    return(result);
}

void Raster::rxD3D9VideoMemoryRasterListRemove(RwRaster* raster)
{
    RasterList.remove(raster);
}

void Raster::rxD3D9VideoMemoryRasterListAdd(RwRaster* raster)
{
   RasterList.push_back(raster);
}

RwBool Raster::rxD3D9VideoMemoryRasterListCreate()
{
    return true;
}

RwRaster* Raster::RwD3D9RasterStreamReadDDS(RwStream* stream)
{
    HRESULT         hr;
    RwRaster* raster;
    _rwD3D9RasterExt* rasExt;
    RwUInt32        magic;
    DDS_HEADER      header;
    DDS_HEADER      headerOrignal;
    RwInt32         mipMapped;
    RwInt32         numMipLevels;
    RwUInt32        size;
    RwInt32         i;
    RwBool          cubeTexture;
    RwBool          convert;

    raster = NULL;
    cubeTexture = FALSE;
    convert = FALSE;

    size = sizeof(RwUInt32);
    if (RwStreamRead(stream, (void*)&magic, size) != size)
       return nullptr;

    /* Check it's a "DDS " file */
    if (0x20534444 != magic)
       return nullptr;

    /* Read the header */
    size = sizeof(DDS_HEADER);
    if (RwStreamRead(stream, (void*)&header, size) != size)
       return nullptr;

    /* Is it a texture ? */
    if (!(header.dwHeaderFlags & DDS_HEADER_FLAGS_TEXTURE))
       return nullptr;

    /* Is this texture mipmapped? */
    mipMapped = header.dwHeaderFlags & DDS_HEADER_FLAGS_MIPMAP ? rwRASTERFORMATMIPMAP : 0;

    /* Is this a cube texture */
    cubeTexture = ((header.dwSurfaceFlags & DDS_SURFACE_FLAGS_CUBEMAP) != 0 && (header.dwCubemapFlags & DDS_CUBEMAP_ALLFACES) != 0);

    headerOrignal = header;
    if (mipMapped)
    {
        header.dwMipMapCount = ((RwInt32)header.dwMipMapCount > 0 ? header.dwMipMapCount : 1);
        header.dwWidth = header.dwWidth >> (headerOrignal.dwMipMapCount - header.dwMipMapCount);
        header.dwHeight = header.dwHeight >> (headerOrignal.dwMipMapCount - header.dwMipMapCount);
    }

    /* Handle DXT1-5 format DDS files */
    if (DDS_FOURCC == header.ddspf.dwFlags)
    {
        if (cubeTexture)
        {
            convert = (_rwD3D9CheckValidCubeTextureFormat(header.ddspf.dwFourCC) == FALSE);

            if (convert)
               return nullptr;
        }
        else
            convert = (_rwD3D9CheckValidTextureFormat(header.ddspf.dwFourCC) == FALSE);

        if (!convert)
        {
            RwUInt32 levels;

            /* Create a raster */
            raster = RwRasterCreate(header.dwWidth, header.dwHeight, 0,
                rwRASTERTYPETEXTURE | rwRASTERDONTALLOCATE | mipMapped);

            if (raster == nullptr)
               return nullptr;

            /* Remove the rwRASTERDONTALLOCATE flag so it will get destroyed */
            raster->cFlags &= ~rwRASTERDONTALLOCATE;

            rasExt = RASTEREXTFROMRASTER(raster);

            /* Find the surface format */
            rasExt->d3dFormat = (D3DFORMAT)header.ddspf.dwFourCC;

            /* is it compressed? */
            if (rasExt->d3dFormat >= D3DFMT_DXT1 &&
                rasExt->d3dFormat <= D3DFMT_DXT5)
                rasExt->compressed = TRUE;
            else
                rasExt->compressed = FALSE;

            /* Create the texture, if mipped 0 gets all the mip levels */
            if (cubeTexture)
            {
                if (_RwD3D9DeviceCaps.CubeTextureFilterCaps & (D3DPTFILTERCAPS_MIPFPOINT | D3DPTFILTERCAPS_MIPFLINEAR))
                    levels = ((header.dwHeaderFlags & DDS_HEADER_FLAGS_MIPMAP) ? header.dwMipMapCount : 1);
                else
                    levels = 1;

                rasExt->cube = TRUE;

                hr = _RwD3DDevice->CreateCubeTexture(raster->width, levels,
                    0,(D3DFORMAT)rasExt->d3dFormat, D3DPOOL_MANAGED, (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture), NULL);
            }
            else
            {
                levels = ((header.dwHeaderFlags & DDS_HEADER_FLAGS_MIPMAP) ? header.dwMipMapCount : 1);

                hr = _RwD3DDevice->CreateTexture(raster->width,  raster->height, levels,
                    0, (D3DFORMAT)rasExt->d3dFormat, D3DPOOL_MANAGED, &(rasExt->texture),NULL);
            }

            if (FAILED(hr))
               return nullptr;
        }
    }
    else if (DDS_RGB == header.ddspf.dwFlags)
    {
        D3DFORMAT       d3dFormat = D3DFMT_UNKNOWN;

        if (cubeTexture)
        {
            RwRasterFormat  rasFormat = rwRASTERFORMATDEFAULT;

            if (header.ddspf.dwRGBBitCount == 24)
            {
                d3dFormat = D3DFMT_R8G8B8;

                if (_rwD3D9CheckValidCubeTextureFormat(D3DFMT_R8G8B8))
                    rasFormat = rwRASTERFORMAT888;
                else
                   return nullptr;
            }
            else if (0x00ff0000 == header.ddspf.dwRBitMask)
            {
                d3dFormat = D3DFMT_X8R8G8B8;

                if (_rwD3D9CheckValidCubeTextureFormat(D3DFMT_X8R8G8B8))
                    rasFormat = rwRASTERFORMAT888;
                else
                   return nullptr;
            }
            else if (0x0000f800 == header.ddspf.dwRBitMask)
            {
                d3dFormat = D3DFMT_R5G6B5;

                if (_rwD3D9CheckValidCubeTextureFormat(D3DFMT_R5G6B5))
                    rasFormat = rwRASTERFORMAT565;
                else
                   return nullptr;
            }
            else
               return nullptr;

            if (!convert)
            {
                RwUInt32 width, height;

                /* Check size */
                width = header.dwWidth;
                height = header.dwHeight;

                _rwD3D9CheckRasterSize((RwInt32*)&width, (RwInt32*)&height, rwRASTERTYPETEXTURE | rasFormat | mipMapped);

                if (header.dwWidth != width ||
                    header.dwHeight != height)
                   return nullptr;
                else
                {
                    raster = RwRasterCreate(header.dwWidth, header.dwHeight, 0,  rwRASTERTYPETEXTURE | rwRASTERDONTALLOCATE |
                        rasFormat | mipMapped);

                    if (raster == nullptr)
                       return nullptr;
                    else
                    {
                        RwUInt32 levels;

                        rasExt = RASTEREXTFROMRASTER(raster);

                        rasExt->cube = TRUE;

                        if (_RwD3D9DeviceCaps.CubeTextureFilterCaps & (D3DPTFILTERCAPS_MIPFPOINT | D3DPTFILTERCAPS_MIPFLINEAR))
                            levels = ((header.dwHeaderFlags & DDS_HEADER_FLAGS_MIPMAP) ? header.dwMipMapCount : 1);
                        else
                            levels = 1;

                        hr =
                            _RwD3DDevice->CreateCubeTexture(raster->width, levels,
                                0,(D3DFORMAT)d3dFormat, D3DPOOL_MANAGED, (LPDIRECT3DCUBETEXTURE9*)
                                &(rasExt->texture), NULL);

                        if (FAILED(hr))
                           return nullptr;

                        /* Remove the rwRASTERDONTALLOCATE flag so it will get destroyed */
                        raster->cFlags &= ~rwRASTERDONTALLOCATE;
                        rasExt->d3dFormat = d3dFormat;

                    }
                }
            }
        }
        else
        {
            if (header.ddspf.dwRGBBitCount == 24)
            {
                if (_rwD3D9CheckValidCubeTextureFormat(D3DFMT_R8G8B8))
                    d3dFormat = D3DFMT_R8G8B8;
                else
                    convert = TRUE;
            }
            else if (0x00ff0000 == header.ddspf.dwRBitMask)
            {
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_X8R8G8B8))
                    d3dFormat = D3DFMT_X8R8G8B8;
                else
                    convert = TRUE;
            }
            else if (0x0000f800 == header.ddspf.dwRBitMask)
                d3dFormat = D3DFMT_R5G6B5;
            else if (0x000000e0 == header.ddspf.dwRBitMask)
            {
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_R3G3B2))
                    d3dFormat = D3DFMT_R3G3B2;
                else
                    convert = TRUE;
            }
            else
               return nullptr;

            if (!convert)
            {
                RwUInt32 width, height;

                /* Check size */
                width = header.dwWidth;
                height = header.dwHeight;

                _rwD3D9CheckRasterSize((RwInt32*)&width, (RwInt32*)&height, rwRASTERTYPETEXTURE | mipMapped);

                if (header.dwWidth != width ||
                    header.dwHeight != height)
                    convert = TRUE;
                else
                {
                    raster = RwD3D9RasterCreate(header.dwWidth, header.dwHeight, d3dFormat, rwRASTERTYPETEXTURE | mipMapped);

                    if (raster == nullptr)
                       return nullptr;
                }
            }
        }
    }
    else if (DDS_RGBA == header.ddspf.dwFlags)
    {
        D3DFORMAT d3dFormat = D3DFMT_UNKNOWN;

        if (cubeTexture)
        {
            RwRasterFormat    rasFormat = rwRASTERFORMATDEFAULT;

            if (0x00ff0000 == header.ddspf.dwRBitMask)
            {
                d3dFormat = D3DFMT_A8R8G8B8;

                if (_rwD3D9CheckValidCubeTextureFormat(D3DFMT_A8R8G8B8))
                    rasFormat = rwRASTERFORMAT8888;
                else
                   return nullptr;
            }
            else if (0x00007c00 == header.ddspf.dwRBitMask)
            {
                d3dFormat = D3DFMT_A1R5G5B5;

                if (_rwD3D9CheckValidCubeTextureFormat(D3DFMT_A1R5G5B5))
                    rasFormat = rwRASTERFORMAT1555;
                else
                   return nullptr;
            }
            else if (0x00000f00 == header.ddspf.dwRBitMask)
            {
                d3dFormat = D3DFMT_A4R4G4B4;

                if (_rwD3D9CheckValidCubeTextureFormat(D3DFMT_A4R4G4B4))
                    rasFormat = rwRASTERFORMAT4444;
                else
                   return nullptr;
            }
            else
               return nullptr;

            if (!convert)
            {
                RwUInt32 width, height;

                /* Check size */
                width = header.dwWidth;
                height = header.dwHeight;

                _rwD3D9CheckRasterSize((RwInt32*)&width, (RwInt32*)&height, rwRASTERTYPETEXTURE | rasFormat | mipMapped);

                if (header.dwWidth != width ||
                    header.dwHeight != height)
                   return nullptr;
                else
                {
                    raster = RwRasterCreate(header.dwWidth, header.dwHeight, 0, rwRASTERTYPETEXTURE | rwRASTERDONTALLOCATE | rasFormat | mipMapped);

                    if (raster == nullptr)
                       return nullptr;
                    else
                    {
                        RwUInt32 levels;

                        rasExt = RASTEREXTFROMRASTER(raster);
                        rasExt->cube = TRUE;

                        if (_RwD3D9DeviceCaps.CubeTextureFilterCaps & (D3DPTFILTERCAPS_MIPFPOINT | D3DPTFILTERCAPS_MIPFLINEAR))
                            levels = ((header.dwHeaderFlags & DDS_HEADER_FLAGS_MIPMAP) ? header.dwMipMapCount : 1);
                        else
                            levels = 1;

                        hr =
                            _RwD3DDevice->CreateCubeTexture(raster->width,levels,
                                0, (D3DFORMAT)d3dFormat, D3DPOOL_MANAGED,(LPDIRECT3DCUBETEXTURE9*)  &(rasExt->texture),  NULL);

                        if (FAILED(hr))
                           return nullptr;

                        /* Remove the rwRASTERDONTALLOCATE flag so it will get destroyed */
                        raster->cFlags &= ~rwRASTERDONTALLOCATE;
                        rasExt->d3dFormat = d3dFormat;

                    }
                }
            }
        }
        else
        {
            if (0x00ff0000 == header.ddspf.dwRBitMask)
            {
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_A8R8G8B8))
                    d3dFormat = D3DFMT_A8R8G8B8;
                else
                    convert = TRUE;
            }
            else if (0x00007c00 == header.ddspf.dwRBitMask)
                d3dFormat = D3DFMT_A1R5G5B5;
            else if (0x00000f00 == header.ddspf.dwRBitMask)
            {
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_A4R4G4B4))
                    d3dFormat = D3DFMT_A4R4G4B4;
                else
                    convert = TRUE;
            }
            else if (0x000003ff == header.ddspf.dwRBitMask)
            {
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_A2R10G10B10))
                    d3dFormat = D3DFMT_A2R10G10B10;
                else
                    convert = TRUE;
            }
            else if (0x000003ff == header.ddspf.dwBBitMask)
            {
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_A2B10G10R10))
                    d3dFormat = D3DFMT_A2B10G10R10;
                else
                    convert = TRUE;
            }
            else if (0x000000e0 == header.ddspf.dwRBitMask)
            {
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_A8R3G3B2))
                    d3dFormat = D3DFMT_A8R3G3B2;
                else
                    convert = TRUE;
            }
            else
            {
               return nullptr;
            }

            if (!convert)
            {
                RwUInt32 width, height;

                /* Check size */
                width = header.dwWidth;
                height = header.dwHeight;

                _rwD3D9CheckRasterSize((RwInt32*)&width, (RwInt32*)&height, rwRASTERTYPETEXTURE | mipMapped);

                if (header.dwWidth != width ||
                    header.dwHeight != height)
                    convert = TRUE;
                else
                {
                    raster = RwD3D9RasterCreate(header.dwWidth, header.dwHeight, d3dFormat,rwRASTERTYPETEXTURE | mipMapped);

                    if (raster == nullptr)
                       return nullptr;
                }
            }
        }
    }
    else
    {

    /* Luminance and palette formats */
    D3DFORMAT       d3dFormat = D3DFMT_UNKNOWN;

    if (cubeTexture)
       return nullptr;
    else
    {
        if (header.ddspf.dwRGBBitCount == 16)
        {
            if (0x0000ffff == header.ddspf.dwRBitMask)
            {
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_L16))
                    d3dFormat = D3DFMT_L16;
                else
                    convert = TRUE;
            }
            else if (0x000000ff == header.ddspf.dwRBitMask &&
                0x0000ff00 == header.ddspf.dwABitMask)
            {
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_A8L8))
                    d3dFormat = D3DFMT_A8L8;
                else
                    convert = TRUE;
            }
            else
               return nullptr;
        }
        else if (header.ddspf.dwRGBBitCount == 8)
        {
            if (0x000000ff == header.ddspf.dwRBitMask)
            {
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_L8))
                    d3dFormat = D3DFMT_L8;
                else
                    convert = TRUE;
            }
            else if (0x000000ff == header.ddspf.dwABitMask)
            {
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_A8))
                    d3dFormat = D3DFMT_A8;
                else
                    convert = TRUE;
            }
            else if (0x0000000f == header.ddspf.dwRBitMask &&
                0x000000f0 == header.ddspf.dwABitMask)
            {
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_A4L4))
                    d3dFormat = D3DFMT_A4L4;
                else
                    convert = TRUE;
            }
            else if (0 == header.ddspf.dwRBitMask &&
                0 == header.ddspf.dwGBitMask &&
                0 == header.ddspf.dwBBitMask &&
                0 == header.ddspf.dwABitMask)
            {
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_P8))
                    d3dFormat = D3DFMT_P8;
                else
                    convert = TRUE;
            }
            else
                return nullptr;
        }
        else
            return nullptr;


            if (!convert)
            {
                RwUInt32 width, height;

                /* Check size */
                width = header.dwWidth;
                height = header.dwHeight;

                _rwD3D9CheckRasterSize((RwInt32*)&width, (RwInt32*)&height, rwRASTERTYPETEXTURE | mipMapped);

                if (header.dwWidth != width ||
                    header.dwHeight != height)
                    convert = TRUE;
                else
                {
                    raster = RwD3D9RasterCreate(header.dwWidth,
                        header.dwHeight,
                        d3dFormat,
                        rwRASTERTYPETEXTURE | mipMapped);

                    if (raster == nullptr)
                       return nullptr;
                    else
                    {
                        if (d3dFormat == D3DFMT_P8)
                        {
                            PALETTEENTRY* palette;

                            palette = (PALETTEENTRY*)RwRasterLockPalette(raster, rwRASTERLOCKWRITE);
                            if (palette != NULL)
                            {
                                if (RwStreamRead(stream, (void*)palette, 256 * sizeof(PALETTEENTRY)) != 256 * sizeof(PALETTEENTRY))
                                {
                                    RwRasterUnlockPalette(raster);
                                    RwRasterDestroy(raster);

                                   return nullptr;
                                }

                                RwRasterUnlockPalette(raster);
                            }
                        }
                    }
                }
            }
        }
    }

    if (!convert)
    {
        RwInt32 rasterNumMipLevels;

        rasExt = RASTEREXTFROMRASTER(raster);

        /* Load the surfaces */
        rasterNumMipLevels = RwRasterGetNumLevels(raster);

        if (mipMapped)
            numMipLevels = header.dwMipMapCount;
        else
            numMipLevels = 1;

        do
        {
            if (cubeTexture)
            {
                if (header.dwCubemapFlags & DDS_CUBEMAP_POSITIVEX)
                {
                    rasExt->face = D3DCUBEMAP_FACE_POSITIVE_X;
                    header.dwCubemapFlags &= ~DDS_CUBEMAP_POSITIVEX;
                }
                else if (header.dwCubemapFlags & DDS_CUBEMAP_NEGATIVEX)
                {
                    rasExt->face = D3DCUBEMAP_FACE_NEGATIVE_X;
                    header.dwCubemapFlags &= ~DDS_CUBEMAP_NEGATIVEX;
                }
                else if (header.dwCubemapFlags & DDS_CUBEMAP_POSITIVEY)
                {
                    rasExt->face = D3DCUBEMAP_FACE_POSITIVE_Y;
                    header.dwCubemapFlags &= ~DDS_CUBEMAP_POSITIVEY;
                }
                else if (header.dwCubemapFlags & DDS_CUBEMAP_NEGATIVEY)
                {
                    rasExt->face = D3DCUBEMAP_FACE_NEGATIVE_Y;
                    header.dwCubemapFlags &= ~DDS_CUBEMAP_NEGATIVEY;
                }
                else if (header.dwCubemapFlags & DDS_CUBEMAP_POSITIVEZ)
                {
                    rasExt->face = D3DCUBEMAP_FACE_POSITIVE_Z;
                    header.dwCubemapFlags &= ~DDS_CUBEMAP_POSITIVEZ;
                }
                else if (header.dwCubemapFlags & DDS_CUBEMAP_NEGATIVEZ)
                {
                    rasExt->face = D3DCUBEMAP_FACE_NEGATIVE_Z;
                    header.dwCubemapFlags = 0;
                }
                else
                    break;
            }

            if (mipMapped)
            {
                _rwD3D9RasterExt* rasExt = RASTEREXTFROMRASTER(raster);
                for (i = (RwInt32)header.dwMipMapCount; i < (RwInt32)headerOrignal.dwMipMapCount; i++)
                {
                    size = D3D9DDSGetSize(headerOrignal.dwWidth, headerOrignal.dwHeight, raster->depth, rasExt->compressed, rasExt->d3dFormat, i - (RwInt32)header.dwMipMapCount);

                    RwStreamSkip(stream, size);
                }
            }

            for (i = 0; i < numMipLevels; i++)
            {

                if (i >= rasterNumMipLevels)
                {
                    /* Skip mip level */
                    size = D3D9RasterGetSize(raster, i);
                    RwStreamSkip(stream, size);
                }
                else
                {
                    D3DSURFACE_DESC surfaceDesc;
                    RwUInt8* pixels;

                    pixels = RwRasterLock(raster, (RwUInt8)i, rwRASTERLOCKWRITE);

                    /* Get the surface descriptor to find the size */
                    IDirect3DTexture9_GetLevelDesc(rasExt->texture, i, &surfaceDesc);

                    /* Load the surface */
                    if (DDS_FOURCC == header.ddspf.dwFlags)
                    {
                        if (D3DFMT_DXT1 == header.ddspf.dwFourCC ||
                            D3DFMT_DXT2 == header.ddspf.dwFourCC ||
                            D3DFMT_DXT3 == header.ddspf.dwFourCC ||
                            D3DFMT_DXT4 == header.ddspf.dwFourCC ||
                            D3DFMT_DXT5 == header.ddspf.dwFourCC)
                        {
                            if (surfaceDesc.Height < 4)
                                size = (rasExt->lockedRect.Pitch);
                            else
                                size = (surfaceDesc.Height / 4) * (rasExt->lockedRect.Pitch);
                        }
                        else
                            size = surfaceDesc.Height * (rasExt->lockedRect.Pitch);

                        if (RwStreamRead(stream, (void*)pixels, size) != size)
                           return nullptr;
                    }
                    else
                    {
                        // new codes for the right below
                        if (surfaceDesc.Height < 4)
                            size = (rasExt->lockedRect.Pitch);
                        else
                            size = surfaceDesc.Height * (rasExt->lockedRect.Pitch);

                        // original codes : Something strange has happened here when the last mipmap size has been 1.
                        //size = surfaceDesc.Height * (rasExt->lockedRect.Pitch);

                        if (RwStreamRead(stream, (void*)pixels, size) != size)
                           return nullptr;
                    }

                    RwRasterUnlock(raster);
                }
            }
        } while (cubeTexture && header.dwCubemapFlags);

        /* Does this raster format have an alpha component */
        if (DDS_FOURCC == header.ddspf.dwFlags)
        {
            RwRasterFormat pixelFormat;

            pixelFormat = (RwRasterFormat)(RwRasterGetFormat(raster) & rwRASTERFORMATPIXELFORMATMASK);

            if (D3DFMT_DXT1 == rasExt->d3dFormat)
            {
                rasExt->alpha = D3D9RasterDXT1HasAlpha(raster);

                if (rasExt->alpha == FALSE)
                {
                    raster->cFormat &= ~(rwRASTERFORMATPIXELFORMATMASK >> 8);
                    raster->cFormat |= (rwRASTERFORMAT565 >> 8);
                }
                else
                {
                    raster->cFormat &= ~(rwRASTERFORMATPIXELFORMATMASK >> 8);
                    raster->cFormat |= (rwRASTERFORMAT1555 >> 8);
                    raster->depth = 16;
                }
            }
            else if (D3DFMT_DXT2 == rasExt->d3dFormat ||
                D3DFMT_DXT3 == rasExt->d3dFormat ||
                D3DFMT_DXT4 == rasExt->d3dFormat ||
                D3DFMT_DXT5 == rasExt->d3dFormat)

            {
                rasExt->alpha = TRUE;
                raster->cFormat &= ~(rwRASTERFORMATPIXELFORMATMASK >> 8);
                raster->cFormat |= (rwRASTERFORMAT4444 >> 8);
                raster->depth = 16;
            }
            else
            {
                rasExt->alpha = _rwD3D9PixelFormatInfo[rasExt->d3dFormat].alpha;
                raster->depth = _rwD3D9PixelFormatInfo[rasExt->d3dFormat].depth;
                raster->cFormat &= ~(rwRASTERFORMATPIXELFORMATMASK >> 8);
                raster->cFormat |= (_rwD3D9PixelFormatInfo[rasExt->d3dFormat].rwFormat >> 8);
            }
        }
        else if ((header.ddspf.dwFlags & DDS_ALPHAPIXELS) != 0 ||
            header.ddspf.dwABitMask != 0)
            rasExt->alpha = TRUE;
        else
            rasExt->alpha = FALSE;
    }
    else
    {
        RwUInt8* pixels = NULL;
        RwRGBA* palette = NULL;
        RwImage* image;
        RwRasterFormat  rasFormat = rwRASTERFORMATDEFAULT;
        RwUInt32        pitch;

        /*
         * Read to a 32 bits image and later create a raster from it
         */

         /* Load the first mipmap level */
        if (DDS_FOURCC == header.ddspf.dwFlags)
        {
            if (header.dwPitchOrLinearSize == 0)
            {
                if (header.ddspf.dwFourCC < D3DFMT_DXT1 ||
                    header.ddspf.dwFourCC > D3DFMT_DXT5)
                {
                    pitch = header.dwWidth * (_rwD3D9PixelFormatInfo[header.ddspf.dwFourCC].depth >> 3);

                    size = header.dwHeight * pitch;
                }
                else
                {
                    RwUInt32 shift, blocksX, blocksY, totalBlocks;

                    if (D3DFMT_DXT1 == header.ddspf.dwFourCC)
                        shift = 3; /* 64Bits / 8 = 8. 2^3 = 8 */
                    else if (D3DFMT_DXT2 == header.ddspf.dwFourCC ||
                        D3DFMT_DXT3 == header.ddspf.dwFourCC ||
                        D3DFMT_DXT4 == header.ddspf.dwFourCC ||
                        D3DFMT_DXT5 == header.ddspf.dwFourCC)
                        shift = 4; /* 128Bits / 8 = 16. 2^4 = 16 */

                    blocksX = header.dwWidth >> 2;
                    if (blocksX == 0)
                        blocksX = 1;

                    blocksY = header.dwHeight >> 2;
                    if (blocksY == 0)
                        blocksY = 1;

                    totalBlocks = blocksX * blocksY;

                    size = totalBlocks << shift;
                }
            }
            else
            {
                size = header.dwPitchOrLinearSize;
            }
        }
        else
        {
            if (header.dwHeaderFlags & DDS_HEADER_FLAGS_LINEARSIZE)
            {
                size = header.dwPitchOrLinearSize;

                if (header.ddspf.dwRGBBitCount == 8)
                    pitch = header.dwWidth;
                else if (header.ddspf.dwRGBBitCount == 16)
                    pitch = header.dwWidth * 2;
                else if (header.ddspf.dwRGBBitCount == 24)
                    pitch = header.dwWidth * 3;
                else if (header.ddspf.dwRGBBitCount == 32)
                    pitch = header.dwWidth * 4;
                else if (header.ddspf.dwRGBBitCount == 64)
                    pitch = header.dwWidth * 8;
                else if (0x00ff0000 == header.ddspf.dwRBitMask)
                    pitch = header.dwWidth * 4;
                else
                    pitch = header.dwWidth * 2;
            }
            else
            {
                if (header.dwPitchOrLinearSize == 0)
                {
                    if (header.ddspf.dwRGBBitCount == 8)
                        header.dwPitchOrLinearSize = header.dwWidth;
                    else if (header.ddspf.dwRGBBitCount == 16)
                        header.dwPitchOrLinearSize = header.dwWidth * 2;
                    else if (header.ddspf.dwRGBBitCount == 24)
                        header.dwPitchOrLinearSize = header.dwWidth * 3;
                    else if (header.ddspf.dwRGBBitCount == 32)
                        header.dwPitchOrLinearSize = header.dwWidth * 4;
                    else if (header.ddspf.dwRGBBitCount == 64)
                        header.dwPitchOrLinearSize = header.dwWidth * 8;
                    else if (0x00ff0000 == header.ddspf.dwRBitMask)
                        header.dwPitchOrLinearSize = header.dwWidth * 4;
                    else
                        header.dwPitchOrLinearSize = header.dwWidth * 2;
                }

                size = header.dwPitchOrLinearSize * header.dwHeight;
                pitch = header.dwPitchOrLinearSize;
            }
        }

        /* Palette? */
        if (header.ddspf.dwRGBBitCount == 8 &&
            0 == header.ddspf.dwRBitMask &&
            0 == header.ddspf.dwGBitMask &&
            0 == header.ddspf.dwBBitMask &&
            0 == header.ddspf.dwABitMask)
        {
            /* Read palette */
            palette = (RwRGBA*)RwMalloc(256 * sizeof(RwRGBA), rwID_IMAGE | rwMEMHINTDUR_FUNCTION);

            if (RwStreamRead(stream, (void*)palette, 256 * sizeof(RwRGBA)) != 256 * sizeof(RwRGBA))
            {
                RwFree(palette);

               return nullptr;
            }
        }

        /* Read pixel data */
        pixels = (RwUInt8*)RwMalloc(size, rwID_IMAGE | rwMEMHINTDUR_FUNCTION);

        if (RwStreamRead(stream, (void*)pixels, size) != size)
        {
            RwFree(pixels);

           return nullptr;
        }

        /* Create an image */
        image = RwImageCreate(header.dwWidth, header.dwHeight, 32);

        if (NULL == image)
        {
            RwFree(pixels);

           return nullptr;
        }

        RwImageAllocatePixels(image);

        /* Convert the dds to 32 bits */
        if (DDS_FOURCC == header.ddspf.dwFlags)
        {
            switch (header.ddspf.dwFourCC)
            {
            case D3DFMT_A16B16G16R16:
            {
                const RwUInt8* srcPixel;
                RwRGBA* dstPixel;
                RwUInt32        j, i;

                for (j = 0; j < header.dwHeight; j++)
                {
                    srcPixel = (const RwUInt8*)(pixels + (pitch * j));
                    dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                    for (i = 0; i < header.dwWidth; i++)
                    {
                        dstPixel->red = srcPixel[0 * 2 + 1];
                        dstPixel->green = srcPixel[1 * 2 + 1];
                        dstPixel->blue = srcPixel[2 * 2 + 1];
                        dstPixel->alpha = srcPixel[3 * 2 + 1];

                        srcPixel += 8;
                        dstPixel++;
                    }
                }
            }
            break;

            case D3DFMT_DXT1:
                if (_rwD3D9DecompressDXT1(header.dwWidth, header.dwHeight, (RwUInt32*)image->cpPixels, (const RwUInt16*)pixels))
                    rasFormat = rwRASTERFORMAT1555;
                else
                    rasFormat = rwRASTERFORMAT565;
                break;

            case D3DFMT_DXT2:
            case D3DFMT_DXT3:
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_A4R4G4B4))
                    rasFormat = rwRASTERFORMAT4444;
                else
                    rasFormat = rwRASTERFORMAT1555;

                _rwD3D9DecompressDXT3(header.dwWidth, header.dwHeight, (RwUInt32*)image->cpPixels, (const RwUInt16*)pixels);
                break;

            case D3DFMT_DXT4:
            case D3DFMT_DXT5:
                if (_rwD3D9CheckValidTextureFormat(D3DFMT_A4R4G4B4))
                    rasFormat = rwRASTERFORMAT4444;
                else
                    rasFormat = rwRASTERFORMAT1555;

                _rwD3D9DecompressDXT5(header.dwWidth, header.dwHeight, (RwUInt32*)image->cpPixels, (const RwUInt16*)pixels);
                break;
            }
        }
        else if (DDS_RGB == header.ddspf.dwFlags)
        {
            RwRGBA* dstPixel;
            RwUInt32        j, i;

            if (header.ddspf.dwRGBBitCount == 24)
            {
                const RwUInt8* srcPixel;

                for (j = 0; j < header.dwHeight; j++)
                {
                    srcPixel = (const RwUInt8*)(pixels + (pitch * j));
                    dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                    for (i = 0; i < header.dwWidth; i++)
                    {
                        dstPixel->red = srcPixel[2];
                        dstPixel->green = srcPixel[1];
                        dstPixel->blue = srcPixel[0];
                        dstPixel->alpha = 0xFF;

                        srcPixel += 3;
                        dstPixel++;
                    }
                }
            }
            else if (0x00ff0000 == header.ddspf.dwRBitMask)
            {
                const RwUInt32* srcPixel;

                for (j = 0; j < header.dwHeight; j++)
                {
                    srcPixel = (const RwUInt32*)(pixels + (pitch * j));
                    dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                    for (i = 0; i < header.dwWidth; i++)
                    {
                        const RwUInt32 pixIn = *srcPixel;

                        srcPixel++;

                        dstPixel->alpha = 0xFF;
                        dstPixel->red = (RwUInt8)((pixIn >> 16) & 0xFF);
                        dstPixel->green = (RwUInt8)((pixIn >> 8) & 0xFF);
                        dstPixel->blue = (RwUInt8)((pixIn) & 0xFF);

                        dstPixel++;
                    }
                }
            }
            else if (0x0000f800 == header.ddspf.dwRBitMask)
            {
                const RwUInt16* srcPixel;

                rasFormat = rwRASTERFORMAT565;

                for (j = 0; j < header.dwHeight; j++)
                {
                    srcPixel = (const RwUInt16*)(pixels + (pitch * j));
                    dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                    for (i = 0; i < header.dwWidth; i++)
                    {
                        const RwUInt16 pixIn = *srcPixel;

                        dstPixel->alpha = 0xFF;
                        dstPixel->red = (RwUInt8)((pixIn >> 8) & 0xF8);
                        dstPixel->green = (RwUInt8)((pixIn >> 3) & 0xFC);
                        dstPixel->blue = (RwUInt8)((pixIn << 3) & 0xF8);

                        srcPixel++;
                        dstPixel++;
                    }
                }
            }
            else if (0x000000e0 == header.ddspf.dwRBitMask)
            {
                const RwUInt8* srcPixel;

                rasFormat = rwRASTERFORMAT565;

                for (j = 0; j < header.dwHeight; j++)
                {
                    srcPixel = (const RwUInt8*)(pixels + (pitch * j));
                    dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                    for (i = 0; i < header.dwWidth; i++)
                    {
                        const RwUInt8 pixIn = *srcPixel;

                        dstPixel->red = (RwUInt8)((pixIn >> 0) & 0xE0);
                        dstPixel->green = (RwUInt8)((pixIn << 3) & 0xE0);
                        dstPixel->blue = (RwUInt8)((pixIn << 6) & 0xC0);
                        dstPixel->alpha = 0xFF;

                        srcPixel++;
                        dstPixel++;
                    }
                }
            }
        }
        else if (DDS_RGBA == header.ddspf.dwFlags)
        {
            RwRGBA* dstPixel;
            RwUInt32        j, i;

            if (0x00ff0000 == header.ddspf.dwRBitMask)
            {
                const RwUInt32* srcPixel;

                for (j = 0; j < header.dwHeight; j++)
                {
                    srcPixel = (const RwUInt32*)(pixels + (pitch * j));
                    dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                    for (i = 0; i < header.dwWidth; i++)
                    {
                        const RwUInt32 pixIn = *srcPixel;

                        srcPixel++;

                        dstPixel->red = (RwUInt8)((pixIn >> 16) & 0xFF);
                        dstPixel->green = (RwUInt8)((pixIn >> 8) & 0xFF);
                        dstPixel->blue = (RwUInt8)((pixIn) & 0xFF);
                        dstPixel->alpha = (RwUInt8)(pixIn >> 24);

                        dstPixel++;
                    }
                }
            }
            else if (0x00007c00 == header.ddspf.dwRBitMask)
            {
                const RwUInt16* srcPixel;

                rasFormat = rwRASTERFORMAT1555;

                for (j = 0; j < header.dwHeight; j++)
                {
                    srcPixel = (const RwUInt16*)(pixels + (pitch * j));
                    dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                    for (i = 0; i < header.dwWidth; i++)
                    {
                        const RwUInt16 pixIn = *srcPixel;

                        if ((pixIn >> 8) & 0x80)
                            dstPixel->alpha = 0xFF;
                        else
                            dstPixel->alpha = 0x00;

                        dstPixel->red = (RwUInt8)((pixIn >> 7) & 0xF8);
                        dstPixel->green = (RwUInt8)((pixIn >> 2) & 0xF8);
                        dstPixel->blue = (RwUInt8)((pixIn << 3) & 0xF8);

                        srcPixel++;
                        dstPixel++;
                    }
                }
            }
            else if (0x00000f00 == header.ddspf.dwRBitMask)
            {
                const RwUInt16* srcPixel;

                for (j = 0; j < header.dwHeight; j++)
                {
                    srcPixel = (const RwUInt16*)(pixels + (pitch * j));
                    dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                    for (i = 0; i < header.dwWidth; i++)
                    {
                        const RwUInt16 pixIn = *srcPixel;

                        dstPixel->alpha = (RwUInt8)((pixIn >> 8) & 0xF0);
                        dstPixel->red = (RwUInt8)((pixIn >> 4) & 0xF0);
                        dstPixel->green = (RwUInt8)((pixIn >> 0) & 0xF0);
                        dstPixel->blue = (RwUInt8)((pixIn << 4) & 0xF0);

                        srcPixel++;
                        dstPixel++;
                    }
                }
            }
            else if (0x000003ff == header.ddspf.dwRBitMask)
            {
                const RwUInt32* srcPixel;

                for (j = 0; j < header.dwHeight; j++)
                {
                    srcPixel = (const RwUInt32*)(pixels + (pitch * j));
                    dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                    for (i = 0; i < header.dwWidth; i++)
                    {
                        const RwUInt32 pixIn = *srcPixel;

                        srcPixel++;

                        dstPixel->red = (RwUInt8)((pixIn >> 22) & 0xFF);
                        dstPixel->green = (RwUInt8)((pixIn >> 12) & 0xFF);
                        dstPixel->blue = (RwUInt8)((pixIn >> 2) & 0xFF);
                        dstPixel->alpha = (RwUInt8)((pixIn >> 30)* (0xff / 0x03));

                        dstPixel++;
                    }
                }
            }
            else if (0x000000e0 == header.ddspf.dwRBitMask)
            {
                const RwUInt16* srcPixel;

                rasFormat = rwRASTERFORMAT4444;

                for (j = 0; j < header.dwHeight; j++)
                {
                    srcPixel = (const RwUInt16*)(pixels + (pitch * j));
                    dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                    for (i = 0; i < header.dwWidth; i++)
                    {
                        const RwUInt16 pixIn = *srcPixel;

                        dstPixel->red = (RwUInt8)((pixIn >> 0) & 0xE0);
                        dstPixel->green = (RwUInt8)((pixIn << 3) & 0xE0);
                        dstPixel->blue = (RwUInt8)((pixIn << 6) & 0xC0);
                        dstPixel->alpha = (RwUInt8)(pixIn >> 8);

                        srcPixel++;
                        dstPixel++;
                    }
                }
            }
            else if (0x000003ff == header.ddspf.dwBBitMask)
            {
                const RwUInt32* srcPixel;

                for (j = 0; j < header.dwHeight; j++)
                {
                    srcPixel = (const RwUInt32*)(pixels + (pitch * j));
                    dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                    for (i = 0; i < header.dwWidth; i++)
                    {
                        const RwUInt32 pixIn = *srcPixel;

                        srcPixel++;

                        dstPixel->red = (RwUInt8)((pixIn >> 2) & 0xFF);
                        dstPixel->green = (RwUInt8)((pixIn >> 12) & 0xFF);
                        dstPixel->blue = (RwUInt8)((pixIn >> 22) & 0xFF);
                        dstPixel->alpha = (RwUInt8)((pixIn >> 30)* (0xff / 0x03));

                        dstPixel++;
                    }
                }
            }
        }
        else
        {
            RwRGBA* dstPixel;
            RwUInt32        j, i;

            /* Luminance and palette formats */
            if (header.ddspf.dwRGBBitCount == 16)
            {
                const RwUInt16* srcPixel;

                if (0x0000ffff == header.ddspf.dwRBitMask) /* D3DFMT_L16 */
                {
                    if (_rwD3D9CheckValidTextureFormat(D3DFMT_L8))
                    {
                        rasFormat = rwRASTERFORMATLUM8;
                    }

                    for (j = 0; j < header.dwHeight; j++)
                    {
                        srcPixel = (const RwUInt16*)(pixels + (pitch * j));
                        dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                        for (i = 0; i < header.dwWidth; i++)
                        {
                            const RwUInt32 pixIn = ((*srcPixel) >> 8);

                            srcPixel++;

                            dstPixel->red = (RwUInt8)pixIn;
                            dstPixel->green = (RwUInt8)pixIn;
                            dstPixel->blue = (RwUInt8)pixIn;
                            dstPixel->alpha = 0xff;

                            dstPixel++;
                        }
                    }
                }
                else if (0x000000ff == header.ddspf.dwRBitMask &&
                    0x0000ff00 == header.ddspf.dwABitMask) /* D3DFMT_A8L8 */
                {
                    for (j = 0; j < header.dwHeight; j++)
                    {
                        srcPixel = (const RwUInt16*)(pixels + (pitch * j));
                        dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                        for (i = 0; i < header.dwWidth; i++)
                        {
                            const RwUInt32 pixIn = *srcPixel;

                            srcPixel++;

                            dstPixel->red = (RwUInt8)(pixIn & 0xff);
                            dstPixel->green = (RwUInt8)(pixIn & 0xff);
                            dstPixel->blue = (RwUInt8)(pixIn & 0xff);
                            dstPixel->alpha = (RwUInt8)(pixIn >> 8);

                            dstPixel++;
                        }
                    }
                }
            }
            else if (header.ddspf.dwRGBBitCount == 8)
            {
                const RwUInt8* srcPixel;

                if (0x000000ff == header.ddspf.dwRBitMask) /* D3DFMT_L8 */
                {
                    for (j = 0; j < header.dwHeight; j++)
                    {
                        srcPixel = (const RwUInt8*)(pixels + (pitch * j));
                        dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                        for (i = 0; i < header.dwWidth; i++)
                        {
                            const RwUInt32 pixIn = *srcPixel;

                            srcPixel++;

                            dstPixel->red = (RwUInt8)pixIn;
                            dstPixel->green = (RwUInt8)pixIn;
                            dstPixel->blue = (RwUInt8)pixIn;
                            dstPixel->alpha = 0xff;

                            dstPixel++;
                        }
                    }
                }
                else if (0x000000ff == header.ddspf.dwABitMask) /* D3DFMT_A8 */
                {
                    for (j = 0; j < header.dwHeight; j++)
                    {
                        srcPixel = (const RwUInt8*)(pixels + (pitch * j));
                        dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                        for (i = 0; i < header.dwWidth; i++)
                        {
                            const RwUInt32 pixIn = *srcPixel;

                            srcPixel++;

                            dstPixel->red = 0xff;
                            dstPixel->green = 0xff;
                            dstPixel->blue = 0xff;
                            dstPixel->alpha = (RwUInt8)pixIn;

                            dstPixel++;
                        }
                    }
                }
                else if (0x0000000f == header.ddspf.dwRBitMask &&
                    0x000000f0 == header.ddspf.dwABitMask) /* D3DFMT_A4L4 */
                {
                    for (j = 0; j < header.dwHeight; j++)
                    {
                        srcPixel = (const RwUInt8*)(pixels + (pitch * j));
                        dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                        for (i = 0; i < header.dwWidth; i++)
                        {
                            const RwUInt32 pixIn = *srcPixel;

                            srcPixel++;

                            dstPixel->red = (RwUInt8)(pixIn << 4);
                            dstPixel->green = (RwUInt8)(pixIn << 4);
                            dstPixel->blue = (RwUInt8)(pixIn << 4);
                            dstPixel->alpha = (RwUInt8)(pixIn & 0xf0);

                            dstPixel++;
                        }
                    }
                }
                else if (0 == header.ddspf.dwRBitMask &&
                    0 == header.ddspf.dwGBitMask &&
                    0 == header.ddspf.dwBBitMask &&
                    0 == header.ddspf.dwABitMask)
                {
                    for (j = 0; j < header.dwHeight; j++)
                    {
                        srcPixel = (const RwUInt8*)(pixels + (pitch * j));
                        dstPixel = (RwRGBA*)(image->cpPixels + (image->stride * j));

                        for (i = 0; i < header.dwWidth; i++)
                        {
                            const RwUInt32 pixIn = *srcPixel;

                            srcPixel++;

                            dstPixel->red = palette[pixIn].red;
                            dstPixel->green = palette[pixIn].green;
                            dstPixel->blue = palette[pixIn].blue;
                            dstPixel->alpha = palette[pixIn].alpha;

                            dstPixel++;
                        }
                    }
                }
            }
        }

        RwFree(pixels);

        if (palette != NULL)
        {
            RwFree(palette);
        }

        if (rasFormat == 0)
        {
            rasFormat = (RwRasterFormat)_rwD3D9ImageFindFormat(image);
            rasFormat = (RwRasterFormat)D3D9FindCorrectRasterFormat(rwRASTERTYPETEXTURE, rasFormat);
        }

        if (mipMapped)
            mipMapped |= rwRASTERFORMATAUTOMIPMAP;

        /* Create a raster */
        raster = RwRasterCreate(header.dwWidth, header.dwHeight, 0, rwRASTERTYPETEXTURE | rasFormat | mipMapped);

        if (raster == nullptr)
        {
            RwImageDestroy(image);
           return nullptr;
        }

        /* Check dimensions */
        if (RwImageGetWidth(image) != RwRasterGetWidth(raster) ||
            RwImageGetHeight(image) != RwRasterGetHeight(raster))
        {
            RwImage* resampledimage;

            resampledimage = RwImageCreateResample(image, RwRasterGetWidth(raster), RwRasterGetHeight(raster));

            RwImageDestroy(image);

            image = resampledimage;
        }

        /* Convert image to raster */
        RwRasterSetFromImage(raster, image);

        RwImageDestroy(image);
    }

    return(raster);
}

RwTexture* Raster::rwD3D9DXTTextureRead(const RwChar* name, const RwChar* fullPathName)
{
    RwStream* stream;
    RwRaster* raster;
    RwTexture* texture;

    stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, fullPathName);
    if (!stream)
       return nullptr;

    /* Create raster from stream */
    raster = RwD3D9RasterStreamReadDDS(stream);

    /* Close the stream */
    RwStreamClose(stream, NULL);

    /* Check raster */
    if (raster == NULL)
       return nullptr;

    /* Create the texture */
    texture = RwTextureCreate(raster);
    if (!texture)
       return nullptr;

    if (RwRasterGetFormat(raster) & rwRASTERFORMATMIPMAP)
        RwTextureSetFilterMode(texture, rwFILTERLINEARMIPLINEAR);
    else
        RwTextureSetFilterMode(texture, rwFILTERLINEAR);

    /* Set the textures name */
    RwTextureSetName(texture, name);

    return(texture);
}

RwTexture* Raster::RwD3D9DDSTextureRead(const RwChar* name, const RwChar* maskname __RWUNUSED__)
{
    RwChar      fullPathName[256];
    RwChar* pathElement;

    pathElement = RwImageGetPath();

    /*
     * If we have an absolute path, ignore the search directory.
     * If we have no search path, use the current directory.
     * Either way, just copy the name.
     */
    if ((_rwpathisabsolute(name) == TRUE) ||
        (pathElement == NULL) ||
        (pathElement[0] == '\0'))
    {
        const RwChar* curChar;

        curChar = name + strlen(name);

        do
        {
            curChar--;
        } while (curChar >= name && '\\' != *curChar);

        strcpy(fullPathName, name);
        strcat(fullPathName, ".dds");

        /* Try to load the texture */
        if (RwFexist(fullPathName) != FALSE)
            return(rwD3D9DXTTextureRead(curChar + 1, fullPathName));
    }
    else
    {
        /* While we have a search path to try */
        while (pathElement && pathElement[0] != '\0')
        {
            RwChar* nextPathElement = strchr(pathElement, ';');
            RwInt32     pathElementLength;

            nextPathElement = strchr(pathElement, ';');
            if (nextPathElement)
            {
                /* Length of pathstring (in bytes) */
                pathElementLength = (RwUInt8*)nextPathElement - (RwUInt8*)pathElement;
                nextPathElement++;  /* Skip the separator */
            }
            else
                pathElementLength = strlen(pathElement) * sizeof(RwChar);

            /* Full path + full name */
            memcpy(fullPathName, pathElement, pathElementLength);
            strcpy((RwChar*)(((RwUInt8*)fullPathName) + pathElementLength), name);
            strcat(fullPathName, ".dds");

            /* Try to load the texture */
            if (RwFexist(fullPathName) != FALSE)
                return(rwD3D9DXTTextureRead(name, fullPathName));

            /* On to the next path */
            pathElement = nextPathElement;
        }
    }

   return nullptr;
}