#include "RenderStates.h"
#include "RWCommon.h"

_rwD3D9StateCache _RwD3D9StateCache/* = *(_rwD3D9StateCache*)0xC9A4C0*/;

RwInt32 D3D9AnisotTextureOffset =0;
#define CONSTTEXTUREGETANISOT(tex) \
    (*((const RwUInt8*)(((const RwUInt8*)tex) + D3D9AnisotTextureOffset)))

typedef struct
{
    uint32_t value;
    bool changed;
} RxStateCache;
//RxStateCache(&RenderStateCache)[210] = *(RxStateCache(*)[210])0x00C991D0;
//uint32_t(&RenderStateD3D)[210] = *(uint32_t(*)[210])0x00C98E88;
//uint32_t& NumRenderStateChanges = *(uint32_t*)0xC9A5EC;
//D3DRENDERSTATETYPE(&RenderStateChanges)[210] = *(D3DRENDERSTATETYPE(*)[210])0x00C98B40;
//RxStateCache(&TextureStageStateCache)[RWD3D9_MAX_TEXTURE_STAGES][33] = *(RxStateCache(*)[RWD3D9_MAX_TEXTURE_STAGES][33])0x00C980F8;
//uint32_t(&TextureStageStateD3D)[RWD3D9_MAX_TEXTURE_STAGES][33] = *(uint32_t(*)[RWD3D9_MAX_TEXTURE_STAGES][33])0x00C99860;
//uint32_t& NumTextureStageStateChanges = *(uint32_t*)0xC9A5F0;
//RxTextureStageChangeType(&TextureStageStateChanges)[RWD3D9_MAX_TEXTURE_STAGES * 33] = *(RxTextureStageChangeType(*)[RWD3D9_MAX_TEXTURE_STAGES * 33])0x00C99C80;
//uint32_t(&SamplerStageStateD3D)[RWD3D9_MAX_TEXTURE_STAGES][14] = *(uint32_t(*)[RWD3D9_MAX_TEXTURE_STAGES][14])0x00C98938;

RxStateCache RenderStateCache[210];
uint32_t RenderStateD3D[210];

uint32_t NumRenderStateChanges = 0;
D3DRENDERSTATETYPE RenderStateChanges[210];

RxStateCache TextureStageStateCache[RWD3D9_MAX_TEXTURE_STAGES][33];
uint32_t TextureStageStateD3D[RWD3D9_MAX_TEXTURE_STAGES][33];
uint32_t SamplerStageStateD3D[RWD3D9_MAX_TEXTURE_STAGES][14];
uint32_t NumTextureStageStateChanges = 0;
RxTextureStageChangeType TextureStageStateChanges[RWD3D9_MAX_TEXTURE_STAGES * 33];

D3DMATERIAL9 LastMaterial;

RwUInt32 LastMaterialFlags = 0;
RwUInt32 LastMaterialColor = 0;
RwReal LastMaterialDiffuse = 0.0f;
RwReal LastMaterialAmbient = 0.0f;
RwRGBAReal LastMaterialAmbientSaturated;
RwRGBAReal& AmbientSaturated = *(RwRGBAReal*)0x8E2418;

void RenderStates::RenderStateFlushCache()
{
    if (NumRenderStateChanges)
    {
        uint n = 0;
        do
        {
            auto stateChanged = RenderStateChanges[n];
            auto newValue = RenderStateCache[stateChanged].value;

            RenderStateCache[stateChanged].changed = false;

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
        uint n = 0;
        do
        {
            auto stage = TextureStageStateChanges[n].stage;
            auto type = TextureStageStateChanges[n].type;
            auto newValue = TextureStageStateCache[stage][type].value;

            TextureStageStateCache[stage][type].changed = false;

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
            RenderStateCache[state].changed = true;
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
                SetRenderState(D3DRS_FOGENABLE, true);
                _RwD3D9StateCache.fogEnable = true;
            }
        }
    }
    else
    {
        if (_RwD3D9StateCache.fogEnable)
        {
            SetRenderState(D3DRS_FOGENABLE, false);
            _RwD3D9StateCache.fogEnable = false;
        }
    }

    return true;
}

RwBool RenderStates::rwD3D9RenderStateFogColor(RwUInt32 fogColor)
{
    if (fogColor != _RwD3D9StateCache.fogColor)
    {
        SetRenderState(D3DRS_FOGCOLOR, fogColor);
        _RwD3D9StateCache.fogColor = fogColor;
    }

    return true;
}

RwBool RenderStates::rwD3D9RenderStateFogType(RwFogType fogType)
{
    if (fogType != _RwD3D9StateCache.fogType)
    {
        if ((_RwD3D9DeviceCaps.RasterCaps & D3DPRASTERCAPS_FOGTABLE) != 0 &&
            (_RwD3D9DeviceCaps.RasterCaps & D3DPRASTERCAPS_WFOG) != 0)
        {
            SetRenderState(D3DRS_FOGTABLEMODE, _RwD3D9FogConvTable[fogType]);

            if (RenderStateCache[D3DRS_FOGVERTEXMODE].value != D3DFOG_NONE)
                SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
        }
        else
        {
            if (RenderStateCache[D3DRS_FOGTABLEMODE].value != D3DFOG_NONE)
                SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);

            SetRenderState(D3DRS_FOGVERTEXMODE, _RwD3D9FogConvTable[fogType]);
        }

        _RwD3D9StateCache.fogType = fogType;
    }

   return true;
}

RwBool RenderStates::rwD3D9RenderStateFogDensity(RwReal fogDensity)
{
    if (fogDensity != _RwD3D9StateCache.fogDensity)
    {
        SetRenderState(D3DRS_FOGDENSITY, FLOATASDWORD(fogDensity));
        _RwD3D9StateCache.fogDensity = fogDensity;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RenderStateTextureAddress(RwTextureAddressMode addressMode)
{

    if (addressMode != _RwD3D9StateCache.stage[0].texAddressU)
    {
        _RwD3D9StateCache.stage[0].texAddressU = addressMode;
        SetSamplerState(0, D3DSAMP_ADDRESSU, _RwD3D9AddressConvTable[addressMode]);
    }

    if (addressMode != _RwD3D9StateCache.stage[0].texAddressV)
    {
        _RwD3D9StateCache.stage[0].texAddressV = addressMode;
        SetSamplerState(0, D3DSAMP_ADDRESSV, _RwD3D9AddressConvTable[addressMode]);
    }

   return true;
}

RwBool  RenderStates::_rwD3D9RenderStateTextureAddressU(RwTextureAddressMode addressMode)
{
    if (addressMode != _RwD3D9StateCache.stage[0].texAddressU)
    {
        _RwD3D9StateCache.stage[0].texAddressU = addressMode;
        SetSamplerState(0, D3DSAMP_ADDRESSU, _RwD3D9AddressConvTable[addressMode]);
    }

   return true;
}

RwBool RenderStates::_rwD3D9RenderStateTextureAddressV(RwTextureAddressMode addressMode)
{
    if (addressMode != _RwD3D9StateCache.stage[0].texAddressV)
    {
        _RwD3D9StateCache.stage[0].texAddressV = addressMode;
        SetSamplerState(0, D3DSAMP_ADDRESSV, _RwD3D9AddressConvTable[addressMode]);
    }

   return true;
}

RwBool  RenderStates::_rwD3D9RenderStateTextureFilter(RwTextureFilterMode filterMode)
{
    if (_RwD3D9StateCache.stage[0].maxAnisotropy > 1)
    {
        _RwD3D9StateCache.stage[0].maxAnisotropy = 1;
        SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 1);
    }

    if (filterMode != _RwD3D9StateCache.stage[0].filterMode)
    {
        auto minmag = _RwD3D9FilterModeConvTable[filterMode].minmag;
        auto mip = _RwD3D9FilterModeConvTable[filterMode].mip;

        _RwD3D9StateCache.stage[0].filterMode = filterMode;

        SetSamplerState(0, D3DSAMP_MINFILTER, minmag);
        SetSamplerState(0, D3DSAMP_MAGFILTER, minmag);

        if (SamplerStageStateD3D[0][D3DSAMP_MIPFILTER] != mip)
            SetSamplerState(0, D3DSAMP_MIPFILTER, mip);
    }

    return true;
}

RwBool RenderStates::_rwD3D9RenderStateSrcBlend(RwBlendFunction srcBlend)
{
    if (srcBlend != _RwD3D9StateCache.srcBlend)
    {
        SetRenderState(D3DRS_SRCBLEND, _RwD3D9BlendConvTable[srcBlend]);
        _RwD3D9StateCache.srcBlend = srcBlend;
    }

   return true;
}

RwBool  RenderStates::_rwD3D9RenderStateDestBlend(RwBlendFunction dstBlend)
{
    if (dstBlend != _RwD3D9StateCache.dstBlend)
    {
        SetRenderState(D3DRS_DESTBLEND, _RwD3D9BlendConvTable[dstBlend]);
        _RwD3D9StateCache.dstBlend = dstBlend;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RWSetRasterStage(RwRaster* raster, RwUInt32 stage)
{
    _rwD3D9RasterExt* rasExt;

    if (raster)
        rasExt = RASTEREXTFROMRASTER(raster);
    else
        rasExt = nullptr;

    if (stage == 0)
    {
        if (raster && rasExt->alpha)
        {
            if (!_RwD3D9StateCache.textureAlphaBlendEnable)
            {
                _RwD3D9StateCache.textureAlphaBlendEnable = true;

                if (!_RwD3D9StateCache.vertexAlphaBlendEnable)
                {
                    SetRenderState(D3DRS_ALPHABLENDENABLE, true);
                    SetRenderState(D3DRS_ALPHATESTENABLE, _RwD3D9StateCache.alphaTestEnable);
                }
            }
        }
        else
        {
            if (_RwD3D9StateCache.textureAlphaBlendEnable)
            {
                _RwD3D9StateCache.textureAlphaBlendEnable = false;

                if (!_RwD3D9StateCache.vertexAlphaBlendEnable)
                {
                    SetRenderState(D3DRS_ALPHABLENDENABLE, false);
                    SetRenderState(D3DRS_ALPHATESTENABLE, false);
                }
            }
        }
    }

    if (raster != _RwD3D9StateCache.stage[stage].curTexRaster)
    {
        _RwD3D9StateCache.stage[stage].curTexRaster = raster;

        if (raster == nullptr)
        {
            _RwD3DDevice->SetTexture(stage, nullptr);
            return true;
        }

        _RwD3DDevice->SetTexture(stage, (LPDIRECT3DBASETEXTURE9)(rasExt->texture));

        if (rasExt->palette)
            _RwD3DDevice->SetCurrentTexturePalette(rasExt->palette->globalindex);

    }

   return true;
}

RwRaster* RenderStates::_rwD3D9RWGetRasterStage(RwUInt32 stage)
{
    return _RwD3D9StateCache.stage[stage].curTexRaster;
}

void RenderStates::_rwD3D9SetTextureAnisotropyOffset(RwInt32 anisotTextureOffset)
{
    D3D9AnisotTextureOffset = anisotTextureOffset;
}

RwBool RenderStates::RwD3D9SetTexture(RwTexture* texture, RwUInt32 stage)
{
    if (texture)
    {
        RwTextureAddressMode addressingU = RwTextureGetAddressingU(texture);
        if (addressingU != _RwD3D9StateCache.stage[stage].texAddressU)
        {
            _RwD3D9StateCache.stage[stage].texAddressU = addressingU;
            SetSamplerState(stage, D3DSAMP_ADDRESSU, _RwD3D9AddressConvTable[addressingU]);
        }

        RwTextureAddressMode addressingV = RwTextureGetAddressingV(texture);
        if (addressingV != _RwD3D9StateCache.stage[stage].texAddressV)
        {
            _RwD3D9StateCache.stage[stage].texAddressV = addressingV;
            SetSamplerState(stage, D3DSAMP_ADDRESSV, _RwD3D9AddressConvTable[addressingV]);
        }

        RwTextureFilterMode filtering = RwTextureGetFilterMode(texture);

        if (D3D9AnisotTextureOffset > 0)
        {
            auto anisotropy = CONSTTEXTUREGETANISOT(texture);

            if (_RwD3D9StateCache.stage[stage].maxAnisotropy != anisotropy)
            {
                _RwD3D9StateCache.stage[stage].maxAnisotropy = anisotropy;
                SetSamplerState(stage, D3DSAMP_MAXANISOTROPY, anisotropy);
            }

            if (anisotropy > 1)
                filtering = (RwTextureFilterMode)NUMFILTERMODES;
        }

        if (_RwD3D9StateCache.stage[stage].filterMode != filtering)
        {
            D3DTEXTUREFILTERTYPE    minmag, mip;

            _RwD3D9StateCache.stage[stage].filterMode = filtering;

            minmag = _RwD3D9FilterModeConvTable[filtering].minmag;
            mip = _RwD3D9FilterModeConvTable[filtering].mip;

            if (minmag == D3DTEXF_ANISOTROPIC)
            {
                SetSamplerState(stage, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
                SetSamplerState(stage, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);      
            }
            else
            {
                SetSamplerState(stage, D3DSAMP_MINFILTER, minmag);
                SetSamplerState(stage, D3DSAMP_MAGFILTER, minmag);
            }

            SetSamplerState(stage, D3DSAMP_MIPFILTER, mip);
        }
        _rwD3D9RWSetRasterStage(texture->raster, stage);

    }
    else
    {
        if (stage == 0)
        {
            if (_RwD3D9StateCache.textureAlphaBlendEnable)
            {
                _RwD3D9StateCache.textureAlphaBlendEnable = false;

                if (!_RwD3D9StateCache.vertexAlphaBlendEnable)
                {
                    SetRenderState(D3DRS_ALPHABLENDENABLE, false);
                    SetRenderState(D3DRS_ALPHATESTENABLE, false);
                }
            }
        }

        if (_RwD3D9StateCache.stage[stage].curTexRaster != nullptr)
        {
            _RwD3D9StateCache.stage[stage].curTexRaster = nullptr;
             _RwD3DDevice->SetTexture(stage, nullptr);
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
                SetRenderState(D3DRS_ZENABLE, true);

            SetRenderState(D3DRS_ZWRITEENABLE, true);
            _RwD3D9StateCache.zWriteEnable = true;
        }
    }
    else
    {
        if (_RwD3D9StateCache.zWriteEnable)
        {
            if (!_RwD3D9StateCache.zTestEnable)
                SetRenderState(D3DRS_ZENABLE, false);

            SetRenderState(D3DRS_ZWRITEENABLE, false);
            _RwD3D9StateCache.zWriteEnable = false;
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
                SetRenderState(D3DRS_ZENABLE, true);

            SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
            _RwD3D9StateCache.zTestEnable = true;
        }
    }
    else
    {
        if (_RwD3D9StateCache.zTestEnable)
        {
            if (!_RwD3D9StateCache.zWriteEnable)
                SetRenderState(D3DRS_ZENABLE, false);
            
            SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
            _RwD3D9StateCache.zTestEnable = false;
        }
    }

   return true;
}

RwBool RenderStates::rwD3D9RenderStateShadeMode(RwShadeMode shadeMode)
{
    if (shadeMode != _RwD3D9StateCache.shadeMode)
    {
        SetRenderState(D3DRS_SHADEMODE, _RwD3D9ShadeModeConvTable[shadeMode]);
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
            _RwD3D9StateCache.vertexAlphaBlendEnable = true;

            if (!_RwD3D9StateCache.textureAlphaBlendEnable)
            {
                SetRenderState(D3DRS_ALPHABLENDENABLE, true);
                SetRenderState(D3DRS_ALPHATESTENABLE, _RwD3D9StateCache.alphaTestEnable);
            }
        }
    }
    else
    {
        if (_RwD3D9StateCache.vertexAlphaBlendEnable)
        {
            _RwD3D9StateCache.vertexAlphaBlendEnable = false;

            if (!_RwD3D9StateCache.textureAlphaBlendEnable)
            {
                SetRenderState(D3DRS_ALPHABLENDENABLE, false);
                SetRenderState(D3DRS_ALPHATESTENABLE, false);
            }
        }
    }

   return true;
}

RwBool  RenderStates::_rwD3D9RenderStateIsVertexAlphaEnable()
{
    return _RwD3D9StateCache.vertexAlphaBlendEnable;
}

RwBool RenderStates::rwD3D9RenderStateBorderColor(RwUInt32 borderColor)
{
    if (borderColor != _RwD3D9StateCache.stage[0].borderColor)
    {
        _RwD3D9StateCache.stage[0].borderColor = borderColor;
        SetSamplerState(0, D3DSAMP_BORDERCOLOR, borderColor);
    }

   return true;
}


RwBool rwD3D9RenderStateTexturePerspective(RwBool persp)
{
    if (persp)
    {
       return true;
    }

   return false;
}

RwBool RenderStates::rwD3D9RenderStateCullMode(RwCullMode cullMode)
{
    if (cullMode != _RwD3D9StateCache.cullMode)
    {
        SetRenderState(D3DRS_CULLMODE, _RwD3D9CullModeConvTable[cullMode]);
        _RwD3D9StateCache.cullMode = cullMode;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RenderStateStencilEnable(RwBool enable)
{

    if (enable != _RwD3D9StateCache.stencilEnable)
    {
        SetRenderState(D3DRS_STENCILENABLE, enable);
        _RwD3D9StateCache.stencilEnable = enable;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RenderStateStencilFail(RwStencilOperation operation)
{
    if (operation != _RwD3D9StateCache.stencilFailOp)
    {
        SetRenderState(D3DRS_STENCILFAIL, _RwD3D9StencilOpConvTable[operation]);
        _RwD3D9StateCache.stencilFailOp = operation;
    }

   return true;
}

RwBool  RenderStates::_rwD3D9RenderStateStencilZFail(RwStencilOperation operation)
{
    if (operation != _RwD3D9StateCache.stencilZFailOp)
    {
        SetRenderState(D3DRS_STENCILZFAIL, _RwD3D9StencilOpConvTable[operation]);
        _RwD3D9StateCache.stencilZFailOp = operation;
    }

   return true;
}

RwBool  RenderStates::_rwD3D9RenderStateStencilPass(RwStencilOperation operation)
{
    if (operation != _RwD3D9StateCache.stencilPassOp)
    {
        SetRenderState(D3DRS_STENCILPASS, _RwD3D9StencilOpConvTable[operation]);
        _RwD3D9StateCache.stencilPassOp = operation;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RenderStateStencilFunction(RwStencilFunction function)
{
    if (function != _RwD3D9StateCache.stencilFunction)
    {
        SetRenderState(D3DRS_STENCILFUNC, _RwD3D9CmpFuncConvTable[function]);
        _RwD3D9StateCache.stencilFunction = function;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RenderStateStencilFunctionRef(RwInt32 ref)
{
    if (ref != _RwD3D9StateCache.stencilRef)
    {
        SetRenderState(D3DRS_STENCILREF, ref);
        _RwD3D9StateCache.stencilRef = ref;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RenderStateStencilFunctionMask(RwInt32 mask)
{
    if (mask != _RwD3D9StateCache.stencilMask)
    {
        SetRenderState(D3DRS_STENCILMASK, mask);
        _RwD3D9StateCache.stencilMask = mask;
    }

   return true;
}

RwBool  RenderStates::_rwD3D9RenderStateStencilFunctionWriteMask(RwInt32 writeMask)
{
    if (writeMask != _RwD3D9StateCache.stencilWriteMask)
    {
        SetRenderState(D3DRS_STENCILWRITEMASK, writeMask);
        _RwD3D9StateCache.stencilWriteMask = writeMask;
    }

   return true;
}

RwBool RenderStates::_rwD3D9RWSetRenderState(RwRenderState state, void* param)
{
    RwBool  result = false;
    D3D9UnionParam uparam;

    uparam.VoidPtr = param;

    switch (state)
    {
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
    case rwRENDERSTATETEXTUREADDRESS:
        result = _rwD3D9RenderStateTextureAddress(uparam.TextureAddressMode);
        break;
    case rwRENDERSTATETEXTUREADDRESSU:
        result = _rwD3D9RenderStateTextureAddressU(uparam.TextureAddressMode);
        break;
    case rwRENDERSTATETEXTUREADDRESSV:
        result = _rwD3D9RenderStateTextureAddressV(uparam.TextureAddressMode);
        break;
    case rwRENDERSTATETEXTUREFILTER:
        result = _rwD3D9RenderStateTextureFilter(uparam.TextureFilterMode);
        break;
    case rwRENDERSTATETEXTURERASTER:
        result = _rwD3D9RWSetRasterStage(uparam.RasterPtr, 0);
        break;
    case rwRENDERSTATEZWRITEENABLE:
        result = rwD3D9RenderStateZWriteEnable(uparam.Bool);
        break;
    case rwRENDERSTATEZTESTENABLE:
        result = rwD3D9RenderStateZTestEnable(uparam.Bool);
        break;
    case rwRENDERSTATESRCBLEND:
        result = _rwD3D9RenderStateSrcBlend(uparam.BlendFunction);
        break;
    case rwRENDERSTATEDESTBLEND:
        result = _rwD3D9RenderStateDestBlend(uparam.BlendFunction);
        break;
    case rwRENDERSTATESHADEMODE:
        result = rwD3D9RenderStateShadeMode(uparam.ShadeMode);
        break;
    case rwRENDERSTATEVERTEXALPHAENABLE:
        result = _rwD3D9RenderStateVertexAlphaEnable(uparam.Bool);
        break;
    case rwRENDERSTATEBORDERCOLOR:
        result = rwD3D9RenderStateBorderColor(uparam.UInt32);
        break;
    case rwRENDERSTATETEXTUREPERSPECTIVE:
        result = rwD3D9RenderStateTexturePerspective(uparam.Bool);
        break;
    case rwRENDERSTATECULLMODE:
        result = rwD3D9RenderStateCullMode(uparam.CullMode);
        break;
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
                SetRenderState(D3DRS_ALPHATESTENABLE, _RwD3D9StateCache.alphaTestEnable);
            }

            SetRenderState(D3DRS_ALPHAFUNC, _RwD3D9CmpFuncConvTable[uparam.AlphaTestFunction]);
        }
        result = true;
        break;
    case rwRENDERSTATEALPHATESTFUNCTIONREF:
        SetRenderState(D3DRS_ALPHAREF, uparam.UInt32);
        result = true;
        break;
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
    case rwRENDERSTATEFOGENABLE:
    {
        *(RwBool*)param = _RwD3D9StateCache.fogEnable;
        return true;
    }
    case rwRENDERSTATEFOGTYPE:
    {
        *(RwFogType*)param = _RwD3D9StateCache.fogType;
        return true;
    }
    case rwRENDERSTATEFOGCOLOR:
    {
        *(RwInt32*)param = _RwD3D9StateCache.fogColor;
        return true;
    }
    case rwRENDERSTATEFOGDENSITY:
    {
        *(RwReal*)param = _RwD3D9StateCache.fogDensity;
        return true;
    }
    case rwRENDERSTATETEXTUREADDRESS:
    {
        if (_RwD3D9StateCache.stage[0].texAddressU == _RwD3D9StateCache.stage[0].texAddressV)
        {
            *(RwTextureAddressMode*)param = _RwD3D9StateCache.stage[0].texAddressU;
            return true;
        }

        *(RwTextureAddressMode*)param = rwTEXTUREADDRESSNATEXTUREADDRESS;
        return false;
    }
    case rwRENDERSTATETEXTUREADDRESSU:
    {
        *(RwTextureAddressMode*)param = _RwD3D9StateCache.stage[0].texAddressU;
        return true;
    }
    case rwRENDERSTATETEXTUREADDRESSV:
    {
        *(RwTextureAddressMode*)param = _RwD3D9StateCache.stage[0].texAddressV;
        return true;
    }
    case rwRENDERSTATETEXTUREFILTER:
    {
        *(RwTextureFilterMode*)param = _RwD3D9StateCache.stage[0].filterMode;
        return true;
    }
    case rwRENDERSTATETEXTURERASTER:
    {
        *(RwRaster**)param = _RwD3D9StateCache.stage[0].curTexRaster;
        return true;
    }
    case rwRENDERSTATEZWRITEENABLE:
    {
        *(RwBool*)param = _RwD3D9StateCache.zWriteEnable;
        return true;
    }
    case rwRENDERSTATEZTESTENABLE:
    {
        *(RwBool*)param = _RwD3D9StateCache.zTestEnable;
        return true;
    }
    case rwRENDERSTATESRCBLEND:
    {
        *(RwBlendFunction*)param = _RwD3D9StateCache.srcBlend;
        return true;
    }
    case rwRENDERSTATEDESTBLEND:
    {
        *(RwBlendFunction*)param = _RwD3D9StateCache.dstBlend;
        return true;
    }
    case rwRENDERSTATESHADEMODE:
    {
        *(RwShadeMode*)param = _RwD3D9StateCache.shadeMode;
        return true;
    }
    case rwRENDERSTATEVERTEXALPHAENABLE:
    {
        *(RwBool*)param = _RwD3D9StateCache.vertexAlphaBlendEnable;
        return true;
    }
    case rwRENDERSTATEBORDERCOLOR:
    {
        *(RwInt32*)param = _RwD3D9StateCache.stage[0].borderColor;
        return true;
    }
    case rwRENDERSTATETEXTUREPERSPECTIVE:
    {
        return false;
    }
    case rwRENDERSTATECULLMODE:
    {
        *(RwCullMode*)param = _RwD3D9StateCache.cullMode;
        return true;
    }
    case rwRENDERSTATESTENCILENABLE:
    {
        *(RwBool*)param = _RwD3D9StateCache.stencilEnable;
        return true;
    }
    case rwRENDERSTATESTENCILFAIL:
    {
        *(RwStencilOperation*)param = _RwD3D9StateCache.stencilFailOp;
        return true;
    }
    case rwRENDERSTATESTENCILZFAIL:
    {
        *(RwStencilOperation*)param = _RwD3D9StateCache.stencilZFailOp;
        return true;
    }
    case rwRENDERSTATESTENCILPASS:
    {
        *(RwStencilOperation*)param = _RwD3D9StateCache.stencilPassOp;
        return true;
    }
    case rwRENDERSTATESTENCILFUNCTION:
    {
        *(RwStencilFunction*)param = _RwD3D9StateCache.stencilFunction;
        return true;
    }
    case rwRENDERSTATESTENCILFUNCTIONREF:
    {
        *(RwInt32*)param = _RwD3D9StateCache.stencilRef;
        return true;
    }
    case rwRENDERSTATESTENCILFUNCTIONMASK:
    {
        *(RwInt32*)param = _RwD3D9StateCache.stencilMask;
        return true;
    }
    case rwRENDERSTATESTENCILFUNCTIONWRITEMASK:
    {
        *(RwInt32*)param = _RwD3D9StateCache.stencilWriteMask;
        return true;
    }
    case rwRENDERSTATEALPHATESTFUNCTION:
    {
        *(RwAlphaTestFunction*)param = _RwD3D9StateCache.alphaTestFunction;
        return true;
    }
    case rwRENDERSTATEALPHATESTFUNCTIONREF:
    {
        *(RwUInt32*)param = RenderStateCache[D3DRS_ALPHAREF].value;
        return true;
    }
    }

    return false;
}

RwBool RenderStates::RwD3D9SetMaterial(const void* material)
{
    if (memcmp((const RwUInt32*)&LastMaterial, (const RwUInt32*)material, sizeof(D3DMATERIAL9)) != 0)
    {
        memcpy(&LastMaterial, material, sizeof(D3DMATERIAL9));
        LastMaterialFlags = 0;

        auto hr = IDirect3DDevice9_SetMaterial(_RwD3DDevice, (const D3DMATERIAL9*)material);

        return SUCCEEDED(hr);
    }
    return true;
}

RwBool RenderStates::RwD3D9SetSurfaceProperties(const RwSurfaceProperties* surfaceProps, const RwRGBA* color, RwUInt32 flags)
{
    /* Keep only useful flags */
    flags &= (rxGEOMETRY_MODULATE | rxGEOMETRY_PRELIT);

    if (FLOATASINT(LastMaterialDiffuse) != FLOATASINT(surfaceProps->diffuse) ||
        FLOATASINT(LastMaterialAmbient) != FLOATASINT(surfaceProps->ambient) ||
        LastMaterialColor != *((const RwUInt32*)color) ||
        LastMaterialFlags != flags ||
        FLOATASINT(LastMaterialAmbientSaturated.red) != FLOATASINT(AmbientSaturated.red) ||
        FLOATASINT(LastMaterialAmbientSaturated.green) != FLOATASINT(AmbientSaturated.green) ||
        FLOATASINT(LastMaterialAmbientSaturated.blue) != FLOATASINT(AmbientSaturated.blue))
    {
        HRESULT             hr;

        LastMaterialDiffuse = surfaceProps->diffuse;
        LastMaterialAmbient = surfaceProps->ambient;
        LastMaterialColor = *((const RwUInt32*)color);
        LastMaterialFlags = flags;
        LastMaterialAmbientSaturated.red = AmbientSaturated.red;
        LastMaterialAmbientSaturated.green = AmbientSaturated.green;
        LastMaterialAmbientSaturated.blue = AmbientSaturated.blue;

        if ((flags & rxGEOMETRY_MODULATE) &&
            *((const RwUInt32*)color) != 0xffffffff)
        {
            RwReal coef;

            auto COLORSCALAR = 1.0f / 255.0f;
            coef = surfaceProps->diffuse * COLORSCALAR;

            LastMaterial.Diffuse.r = (color->red) * coef;
            LastMaterial.Diffuse.g = (color->green) * coef;
            LastMaterial.Diffuse.b = (color->blue) * coef;
            LastMaterial.Diffuse.a = (color->alpha) * COLORSCALAR;

            coef = surfaceProps->ambient * COLORSCALAR;

            if (flags & rxGEOMETRY_PRELIT)
            {
                SetRenderState(D3DRS_AMBIENT, ((D3DCOLOR)((((RwUInt32)color->alpha) << 24) | (((RwUInt32)color->red) << 16) | (((RwUInt32)color->green) << 8) | ((RwUInt32)color->blue))));
                SetRenderState(D3DRS_COLORVERTEX, TRUE);
                SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
                SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

                LastMaterial.Ambient.r = 0.0f;
                LastMaterial.Ambient.g = 0.0f;
                LastMaterial.Ambient.b = 0.0f;
                LastMaterial.Ambient.a = 1.0f;

                LastMaterial.Emissive.r = (color->red) * AmbientSaturated.red * coef;
                LastMaterial.Emissive.g = (color->green) * AmbientSaturated.green * coef;
                LastMaterial.Emissive.b = (color->blue) * AmbientSaturated.blue * coef;
                LastMaterial.Emissive.a = 1.0f;
            }
            else
            {
                SetRenderState(D3DRS_AMBIENT, 0xffffffff);
                SetRenderState(D3DRS_COLORVERTEX, FALSE);
                SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
                SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

                LastMaterial.Ambient.r = (color->red) * AmbientSaturated.red * coef;
                LastMaterial.Ambient.g = (color->green) * AmbientSaturated.green * coef;
                LastMaterial.Ambient.b = (color->blue) * AmbientSaturated.blue * coef;
                LastMaterial.Ambient.a = 1.0f;

                LastMaterial.Emissive.r = 0.0f;
                LastMaterial.Emissive.g = 0.0f;
                LastMaterial.Emissive.b = 0.0f;
                LastMaterial.Emissive.a = 1.0f;
            }
        }
        else
        {
            LastMaterial.Diffuse.b = LastMaterial.Diffuse.g =
                LastMaterial.Diffuse.r = surfaceProps->diffuse;

            LastMaterial.Diffuse.a = 1.f;

            SetRenderState(D3DRS_AMBIENT, 0xffffffff);
            SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);

            if (flags & rxGEOMETRY_PRELIT)
            {
                /* Emmisive color from the vertex colors */
                SetRenderState(D3DRS_COLORVERTEX, TRUE);
                SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);
            }
            else
            {
                /* Emmisive color from material, set to black */
                SetRenderState(D3DRS_COLORVERTEX, FALSE);
                SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
            }
            LastMaterial.Emissive.r = 0.0f;
            LastMaterial.Emissive.g = 0.0f;
            LastMaterial.Emissive.b = 0.0f;
            LastMaterial.Emissive.a = 1.0f;

            if (surfaceProps->ambient != 1.f)
            {
                LastMaterial.Ambient.r =
                    surfaceProps->ambient * AmbientSaturated.red;
                LastMaterial.Ambient.g =
                    surfaceProps->ambient * AmbientSaturated.green;
                LastMaterial.Ambient.b =
                    surfaceProps->ambient * AmbientSaturated.blue;
            }
            else
            {
                LastMaterial.Ambient.r = AmbientSaturated.red;
                LastMaterial.Ambient.g = AmbientSaturated.green;
                LastMaterial.Ambient.b = AmbientSaturated.blue;
            }
            LastMaterial.Ambient.a = 1.0f;
        }

        LastMaterial.Specular.r = 0.0f;
        LastMaterial.Specular.g = 0.0f;
        LastMaterial.Specular.b = 0.0f;
        LastMaterial.Specular.a = 1.0f;

        LastMaterial.Power = 0.0f;

        hr = IDirect3DDevice9_SetMaterial(_RwD3DDevice, &LastMaterial);

        return SUCCEEDED(hr);
    }

    return TRUE;
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
        RenderStateCache[state].changed = false;
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
            TextureStageStateCache[stage][type].changed = false;
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
    RwInt32 i;

    D3D9RenderStateCacheClear();
    D3D9TextureStageStateCacheClear();
    D3D9SamplerStageStateCacheClear();
    //D3D9ClearCacheMaterials();

    /* Fog */
    _RwD3D9StateCache.fogEnable = false;
    _RwD3D9StateCache.fogType = rwFOGTYPELINEAR;
    _RwD3D9StateCache.fogDensity = 1.0f;
    _RwD3D9StateCache.fogColor = 0;

    SetRenderState(D3DRS_FOGENABLE, false);

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
    SetRenderState(D3DRS_ZWRITEENABLE, true);
    SetRenderState(D3DRS_ZENABLE, true);
    _RwD3D9StateCache.zWriteEnable = true;
    _RwD3D9StateCache.zTestEnable = true;

    /* Stencil */
    SetRenderState(D3DRS_STENCILENABLE, false);
    SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
    SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
    SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
    SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
    SetRenderState(D3DRS_STENCILREF, 0);
    SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
    SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
    _RwD3D9StateCache.stencilEnable = false;
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
        _RwD3DDevice->SetTexture(i, nullptr);
        _RwD3D9StateCache.stage[i].curTexRaster = nullptr;

        SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        _RwD3D9StateCache.stage[i].filterMode = rwFILTERLINEAR;
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
    _RwD3D9StateCache.alphaTestEnable = true;
    SetRenderState(D3DRS_ALPHAREF, 0x00000000);
    SetRenderState(D3DRS_ALPHABLENDENABLE, false);
    SetRenderState(D3DRS_ALPHATESTENABLE, false);

    _RwD3D9StateCache.vertexAlphaBlendEnable = false;
    _RwD3D9StateCache.textureAlphaBlendEnable = false;

    /* Turn lighting off */
    SetRenderState(D3DRS_LIGHTING, false);

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

    SetRenderState(D3DRS_DITHERENABLE, true);
    SetRenderState(D3DRS_SPECULARENABLE, false);
    SetRenderState(D3DRS_LOCALVIEWER, false);
    SetRenderState(D3DRS_AMBIENT, 0xffffffff);
    SetRenderState(D3DRS_NORMALIZENORMALS, false);

    RenderStateFlushCache();
}

void RenderStates::_rwD3D9RenderStateReset()
{
    RwUInt32 fogStart, fogEnd;
    RwUInt32 alphaRef;
    RwInt32 i, dither;

    /* Save some parameters */
    fogStart = RenderStateCache[D3DRS_FOGSTART].value;
    fogEnd = RenderStateCache[D3DRS_FOGEND].value;

    alphaRef = RenderStateCache[D3DRS_ALPHAREF].value;

    /* Clear renderstate state */
    D3D9RenderStateCacheClear();
    D3D9TextureStageStateCacheClear();
    D3D9SamplerStageStateCacheClear();
   // D3D9ClearCacheMaterials();

    /* Fog */
    SetRenderState(D3DRS_FOGDENSITY, FLOATASDWORD(_RwD3D9StateCache.fogDensity));

    SetRenderState(D3DRS_FOGENABLE, _RwD3D9StateCache.fogEnable);

    if ((_RwD3D9DeviceCaps.RasterCaps & D3DPRASTERCAPS_FOGTABLE) != 0 &&
        (_RwD3D9DeviceCaps.RasterCaps & D3DPRASTERCAPS_WFOG) != 0)
    {
        SetRenderState(D3DRS_FOGTABLEMODE, _RwD3D9FogConvTable[_RwD3D9StateCache.fogType]);
        SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
    }
    else
    {
        SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
        SetRenderState(D3DRS_FOGVERTEXMODE, _RwD3D9FogConvTable[_RwD3D9StateCache.fogType]);
    }

    SetRenderState(D3DRS_FOGSTART, fogStart);
    SetRenderState(D3DRS_FOGEND, fogEnd);
    SetRenderState(D3DRS_FOGCOLOR, _RwD3D9StateCache.fogColor);

    /* Shading mode */
    SetRenderState(D3DRS_SHADEMODE, _RwD3D9ShadeModeConvTable[_RwD3D9StateCache.shadeMode]);

    /* Set Z Compare mode */
    SetRenderState(D3DRS_ZWRITEENABLE, _RwD3D9StateCache.zWriteEnable);

    if (_RwD3D9StateCache.zTestEnable)
        SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    else
        SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

    if (_RwD3D9StateCache.zTestEnable || _RwD3D9StateCache.zWriteEnable)
        SetRenderState(D3DRS_ZENABLE, TRUE);
    else
        SetRenderState(D3DRS_ZENABLE, FALSE);

    /* Stencil */
    SetRenderState(D3DRS_STENCILENABLE, _RwD3D9StateCache.stencilEnable);
    SetRenderState(D3DRS_STENCILFAIL, _RwD3D9StencilOpConvTable[_RwD3D9StateCache.stencilFailOp]);
    SetRenderState(D3DRS_STENCILZFAIL, _RwD3D9StencilOpConvTable[_RwD3D9StateCache.stencilZFailOp]);
    SetRenderState(D3DRS_STENCILPASS, _RwD3D9StencilOpConvTable[_RwD3D9StateCache.stencilPassOp]);
    SetRenderState(D3DRS_STENCILFUNC, _RwD3D9CmpFuncConvTable[_RwD3D9StateCache.stencilFunction]);
    SetRenderState(D3DRS_STENCILREF, _RwD3D9StateCache.stencilRef);
    SetRenderState(D3DRS_STENCILMASK, _RwD3D9StateCache.stencilMask);
    SetRenderState(D3DRS_STENCILWRITEMASK, _RwD3D9StateCache.stencilWriteMask);

    /* Texture states */
    for (i = 0; i < RWD3D9_MAX_TEXTURE_STAGES; i++)
    {
        /* Texture */
        _RwD3DDevice->SetTexture(i, NULL);
        _RwD3D9StateCache.stage[i].curTexRaster = NULL;

        /* Texture filter mode - Liner/Trilinear */
        SetSamplerState(i, D3DSAMP_MINFILTER,  _RwD3D9FilterModeConvTable[_RwD3D9StateCache.stage[i].filterMode].minmag);
        SetSamplerState(i, D3DSAMP_MAGFILTER, _RwD3D9FilterModeConvTable[_RwD3D9StateCache.stage[i].filterMode].minmag);
        SetSamplerState(i, D3DSAMP_MIPFILTER, _RwD3D9FilterModeConvTable[_RwD3D9StateCache.stage[i].filterMode].mip);

        /* Texture addressing mode */
        SetSamplerState(i, D3DSAMP_ADDRESSU, _RwD3D9AddressConvTable[_RwD3D9StateCache.stage[i].texAddressU]);
        SetSamplerState(i, D3DSAMP_ADDRESSV,  _RwD3D9AddressConvTable[_RwD3D9StateCache.stage[i].texAddressV]);

        /* Border color */
        SetSamplerState(i, D3DSAMP_BORDERCOLOR, _RwD3D9StateCache.stage[i].borderColor);

        /* Max Anisotropy Level: 1 to disable it */
        SetSamplerState(i, D3DSAMP_MAXANISOTROPY, _RwD3D9StateCache.stage[i].maxAnisotropy);

        /* Disable stage if not the first one */
        if (i)
        {
            SetTextureStageState(i, D3DTSS_COLOROP, D3DTOP_DISABLE);
            SetTextureStageState(i, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
        }
    }

    /* Set default blending modes */
    SetRenderState(D3DRS_SRCBLEND, _RwD3D9BlendConvTable[_RwD3D9StateCache.srcBlend]);

    SetRenderState(D3DRS_DESTBLEND, _RwD3D9BlendConvTable[_RwD3D9StateCache.dstBlend]);

    /* Set up the alpha compare function for future use */
    SetRenderState(D3DRS_ALPHAFUNC, _RwD3D9CmpFuncConvTable[_RwD3D9StateCache.alphaTestFunction]);
    SetRenderState(D3DRS_ALPHAREF, alphaRef);

    /* Disable alpha blending */
    if (_RwD3D9StateCache.vertexAlphaBlendEnable)
    {
        SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        SetRenderState(D3DRS_ALPHATESTENABLE, _RwD3D9StateCache.alphaTestEnable);
    }
    else
    {
        SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    }

    _RwD3D9StateCache.textureAlphaBlendEnable = FALSE;

    /* Turn lighting off */
    SetRenderState(D3DRS_LIGHTING, FALSE);

    /* Cull clockwise facing triangles */
    SetRenderState(D3DRS_CULLMODE, _RwD3D9CullModeConvTable[_RwD3D9StateCache.cullMode]);

    /* Vertex color only until a texture has been set */
    SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

    /* Diffuse, Specular & Ambient colors come from the material */
    SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
    SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
    SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);


    SetRenderState(D3DRS_DITHERENABLE, true);
    SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    SetRenderState(D3DRS_LOCALVIEWER, FALSE);
    SetRenderState(D3DRS_AMBIENT, 0xffffffff);
    SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);

    _rwD3D9RenderStateFlushCache();

}