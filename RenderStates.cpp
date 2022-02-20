#include "RenderStates.h"
#include "RWCommon.h"

#define FORCE_SAMPLERSTATE(stage, type, newvalue)                           \
{                                                                           \
    SamplerStageStateD3D[stage][type] = newvalue;                           \
     _RwD3DDevice->SetSamplerState(stage, type, newvalue);  \
}

#define FORCE_RENDERSTATE(state, newvalue)                          \
{                                                                   \
    RenderStateCache[state].value = newvalue;                       \
    if (!RenderStateCache[state].changed)                           \
    {                                                               \
        RenderStateCache[state].changed = TRUE;                     \
        RenderStateChanges[NumRenderStateChanges++] = state;        \
    }                                                               \
}

_rwD3D9StateCache& _RwD3D9StateCache = *(_rwD3D9StateCache*)0xC9A4C0;

RwInt32& D3D9AnisotTextureOffset = *(RwInt32*)0xC9A5E8;
#define CONSTTEXTUREGETANISOT(tex) \
    (*((const RwUInt8*)(((const RwUInt8*)tex) + D3D9AnisotTextureOffset)))

typedef struct
{
    uint32_t value;
    bool changed;
} RxStateCache;
RxStateCache(&RenderStateCache)[210] = *(RxStateCache(*)[210])0x00C991D0;
uint32_t(&RenderStateD3D)[210] = *(uint32_t(*)[210])0x00C98E88;

uint32_t& NumRenderStateChanges = *(uint32_t*)0xC9A5EC;
D3DRENDERSTATETYPE(&RenderStateChanges)[210] = *(D3DRENDERSTATETYPE(*)[210])0x00C98B40;

RxStateCache(&TextureStageStateCache)[RWD3D9_MAX_TEXTURE_STAGES][33] = *(RxStateCache(*)[RWD3D9_MAX_TEXTURE_STAGES][33])0x00C980F8;
uint32_t(&TextureStageStateD3D)[RWD3D9_MAX_TEXTURE_STAGES][33] = *(uint32_t(*)[RWD3D9_MAX_TEXTURE_STAGES][33])0x00C99860;

uint32_t& NumTextureStageStateChanges = *(uint32_t*)0xC9A5F0;
RxTextureStageChangeType(&TextureStageStateChanges)[RWD3D9_MAX_TEXTURE_STAGES * 33] = *(RxTextureStageChangeType(*)[RWD3D9_MAX_TEXTURE_STAGES * 33])0x00C99C80;
uint32_t(&SamplerStageStateD3D)[RWD3D9_MAX_TEXTURE_STAGES][14] = *(uint32_t(*)[RWD3D9_MAX_TEXTURE_STAGES][14])0x00C98938;

void RenderStates::RenderStateFlushCache()
{
    if (NumRenderStateChanges)
    {
        auto n = 0u;
        do
        {
            auto stateChanged = RenderStateChanges[n];
            auto newValue = RenderStateCache[stateChanged].value;

            RenderStateCache[stateChanged].changed = FALSE;

            if (RenderStateD3D[stateChanged] != newValue)
            {
                RenderStateD3D[stateChanged] = newValue;

                _RwD3DDevice->SetRenderState(stateChanged, newValue);
            }

        } while (++n < NumRenderStateChanges);

        NumRenderStateChanges = 0;
    }

    if (NumTextureStageStateChanges)
    {
        auto n = 0u;
        do
        {
            const RwUInt32 stage = TextureStageStateChanges[n].stage;
            const D3DTEXTURESTAGESTATETYPE type = TextureStageStateChanges[n].type;
            const RwUInt32 newValue = TextureStageStateCache[stage][type].value;

            TextureStageStateCache[stage][type].changed = FALSE;

            if (TextureStageStateD3D[stage][type] != newValue)
            {
                TextureStageStateD3D[stage][type] = newValue;

                _RwD3DDevice->SetTextureStageState(stage, type, newValue);
            }


        } while (++n < NumTextureStageStateChanges);

        NumTextureStageStateChanges = 0;
    }
}

void RenderStates::SetRenderState(RwUInt32 state, RwUInt32 value)
{
    if (RenderStateCache[state].value != value)
    {
        RenderStateCache[state].value = value;

        if (!RenderStateCache[state].changed)
        {
            RenderStateCache[state].changed = TRUE;
            RenderStateChanges[NumRenderStateChanges++] = (D3DRENDERSTATETYPE)state;
        }
    }
}

void RenderStates::GetRenderState(uint32_t state, void* value)
{
    *reinterpret_cast<uint32_t*>(value) = RenderStateCache[state].value;
}

void RenderStates::SetTextureStageState(uint32_t stage, uint32_t type, uint32_t value)
{
    if (TextureStageStateCache[stage][type].value != value)
    {
        TextureStageStateCache[stage][type].value = value;

        if (!TextureStageStateCache[stage][type].changed)
        {
            TextureStageStateCache[stage][type].changed = true;
            TextureStageStateChanges[NumTextureStageStateChanges].stage = stage;
            TextureStageStateChanges[NumTextureStageStateChanges].type = (D3DTEXTURESTAGESTATETYPE)type;
            NumTextureStageStateChanges++;
        }
    }
}

void RenderStates::GetTextureStageState(uint32_t stage, uint32_t type, void* value)
{
    *reinterpret_cast<uint32_t*>(value) = TextureStageStateCache[stage][type].value;
}

void RenderStates::SetSamplerState(uint32_t stage, uint32_t type, uint32_t value)
{
    if (SamplerStageStateD3D[stage][type] != value)
    {
        SamplerStageStateD3D[stage][type] = value;

        RwD3DDevice->SetSamplerState(stage, (D3DSAMPLERSTATETYPE)type, value);
    }
}

void RenderStates::GetSamplerState(uint32_t stage, uint32_t type, void* value)
{
    *reinterpret_cast<uint32_t*>(value) = SamplerStageStateD3D[stage][type];
}

void RenderStates::ForceSamplerState(uint32_t stage, uint32_t type, uint32_t value)
{
    SamplerStageStateD3D[stage][type] = value;

    RwD3DDevice->SetSamplerState(stage, (D3DSAMPLERSTATETYPE)type, value);
}

void RenderStates::ForceRenderState(uint32_t state, uint32_t value)
{
    RenderStateCache[state].value = value;
    RenderStateD3D[state] = value;

    RwD3DDevice->SetRenderState((D3DRENDERSTATETYPE)state, value);
}

void RenderStates::ForceTextureStageState(uint32_t stage, uint32_t type, uint32_t value)
{
    TextureStageStateCache[stage][type].value = value;
    TextureStageStateD3D[stage][type] = value;

    RwD3DDevice->SetTextureStageState(stage, (D3DTEXTURESTAGESTATETYPE)type, value);
}

RwBool RenderStates::rwD3D9RenderStateFogEnable(RwBool fog)
{
    if (fog)
    {
        if (!_RwD3D9StateCache.fogEnable)
        {
            if (_RwD3D9DeviceCaps.RasterCaps & (D3DPRASTERCAPS_FOGTABLE | D3DPRASTERCAPS_FOGVERTEX))
            {
                FORCE_RENDERSTATE(D3DRS_FOGENABLE, TRUE);
                _RwD3D9StateCache.fogEnable = TRUE;
            }
        }
    }
    else
    {
        if (_RwD3D9StateCache.fogEnable)
        {
            FORCE_RENDERSTATE(D3DRS_FOGENABLE, FALSE);
            _RwD3D9StateCache.fogEnable = FALSE;
        }
    }

    return TRUE;
}

RwBool RenderStates::rwD3D9RenderStateFogColor(RwUInt32 fogColor)
{
    if (fogColor != _RwD3D9StateCache.fogColor)
    {
        FORCE_RENDERSTATE(D3DRS_FOGCOLOR, fogColor);
        _RwD3D9StateCache.fogColor = fogColor;
    }

    return TRUE;
}

RwBool RenderStates::rwD3D9RenderStateFogType(RwFogType fogType)
{
    if (fogType != _RwD3D9StateCache.fogType)
    {
        if ((_RwD3D9DeviceCaps.RasterCaps & D3DPRASTERCAPS_FOGTABLE) != 0 &&
            (_RwD3D9DeviceCaps.RasterCaps & D3DPRASTERCAPS_WFOG) != 0)
        {
            FORCE_RENDERSTATE(D3DRS_FOGTABLEMODE, _RwD3D9FogConvTable[fogType]);

            if (RenderStateCache[D3DRS_FOGVERTEXMODE].value != D3DFOG_NONE)
            {
                FORCE_RENDERSTATE(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
            }
        }
        else
        {
            if (RenderStateCache[D3DRS_FOGTABLEMODE].value != D3DFOG_NONE)
            {
                FORCE_RENDERSTATE(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
            }

            FORCE_RENDERSTATE(D3DRS_FOGVERTEXMODE, _RwD3D9FogConvTable[fogType]);
        }

        _RwD3D9StateCache.fogType = fogType;
    }

   return true;
}

RwBool RenderStates::rwD3D9RenderStateFogDensity(RwReal fogDensity)
{
    if (fogDensity != _RwD3D9StateCache.fogDensity)
    {
        FORCE_RENDERSTATE(D3DRS_FOGDENSITY, FLOATASDWORD(fogDensity));
        _RwD3D9StateCache.fogDensity = fogDensity;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RenderStateTextureAddress(RwTextureAddressMode addressMode)
{

    if (addressMode != _RwD3D9StateCache.stage[0].texAddressU)
    {
        _RwD3D9StateCache.stage[0].texAddressU = addressMode;

        FORCE_SAMPLERSTATE(0, D3DSAMP_ADDRESSU, _RwD3D9AddressConvTable[addressMode]);
    }

    if (addressMode != _RwD3D9StateCache.stage[0].texAddressV)
    {
        _RwD3D9StateCache.stage[0].texAddressV = addressMode;

        FORCE_SAMPLERSTATE(0, D3DSAMP_ADDRESSV, _RwD3D9AddressConvTable[addressMode]);
    }

   return true;
}

RwBool  RenderStates::_rwD3D9RenderStateTextureAddressU(RwTextureAddressMode addressMode)
{
    if (addressMode != _RwD3D9StateCache.stage[0].texAddressU)
    {
        _RwD3D9StateCache.stage[0].texAddressU = addressMode;

        FORCE_SAMPLERSTATE(0, D3DSAMP_ADDRESSU, _RwD3D9AddressConvTable[addressMode]);
    }

   return true;
}

RwBool RenderStates::_rwD3D9RenderStateTextureAddressV(RwTextureAddressMode addressMode)
{
    if (addressMode != _RwD3D9StateCache.stage[0].texAddressV)
    {
        _RwD3D9StateCache.stage[0].texAddressV = addressMode;

        FORCE_SAMPLERSTATE(0, D3DSAMP_ADDRESSV, _RwD3D9AddressConvTable[addressMode]);
    }

   return true;
}

RwBool  RenderStates::_rwD3D9RenderStateTextureFilter(RwTextureFilterMode filterMode)
{
    if (_RwD3D9StateCache.stage[0].maxAnisotropy > 1)
    {
        _RwD3D9StateCache.stage[0].maxAnisotropy = 1;

        FORCE_SAMPLERSTATE(0, D3DSAMP_MAXANISOTROPY, 1);
    }

    if (filterMode != _RwD3D9StateCache.stage[0].filterMode)
    {
        const RwUInt32 minmag = _RwD3D9FilterModeConvTable[filterMode].minmag;
        const RwUInt32 mip = _RwD3D9FilterModeConvTable[filterMode].mip;

        _RwD3D9StateCache.stage[0].filterMode = filterMode;

        FORCE_SAMPLERSTATE(0, D3DSAMP_MINFILTER, minmag);
        FORCE_SAMPLERSTATE(0, D3DSAMP_MAGFILTER, minmag);

        if (SamplerStageStateD3D[0][D3DSAMP_MIPFILTER] != mip)
        {
            FORCE_SAMPLERSTATE(0, D3DSAMP_MIPFILTER, mip);
        }
    }

   return true;
}

RwBool RenderStates::_rwD3D9RenderStateSrcBlend(RwBlendFunction srcBlend)
{
    if (srcBlend != _RwD3D9StateCache.srcBlend)
    {
        FORCE_RENDERSTATE(D3DRS_SRCBLEND, _RwD3D9BlendConvTable[srcBlend]);

        _RwD3D9StateCache.srcBlend = srcBlend;
    }

   return true;
}

RwBool  RenderStates::_rwD3D9RenderStateDestBlend(RwBlendFunction dstBlend)
{
    if (dstBlend != _RwD3D9StateCache.dstBlend)
    {
        FORCE_RENDERSTATE(D3DRS_DESTBLEND, _RwD3D9BlendConvTable[dstBlend]);

        _RwD3D9StateCache.dstBlend = dstBlend;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RWSetRasterStage(RwRaster* raster, RwUInt32 stage)
{
    _rwD3D9RasterExt* rasExt;


    /*
     * Check alpha blending
     */
    if (raster)
    {
        rasExt = RASTEREXTFROMRASTER(raster);
    }
    else
    {
        rasExt = (_rwD3D9RasterExt*)NULL;
    }

    if (stage == 0)
    {
        if (raster && rasExt->alpha)
        {
            /*
             * Enable Alpha Blending if the texture has alpha
             */
            if (!_RwD3D9StateCache.textureAlphaBlendEnable)
            {
                _RwD3D9StateCache.textureAlphaBlendEnable = TRUE;

                if (!_RwD3D9StateCache.vertexAlphaBlendEnable)
                {
                    FORCE_RENDERSTATE(D3DRS_ALPHABLENDENABLE, TRUE);

                    RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, _RwD3D9StateCache.alphaTestEnable);
                }
            }
        }
        else
        {
            if (_RwD3D9StateCache.textureAlphaBlendEnable)
            {
                _RwD3D9StateCache.textureAlphaBlendEnable = FALSE;

                /*
                * Disable alpha blending if it has not been explicity set
                */
                if (!_RwD3D9StateCache.vertexAlphaBlendEnable)
                {
                    FORCE_RENDERSTATE(D3DRS_ALPHABLENDENABLE, FALSE);

                    RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
                }
            }

        }
    }

    /*
     * Check color
     */
    if (raster != _RwD3D9StateCache.stage[stage].curTexRaster)
    {
        _RwD3D9StateCache.stage[stage].curTexRaster = raster;

        if (raster)
        {
            /* Set the texture */
             _RwD3DDevice->SetTexture(stage, (LPDIRECT3DBASETEXTURE9)(rasExt->texture));

            /* Set the palette if it is a palettized texture */
            if (rasExt->palette)
            {
                 _RwD3DDevice->SetCurrentTexturePalette(rasExt->palette->globalindex);
            }
        }
        else
        {
             _RwD3DDevice->SetTexture(stage, NULL);
        }
    }

   return true;
}

RwRaster* RenderStates::_rwD3D9RWGetRasterStage(RwUInt32 stage)
{
    return _RwD3D9StateCache.stage[stage].curTexRaster;
}

void  RenderStates::_rwD3D9SetTextureAnisotropyOffset(RwInt32 anisotTextureOffset)
{
    D3D9AnisotTextureOffset = anisotTextureOffset;
}

RwBool RenderStates::RwD3D9SetTexture(RwTexture* texture, RwUInt32 stage)
{
    if (texture)
    {
        RwTextureAddressMode    addressingU;
        RwTextureAddressMode    addressingV;
        RwTextureFilterMode     filtering;

        /* Addressing mode */
        addressingU = RwTextureGetAddressingU(texture);
        if (addressingU != _RwD3D9StateCache.stage[stage].texAddressU)
        {
            _RwD3D9StateCache.stage[stage].texAddressU = addressingU;

            FORCE_SAMPLERSTATE(stage, D3DSAMP_ADDRESSU, _RwD3D9AddressConvTable[addressingU]);
        }

        addressingV = RwTextureGetAddressingV(texture);
        if (addressingV != _RwD3D9StateCache.stage[stage].texAddressV)
        {
            _RwD3D9StateCache.stage[stage].texAddressV = addressingV;

            FORCE_SAMPLERSTATE(stage, D3DSAMP_ADDRESSV, _RwD3D9AddressConvTable[addressingV]);
        }

        /* Filter mode */
        filtering = RwTextureGetFilterMode(texture);

        if (D3D9AnisotTextureOffset > 0)
        {
            const RwInt32 anisotropy = CONSTTEXTUREGETANISOT(texture);

            if (_RwD3D9StateCache.stage[stage].maxAnisotropy != anisotropy)
            {
                _RwD3D9StateCache.stage[stage].maxAnisotropy = anisotropy;

                FORCE_SAMPLERSTATE(stage, D3DSAMP_MAXANISOTROPY, anisotropy);
            }

            if (anisotropy > 1)
            {
                filtering = (RwTextureFilterMode)NUMFILTERMODES;
            }
        }

        if (_RwD3D9StateCache.stage[stage].filterMode != filtering)
        {
            D3DTEXTUREFILTERTYPE    minmag, mip;

            _RwD3D9StateCache.stage[stage].filterMode = filtering;

            minmag = _RwD3D9FilterModeConvTable[filtering].minmag;
            mip = _RwD3D9FilterModeConvTable[filtering].mip;

            if (minmag == D3DTEXF_ANISOTROPIC)
            {
                FORCE_SAMPLERSTATE(stage, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
                if (_RwD3D9DeviceCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC)
                {
                    FORCE_SAMPLERSTATE(stage, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
                }
                else
                {
                    FORCE_SAMPLERSTATE(stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                }
            }
            else
            {
                FORCE_SAMPLERSTATE(stage, D3DSAMP_MINFILTER, minmag);
                FORCE_SAMPLERSTATE(stage, D3DSAMP_MAGFILTER, minmag);
            }

            FORCE_SAMPLERSTATE(stage, D3DSAMP_MIPFILTER, mip);
        }

        /* Raster */
        _rwD3D9RWSetRasterStage(texture->raster, stage);

    }
    else
    {
        if (stage == 0)
        {
            if (_RwD3D9StateCache.textureAlphaBlendEnable)
            {
                _RwD3D9StateCache.textureAlphaBlendEnable = FALSE;

                /*
                * Disable alpha blending if it has not been explicity set
                */
                if (!_RwD3D9StateCache.vertexAlphaBlendEnable)
                {
                    FORCE_RENDERSTATE(D3DRS_ALPHABLENDENABLE, FALSE);

                    SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
                }
            }
        }

        if (NULL != _RwD3D9StateCache.stage[stage].curTexRaster)
        {
            _RwD3D9StateCache.stage[stage].curTexRaster = NULL;

             _RwD3DDevice->SetTexture(stage, NULL);
        }
    }

   return true;
}

RwBool RenderStates::rwD3D9RenderStateZWriteEnable(RwBool enable)
{
    if (enable)
    {
        if (!_RwD3D9StateCache.zWriteEnable)
        {
            if (!_RwD3D9StateCache.zTestEnable)
            {
                FORCE_RENDERSTATE(D3DRS_ZENABLE, TRUE);
            }

            FORCE_RENDERSTATE(D3DRS_ZWRITEENABLE, TRUE);

            _RwD3D9StateCache.zWriteEnable = TRUE;
        }
    }
    else
    {
        if (_RwD3D9StateCache.zWriteEnable)
        {
            if (!_RwD3D9StateCache.zTestEnable)
            {
                FORCE_RENDERSTATE(D3DRS_ZENABLE, FALSE);
            }

            FORCE_RENDERSTATE(D3DRS_ZWRITEENABLE, FALSE);

            _RwD3D9StateCache.zWriteEnable = FALSE;
        }
    }
   return true;
}

RwBool RenderStates::rwD3D9RenderStateZTestEnable(RwBool enable)
{

    if (enable)
    {
        if (!_RwD3D9StateCache.zTestEnable)
        {
            if (!_RwD3D9StateCache.zWriteEnable)
            {
                FORCE_RENDERSTATE(D3DRS_ZENABLE, TRUE);
            }

            FORCE_RENDERSTATE(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

            _RwD3D9StateCache.zTestEnable = TRUE;
        }
    }
    else
    {
        if (_RwD3D9StateCache.zTestEnable)
        {
            if (!_RwD3D9StateCache.zWriteEnable)
            {
                FORCE_RENDERSTATE(D3DRS_ZENABLE, FALSE);
            }

            FORCE_RENDERSTATE(D3DRS_ZFUNC, D3DCMP_ALWAYS);

            _RwD3D9StateCache.zTestEnable = FALSE;
        }
    }

   return true;
}

RwBool RenderStates::rwD3D9RenderStateShadeMode(RwShadeMode shadeMode)
{
    if (shadeMode != _RwD3D9StateCache.shadeMode)
    {
        FORCE_RENDERSTATE(D3DRS_SHADEMODE, _RwD3D9ShadeModeConvTable[shadeMode]);
        _RwD3D9StateCache.shadeMode = shadeMode;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RenderStateVertexAlphaEnable(RwBool enable)
{
    if (enable)
    {
        if (!_RwD3D9StateCache.vertexAlphaBlendEnable)
        {
            _RwD3D9StateCache.vertexAlphaBlendEnable = TRUE;

            if (!_RwD3D9StateCache.textureAlphaBlendEnable)
            {
                FORCE_RENDERSTATE(D3DRS_ALPHABLENDENABLE, TRUE);

                RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, _RwD3D9StateCache.alphaTestEnable);
            }
        }
    }
    else
    {
        if (_RwD3D9StateCache.vertexAlphaBlendEnable)
        {
            _RwD3D9StateCache.vertexAlphaBlendEnable = FALSE;

            if (!_RwD3D9StateCache.textureAlphaBlendEnable)
            {
                FORCE_RENDERSTATE(D3DRS_ALPHABLENDENABLE, FALSE);

                RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
            }
        }
    }

   return true;
}

RwBool  RenderStates::_rwD3D9RenderStateIsVertexAlphaEnable(void)
{
    return(_RwD3D9StateCache.vertexAlphaBlendEnable);
}

RwBool RenderStates::rwD3D9RenderStateBorderColor(RwUInt32 borderColor)
{

    if (borderColor != _RwD3D9StateCache.stage[0].borderColor)
    {
        _RwD3D9StateCache.stage[0].borderColor = borderColor;

        FORCE_SAMPLERSTATE(0, D3DSAMP_BORDERCOLOR, borderColor);
    }

   return true;
}


RwBool rwD3D9RenderStateTexturePerspective(RwBool persp)
{
    /* Only can set on */
    if (persp)
    {
       return true;
    }

    /* If trying to set off, fail */
   return false;
}

RwBool RenderStates::rwD3D9RenderStateCullMode(RwCullMode cullMode)
{
    if (cullMode != _RwD3D9StateCache.cullMode)
    {
        FORCE_RENDERSTATE(D3DRS_CULLMODE, _RwD3D9CullModeConvTable[cullMode]);

        _RwD3D9StateCache.cullMode = cullMode;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RenderStateStencilEnable(RwBool enable)
{

    if (enable != _RwD3D9StateCache.stencilEnable)
    {
        FORCE_RENDERSTATE(D3DRS_STENCILENABLE, enable);

        _RwD3D9StateCache.stencilEnable = enable;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RenderStateStencilFail(RwStencilOperation operation)
{
    if (operation != _RwD3D9StateCache.stencilFailOp)
    {
        FORCE_RENDERSTATE(D3DRS_STENCILFAIL, _RwD3D9StencilOpConvTable[operation]);

        _RwD3D9StateCache.stencilFailOp = operation;
    }

   return true;
}

RwBool  RenderStates::_rwD3D9RenderStateStencilZFail(RwStencilOperation operation)
{
    if (operation != _RwD3D9StateCache.stencilZFailOp)
    {
        FORCE_RENDERSTATE(D3DRS_STENCILZFAIL, _RwD3D9StencilOpConvTable[operation]);

        _RwD3D9StateCache.stencilZFailOp = operation;
    }

   return true;
}

RwBool  RenderStates::_rwD3D9RenderStateStencilPass(RwStencilOperation operation)
{
    if (operation != _RwD3D9StateCache.stencilPassOp)
    {
        FORCE_RENDERSTATE(D3DRS_STENCILPASS, _RwD3D9StencilOpConvTable[operation]);

        _RwD3D9StateCache.stencilPassOp = operation;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RenderStateStencilFunction(RwStencilFunction function)
{
    if (function != _RwD3D9StateCache.stencilFunction)
    {
        FORCE_RENDERSTATE(D3DRS_STENCILFUNC, _RwD3D9CmpFuncConvTable[function]);

        _RwD3D9StateCache.stencilFunction = function;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RenderStateStencilFunctionRef(RwInt32 ref)
{
    if (ref != _RwD3D9StateCache.stencilRef)
    {
        FORCE_RENDERSTATE(D3DRS_STENCILREF, ref);

        _RwD3D9StateCache.stencilRef = ref;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RenderStateStencilFunctionMask(RwInt32 mask)
{
    if (mask != _RwD3D9StateCache.stencilMask)
    {
        FORCE_RENDERSTATE(D3DRS_STENCILMASK, mask);

        _RwD3D9StateCache.stencilMask = mask;
    }

   return true;
}

RwBool  RenderStates::_rwD3D9RenderStateStencilFunctionWriteMask(RwInt32 writeMask)
{
    if (writeMask != _RwD3D9StateCache.stencilWriteMask)
    {
        FORCE_RENDERSTATE(D3DRS_STENCILWRITEMASK, writeMask);

        _RwD3D9StateCache.stencilWriteMask = writeMask;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RWSetRenderState(RwRenderState state, void* param)
{
    RwBool  result = FALSE;
    D3D9UnionParam uparam;
    /*
     * Need to be in a camera update cliche for this to work
     */

    uparam.VoidPtr = param;

    switch (state)
    {
        /***************************** FOG ******************************/

    case rwRENDERSTATEFOGENABLE:
        result = rwD3D9RenderStateFogEnable(uparam.Bool);
        break;

    case rwRENDERSTATEFOGCOLOR:
        result = rwD3D9RenderStateFogColor(uparam.UInt32);
        break;

    case rwRENDERSTATEFOGTYPE:
        result = rwD3D9RenderStateFogType(uparam.FogType);
        break;

    case rwRENDERSTATEFOGDENSITY:
        result = rwD3D9RenderStateFogDensity(*uparam.RealPtr);
        break;

        /********************** TEXTURE ADDRESSING **********************/

    case rwRENDERSTATETEXTUREADDRESS:
        result = _rwD3D9RenderStateTextureAddress(uparam.TextureAddressMode);
        break;

    case rwRENDERSTATETEXTUREADDRESSU:
        result = _rwD3D9RenderStateTextureAddressU(uparam.TextureAddressMode);
        break;

    case rwRENDERSTATETEXTUREADDRESSV:
        result = _rwD3D9RenderStateTextureAddressV(uparam.TextureAddressMode);
        break;

        /********************* TEXTURE FILTER MODE **********************/

    case rwRENDERSTATETEXTUREFILTER:
        result = _rwD3D9RenderStateTextureFilter(uparam.TextureFilterMode);
        break;

        /************************ TEXTURE RASTER ************************/

    case rwRENDERSTATETEXTURERASTER:
        result = _rwD3D9RWSetRasterStage(uparam.RasterPtr, 0);
        break;

        /************************ Z BUFFER WRITE ************************/

    case rwRENDERSTATEZWRITEENABLE:
        result = rwD3D9RenderStateZWriteEnable(uparam.Bool);
        break;

        /************************ Z BUFFER TEST *************************/

    case rwRENDERSTATEZTESTENABLE:
        result = rwD3D9RenderStateZTestEnable(uparam.Bool);
        break;

        /************************ BLENDING MODES ************************/

    case rwRENDERSTATESRCBLEND:
        result = _rwD3D9RenderStateSrcBlend(uparam.BlendFunction);
        break;

    case rwRENDERSTATEDESTBLEND:
        result = _rwD3D9RenderStateDestBlend(uparam.BlendFunction);
        break;

        /************************* SHADING MODE *************************/

    case rwRENDERSTATESHADEMODE:
        result = rwD3D9RenderStateShadeMode(uparam.ShadeMode);
        break;

        /************************* VERTEX ALPHA *************************/

    case rwRENDERSTATEVERTEXALPHAENABLE:
        result = _rwD3D9RenderStateVertexAlphaEnable(uparam.Bool);
        break;

        /************************ BORDER COLOR **************************/

    case rwRENDERSTATEBORDERCOLOR:
        result = rwD3D9RenderStateBorderColor(uparam.UInt32);
        break;

        /*************** PERSPECTIVELY CORRECT TEXTUREING ***************/

    case rwRENDERSTATETEXTUREPERSPECTIVE:
        result = rwD3D9RenderStateTexturePerspective(uparam.Bool);
        break;

        /************************* CULLING MODE *************************/

    case rwRENDERSTATECULLMODE:
        result = rwD3D9RenderStateCullMode(uparam.CullMode);
        break;

        /************************** STENCIL BUFFER **************************/

    case rwRENDERSTATESTENCILENABLE:
        result = _rwD3D9RenderStateStencilEnable(uparam.Bool);
        break;

    case rwRENDERSTATESTENCILFAIL:
        result = _rwD3D9RenderStateStencilFail(uparam.StencilOperation);
        break;

    case rwRENDERSTATESTENCILZFAIL:
        result = _rwD3D9RenderStateStencilZFail(uparam.StencilOperation);
        break;

    case rwRENDERSTATESTENCILPASS:
        result = _rwD3D9RenderStateStencilPass(uparam.StencilOperation);
        break;

    case rwRENDERSTATESTENCILFUNCTION:
        result = _rwD3D9RenderStateStencilFunction(uparam.StencilFunction);
        break;

    case rwRENDERSTATESTENCILFUNCTIONREF:
        result = _rwD3D9RenderStateStencilFunctionRef(uparam.Int32);
        break;

    case rwRENDERSTATESTENCILFUNCTIONMASK:
        result = _rwD3D9RenderStateStencilFunctionMask(uparam.Int32);
        break;

    case rwRENDERSTATESTENCILFUNCTIONWRITEMASK:
        result = _rwD3D9RenderStateStencilFunctionWriteMask(uparam.Int32);
        break;

    case rwRENDERSTATEALPHATESTFUNCTION:
        if (uparam.AlphaTestFunction != _RwD3D9StateCache.alphaTestFunction)
        {
            _RwD3D9StateCache.alphaTestFunction = uparam.AlphaTestFunction;

            _RwD3D9StateCache.alphaTestEnable = (uparam.AlphaTestFunction != rwALPHATESTFUNCTIONALWAYS);

            if (RenderStateCache[D3DRS_ALPHABLENDENABLE].value)
            {
                RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, _RwD3D9StateCache.alphaTestEnable);
            }

            FORCE_RENDERSTATE(D3DRS_ALPHAFUNC,
                _RwD3D9CmpFuncConvTable[uparam.AlphaTestFunction]);
        }
        result = TRUE;
        break;

    case rwRENDERSTATEALPHATESTFUNCTIONREF:
        RwD3D9SetRenderState(D3DRS_ALPHAREF, uparam.UInt32);
        result = TRUE;
        break;

        /************************ NOT SUPPORTED *************************/

    case rwRENDERSTATENARENDERSTATE:
        break;

    case rwRENDERSTATEFORCEENUMSIZEINT:
        break;
    }

    return result;
}

RwBool RenderStates::_rwD3D9RWGetRenderState(RwRenderState nState, void* param)
{
    switch (nState)
    {
        /******************************* FOG ********************************/
    case rwRENDERSTATEFOGENABLE:
    {
        /* Return our conveniently cached value */
        *(RwBool*)param = _RwD3D9StateCache.fogEnable;

        return(TRUE);
    }

    case rwRENDERSTATEFOGTYPE:
    {
        /* Return our conveniently cached value */
        *(RwFogType*)param = _RwD3D9StateCache.fogType;

        return(TRUE);
    }

    case rwRENDERSTATEFOGCOLOR:
    {
        /* Return our conveniently cached value */
        *(RwInt32*)param = _RwD3D9StateCache.fogColor;

        return(TRUE);
    }

    case rwRENDERSTATEFOGDENSITY:
    {
        /* Return our conveniently cached value */
        *(RwReal*)param = _RwD3D9StateCache.fogDensity;

        return(TRUE);
    }

    /************************ TEXTURE ADDRESSING ************************/

    case rwRENDERSTATETEXTUREADDRESS:
    {
        if (_RwD3D9StateCache.stage[0].texAddressU == _RwD3D9StateCache.stage[0].texAddressV)
        {
            /* Return our conveniently cached value */
            *(RwTextureAddressMode*)param = _RwD3D9StateCache.stage[0].texAddressU;

            return(TRUE);
        }

        /* Return and invalid value */
        *(RwTextureAddressMode*)param = rwTEXTUREADDRESSNATEXTUREADDRESS;

        return(FALSE);
    }

    case rwRENDERSTATETEXTUREADDRESSU:
    {
        /* Return our conveniently cached value */
        *(RwTextureAddressMode*)param = _RwD3D9StateCache.stage[0].texAddressU;

        return(TRUE);
    }

    case rwRENDERSTATETEXTUREADDRESSV:
    {
        /* Return our conveniently cached value */
        *(RwTextureAddressMode*)param = _RwD3D9StateCache.stage[0].texAddressV;

        return(TRUE);
    }

    /*********************** TEXTURE FILTER MODE ************************/

    case rwRENDERSTATETEXTUREFILTER:
    {
        /* Return our conveniently cached value */
        *(RwTextureFilterMode*)param = _RwD3D9StateCache.stage[0].filterMode;

        return(TRUE);
    }

    /************************** TEXTURE RASTER **************************/

    case rwRENDERSTATETEXTURERASTER:
    {
        /* Return our conveniently cached value */
        *(RwRaster**)param = _RwD3D9StateCache.stage[0].curTexRaster;

        return(TRUE);
    }

    /************************** Z BUFFER WRITE **************************/

    case rwRENDERSTATEZWRITEENABLE:
    {
        /* Return our conveniently cached value */
        *(RwBool*)param = _RwD3D9StateCache.zWriteEnable;

        return(TRUE);
    }

    /************************** Z BUFFER TEST ***************************/

    case rwRENDERSTATEZTESTENABLE:
    {
        /* Return our conveniently cached value */
        *(RwBool*)param = _RwD3D9StateCache.zTestEnable;

        return(TRUE);
    }

    /************************** BLENDING MODES **************************/

    case rwRENDERSTATESRCBLEND:
    {
        /* Return our conveniently cached value */
        *(RwBlendFunction*)param = _RwD3D9StateCache.srcBlend;

        return(TRUE);
    }

    case rwRENDERSTATEDESTBLEND:
    {
        /* Return our conveniently cached value */
        *(RwBlendFunction*)param = _RwD3D9StateCache.dstBlend;

        return(TRUE);
    }

    /*************************** SHADING MODE ***************************/

    case rwRENDERSTATESHADEMODE:
    {
        /* Return our conveniently cached value */
        *(RwShadeMode*)param = _RwD3D9StateCache.shadeMode;

        return(TRUE);
    }

    /*************************** VERTEX ALPHA ***************************/

    case rwRENDERSTATEVERTEXALPHAENABLE:
    {
        /* Return our conveniently cached value */
        *(RwBool*)param = _RwD3D9StateCache.vertexAlphaBlendEnable;

        return(TRUE);
    }

    /************************** BORDER COLOR ****************************/

    case rwRENDERSTATEBORDERCOLOR:
    {
        /* Return our conveniently cached value */
        *(RwInt32*)param = _RwD3D9StateCache.stage[0].borderColor;

        return(TRUE);
    }

    /***************** PERSPECTIVELY CORRECT TEXTUREING *****************/

    case rwRENDERSTATETEXTUREPERSPECTIVE:
    {
        /* Return our conveniently cached value */

        return(FALSE);
    }

    /*************************** CULLING MODE ***************************/

    case rwRENDERSTATECULLMODE:
    {
        /* Return our conveniently cached value */
        *(RwCullMode*)param = _RwD3D9StateCache.cullMode;

        return(TRUE);
    }

    /************************** STENCIL BUFFER **************************/

    case rwRENDERSTATESTENCILENABLE:
    {
        /* Return our conveniently cached value */
        *(RwBool*)param = _RwD3D9StateCache.stencilEnable;

        return(TRUE);
    }

    case rwRENDERSTATESTENCILFAIL:
    {
        /* Return our conveniently cached value */
        *(RwStencilOperation*)param = _RwD3D9StateCache.stencilFailOp;

        return(TRUE);
    }

    case rwRENDERSTATESTENCILZFAIL:
    {
        /* Return our conveniently cached value */
        *(RwStencilOperation*)param = _RwD3D9StateCache.stencilZFailOp;

        return(TRUE);
    }

    case rwRENDERSTATESTENCILPASS:
    {
        /* Return our conveniently cached value */
        *(RwStencilOperation*)param = _RwD3D9StateCache.stencilPassOp;

        return(TRUE);
    }

    case rwRENDERSTATESTENCILFUNCTION:
    {
        /* Return our conveniently cached value */
        *(RwStencilFunction*)param = _RwD3D9StateCache.stencilFunction;

        return(TRUE);
    }

    case rwRENDERSTATESTENCILFUNCTIONREF:
    {
        /* Return our conveniently cached value */
        *(RwInt32*)param = _RwD3D9StateCache.stencilRef;

        return(TRUE);
    }

    case rwRENDERSTATESTENCILFUNCTIONMASK:
    {
        /* Return our conveniently cached value */
        *(RwInt32*)param = _RwD3D9StateCache.stencilMask;

        return(TRUE);
    }

    case rwRENDERSTATESTENCILFUNCTIONWRITEMASK:
    {
        /* Return our conveniently cached value */
        *(RwInt32*)param = _RwD3D9StateCache.stencilWriteMask;

        return(TRUE);
    }

    case rwRENDERSTATEALPHATESTFUNCTION:
    {
        /* Return our conveniently cached value */
        *(RwAlphaTestFunction*)param = _RwD3D9StateCache.alphaTestFunction;

        return(TRUE);
    }

    case rwRENDERSTATEALPHATESTFUNCTIONREF:
    {
        /* Return our conveniently cached value */
        *(RwUInt32*)param = RenderStateCache[D3DRS_ALPHAREF].value;

        return(TRUE);
    }

    /************************** NOT SUPPORTED ***************************/

    default:
    {
        break;
    }
    }

    return(FALSE);
}

void RenderStates::D3D9RenderStateCacheClear()
{
    RwUInt32 state;

    memset(RenderStateD3D, 0xffffffff, 210 * sizeof(RwUInt32));

    /* Change special values */
    RenderStateD3D[D3DRS_FOGCOLOR] = 0;
    RenderStateD3D[D3DRS_AMBIENT] = 0;
    RenderStateD3D[D3DRS_STENCILMASK] = 0;
    RenderStateD3D[D3DRS_STENCILWRITEMASK] = 0;

    /* Initialize the cache */
    for (state = 0; state < 210; state++)
    {
        RenderStateCache[state].value = RenderStateD3D[state];
        RenderStateCache[state].changed = FALSE;
    }

    NumRenderStateChanges = 0;  
}

void RenderStates::D3D9TextureStageStateCacheClear()
{
    RwUInt32 stage, type;

    memset(TextureStageStateD3D, 0xffffffff, RWD3D9_MAX_TEXTURE_STAGES * 33 * sizeof(RwUInt32));

    /* Initialize the cache */
    for (stage = 0; stage < RWD3D9_MAX_TEXTURE_STAGES; stage++)
    {
        for (type = 0; type < 33; type++)
        {
            TextureStageStateCache[stage][type].value = 0xffffffff;
            TextureStageStateCache[stage][type].changed = FALSE;
        }
    }

    NumTextureStageStateChanges = 0;  
}

void RenderStates::D3D9SamplerStageStateCacheClear()
{
    memset(SamplerStageStateD3D, 0xffffffff, RWD3D9_MAX_TEXTURE_STAGES * 14 * sizeof(RwUInt32));
}

void RenderStates::_rwD3D9RenderStateOpen()
{
    RwInt32 i, dither;

    D3D9RenderStateCacheClear();
    D3D9TextureStageStateCacheClear();
    D3D9SamplerStageStateCacheClear();
    //D3D9ClearCacheMaterials();

    /* Fog */
    _RwD3D9StateCache.fogEnable = FALSE;
    _RwD3D9StateCache.fogType = rwFOGTYPELINEAR;
    _RwD3D9StateCache.fogDensity = 1.0f;
    _RwD3D9StateCache.fogColor = 0;

    SetRenderState(D3DRS_FOGENABLE, FALSE);

    if ((_RwD3D9DeviceCaps.RasterCaps & D3DPRASTERCAPS_FOGTABLE) != 0 &&
        (_RwD3D9DeviceCaps.RasterCaps & D3DPRASTERCAPS_WFOG) != 0)
    {
        SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
        SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
    }
    else
    {
        SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
        SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
    }

    SetRenderState(D3DRS_FOGCOLOR, 0);

    /* Shading mode */
    SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    _RwD3D9StateCache.shadeMode = rwSHADEMODEGOURAUD;

    /* Set Z Compare mode */
    SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    SetRenderState(D3DRS_ZENABLE, TRUE);

    _RwD3D9StateCache.zWriteEnable = TRUE;
    _RwD3D9StateCache.zTestEnable = TRUE;

    /* Stencil */
    SetRenderState(D3DRS_STENCILENABLE, FALSE);
    SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
    SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
    SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
    SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
    SetRenderState(D3DRS_STENCILREF, 0);
    SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
    SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);

    _RwD3D9StateCache.stencilEnable = FALSE;
    _RwD3D9StateCache.stencilFailOp = rwSTENCILOPERATIONKEEP;
    _RwD3D9StateCache.stencilZFailOp = rwSTENCILOPERATIONKEEP;
    _RwD3D9StateCache.stencilPassOp = rwSTENCILOPERATIONKEEP;
    _RwD3D9StateCache.stencilFunction = rwSTENCILFUNCTIONALWAYS;
    _RwD3D9StateCache.stencilRef = 0;
    _RwD3D9StateCache.stencilMask = 0xFFFFFFFF;
    _RwD3D9StateCache.stencilWriteMask = 0xFFFFFFFF;

    /* Texture stages */
    for (i = 0; i < RWD3D9_MAX_TEXTURE_STAGES; i++)
    {
        /* Texture */
         _RwD3DDevice->SetTexture(i, NULL);
        _RwD3D9StateCache.stage[i].curTexRaster = NULL;

        /* Texture filter mode - Liner/Trilinear */
        if (_RwD3D9DeviceCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)
        {
            SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            _RwD3D9StateCache.stage[i].filterMode = rwFILTERLINEAR;
        }
        else
        {
            SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_POINT);
            SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
            _RwD3D9StateCache.stage[i].filterMode = rwFILTERNEAREST;
        }

        SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

        /* Texture addressing mode */
        SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

        _RwD3D9StateCache.stage[i].texAddressU = rwTEXTUREADDRESSWRAP;
        _RwD3D9StateCache.stage[i].texAddressV = rwTEXTUREADDRESSWRAP;

        /* Border color */
        SetSamplerState(i, D3DSAMP_BORDERCOLOR, 0xFF000000);
        _RwD3D9StateCache.stage[i].borderColor = 0xFF000000;

        /* Max Anisotropy Level: 1 to disable it */
        _RwD3D9StateCache.stage[i].maxAnisotropy = 1;
        SetSamplerState(i, D3DSAMP_MAXANISOTROPY, 1);

        /* Disable stage if not the first one */
        if (i)
        {
            SetTextureStageState(i, D3DTSS_COLOROP, D3DTOP_DISABLE);
            SetTextureStageState(i, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
        }
    }

    /* Set default blending modes */
    SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

    SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    _RwD3D9StateCache.srcBlend = rwBLENDSRCALPHA;
    _RwD3D9StateCache.dstBlend = rwBLENDINVSRCALPHA;

    /* Set up the alpha compare function for future use */
    SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    _RwD3D9StateCache.alphaTestFunction = rwALPHATESTFUNCTIONGREATER;
    _RwD3D9StateCache.alphaTestEnable = TRUE;

    SetRenderState(D3DRS_ALPHAREF, 0x00000000);

    /* Disable alpha blending */
    SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    _RwD3D9StateCache.vertexAlphaBlendEnable = FALSE;
    _RwD3D9StateCache.textureAlphaBlendEnable = FALSE;

    /* Turn lighting off */
    SetRenderState(D3DRS_LIGHTING, FALSE);

    /* Cull clockwise facing triangles */
    SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    _RwD3D9StateCache.cullMode = rwCULLMODECULLBACK;

    /* Vertex color only until a texture has been set */
    SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

    /* Diffuse, Specular & Ambient colors come from the material */
    SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);

    SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);

    SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);

    /* Use dithering if possible */
    dither = (_RwD3D9DeviceCaps.RasterCaps & D3DPRASTERCAPS_DITHER) ? TRUE : FALSE;
    SetRenderState(D3DRS_DITHERENABLE, dither);

    SetRenderState(D3DRS_SPECULARENABLE, FALSE);

    SetRenderState(D3DRS_LOCALVIEWER, FALSE);

    SetRenderState(D3DRS_AMBIENT, 0xffffffff);

    SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);

    RenderStateFlushCache();
}