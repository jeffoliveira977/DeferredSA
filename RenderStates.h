#pragma once
#include "CommonD.h"

class RenderStates
{
public:
	static void RenderStateFlushCache();

	static void SetRenderState(RwUInt32 state, RwUInt32 value);
	static void GetRenderState(uint32_t state, void* value);

	static void SetTextureStageState(uint32_t stage, uint32_t type, uint32_t value);
	static void GetTextureStageState(uint32_t stage, uint32_t type, void* value);

	static void SetSamplerState(uint32_t stage, uint32_t type, uint32_t value);
	static void GetSamplerState(uint32_t stage, uint32_t type, void* value);
	static void ForceSamplerState(uint32_t stage, uint32_t type, uint32_t value);

	static void ForceRenderState(uint32_t state, uint32_t value);
	static void ForceTextureStageState(uint32_t stage, uint32_t type, uint32_t value);

	static RwBool rwD3D9RenderStateFogEnable(RwBool fog);
	static RwBool rwD3D9RenderStateFogColor(RwUInt32 fogColor);
	static RwBool rwD3D9RenderStateFogType(RwFogType fogType);
	static RwBool rwD3D9RenderStateFogDensity(RwReal fogDensity);
	static void _rwD3D9SetTextureAnisotropyOffset(RwInt32 anisotTextureOffset);
	static RwBool _rwD3D9RenderStateTextureFilter(RwTextureFilterMode filterMode);
	static RwBool _rwD3D9RenderStateTextureAddress(RwTextureAddressMode addressMode);
	static RwBool _rwD3D9RenderStateTextureAddressU(RwTextureAddressMode addressMode);
	static RwBool _rwD3D9RenderStateTextureAddressV(RwTextureAddressMode addressMode);

	static RwBool _rwD3D9RenderStateSrcBlend(RwBlendFunction srcBlend);
	static RwBool _rwD3D9RenderStateDestBlend(RwBlendFunction dstBlend);

#undef _rwD3D9RWSetRasterStage
#undef _rwD3D9RWGetRasterStage
	static RwBool _rwD3D9RWSetRasterStage(RwRaster* raster, RwUInt32 stage);
	static RwRaster* _rwD3D9RWGetRasterStage(RwUInt32 stage);

	static RwBool _rwD3D9RenderStateVertexAlphaEnable(RwBool enable);
	static RwBool _rwD3D9RenderStateIsVertexAlphaEnable();

	static RwBool rwD3D9RenderStateBorderColor(RwUInt32 borderColor);
	static RwBool rwD3D9RenderStateShadeMode(RwShadeMode shadeMode);
	static RwBool rwD3D9RenderStateCullMode(RwCullMode cullMode);
	static RwBool rwD3D9RenderStateZWriteEnable(RwBool enable);
	static RwBool rwD3D9RenderStateZTestEnable(RwBool enable);
	static RwBool _rwD3D9RenderStateStencilEnable(RwBool enable);
	static RwBool _rwD3D9RenderStateStencilFail(RwStencilOperation operation);
	static RwBool _rwD3D9RenderStateStencilZFail(RwStencilOperation operation);
	static RwBool _rwD3D9RenderStateStencilPass(RwStencilOperation operation);
	static RwBool _rwD3D9RenderStateStencilFunction(RwStencilFunction function);
	static RwBool _rwD3D9RenderStateStencilFunctionRef(RwInt32 ref);
	static RwBool _rwD3D9RenderStateStencilFunctionMask(RwInt32 mask);
	static RwBool _rwD3D9RenderStateStencilFunctionWriteMask(RwInt32 writeMask);

	static RwBool RwD3D9SetTexture(RwTexture* texture, RwUInt32 stage);
	static RwBool _rwD3D9RWSetRenderState(RwRenderState state, void* param);
	static RwBool _rwD3D9RWGetRenderState(RwRenderState nState, void* param);

	static void D3D9RenderStateCacheClear();
	static void D3D9TextureStageStateCacheClear();
	static void D3D9SamplerStageStateCacheClear();
	static void _rwD3D9RenderStateOpen();
};

