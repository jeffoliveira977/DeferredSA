#pragma once

#include "CommonD.h"
#include "DynamicVertexBuffer.h"
#include "DynamicIndexBuffer.h"

namespace DeferredRenderingEngine
{
	class RenderingEngine
	{
	public:
		struct dVB
		{
			uint32_t size;
			VertexBuffer* vb;
		};

		struct DVB_Manager
		{
			uint32_t offset;
			uint32_t size;
			IDirect3DVertexBuffer9* vertexbuffer;
		}; 
		
		struct rxD3D9VertexDeclarationItem
		{
			LPDIRECT3DVERTEXDECLARATION9 vertexdeclaration;
			uint32_t numElements;
			D3DVERTEXELEMENT9* elements;
		};

		static std::list<VertexBuffer*> mVertexBufferList;
		static uint32_t mCurrentDynamicVertexBufferInfo;
		static DVB_Manager mDynamicVertexBufferInfo[4];
		static std::list<dVB> mDynamicVertexBufferList;
		static std::list<IndexBuffer*> mDynamicIndexBufferList;

		static std::vector<rxD3D9VertexDeclarationItem> m_vertexDeclarations;

	public:

		static bool DynamicVertexBufferListCreate();
		static void DynamicVertexBufferListDestroy();

		static void VertexBufferManagerOpen();
		static void VertexBufferManagerClose();
		static bool CreateVertexBuffer(uint32_t stride, uint32_t size, IDirect3DVertexBuffer9** vertexBuffer, uint32_t* offset);
		static void DestroyVertexBuffer(uint32_t stride, uint32_t size, IDirect3DVertexBuffer9* vertexBuffer, uint32_t offset);

		static bool DynamicVertexBufferManagerCreate();
		static void DynamicVertexBufferManagerDestroy();
		static bool DynamicVertexBufferCreate(uint32_t size, IDirect3DVertexBuffer9** vertexbuffer);
		static bool DynamicVertexBufferDestroy(IDirect3DVertexBuffer9* vertexbuffer);
		static bool DynamicIndexBufferCreate(uint32_t size, IDirect3DIndexBuffer9** vertexbuffer);
		static bool DynamicVertexBufferLock(uint32_t vertexSize, uint32_t numVertex, IDirect3DVertexBuffer9** vertexBufferOut, void** vertexDataOut, uint32_t* baseIndexOut);
		static void DynamicVertexBufferUnlock(IDirect3DVertexBuffer9* vertexBufferOut);
		static void DynamicVertexBufferRestore();
		static void DynamicVertexBufferRelease();

		static bool CreateVertexDeclaration(D3DVERTEXELEMENT9* elements, IDirect3DVertexDeclaration9** decl);
		static void DeleteVertexDeclaration(IDirect3DVertexDeclaration9* decl);

		static bool SetRenderTarget(uint32_t index, RwRaster* raster);
		static bool _SetRenderTarget(uint32_t index, IDirect3DSurface9* rendertarget);
		static bool SetDepthStencilSurface(IDirect3DSurface9* depthStencilSurface);
		static IDirect3DSurface9* GetCurrentD3DRenderTarget(uint32_t index);

		static bool CreateVertexShader(DWORD* function, IDirect3DVertexShader9** shader);
		static void DeleteVertexShader(IDirect3DVertexShader9* shader);
		static bool CreatePixelShader(DWORD* function, IDirect3DPixelShader9** shader);
		static void DeletePixelShader(IDirect3DPixelShader9* shader);

		static void SetStreamSource(uint32_t streamNumber, IDirect3DVertexBuffer9* streamData, uint32_t offset, uint32_t stride);
		static void SetStreams(RxD3D9VertexStream* streams, bool useOffsets);

		static void SetIndices(IDirect3DIndexBuffer9* indexBuffer);

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
		static RwBool  _rwD3D9RenderStateStencilEnable(RwBool enable);
		static RwBool  _rwD3D9RenderStateStencilFail(RwStencilOperation operation);
		static RwBool  _rwD3D9RenderStateStencilZFail(RwStencilOperation operation);
		static RwBool  _rwD3D9RenderStateStencilPass(RwStencilOperation operation);
		static RwBool  _rwD3D9RenderStateStencilFunction(RwStencilFunction function);
		static RwBool  _rwD3D9RenderStateStencilFunctionRef(RwInt32 ref);
		static RwBool  _rwD3D9RenderStateStencilFunctionMask(RwInt32 mask);
		static RwBool  _rwD3D9RenderStateStencilFunctionWriteMask(RwInt32 writeMask);

		static RwBool RwD3D9SetTexture(RwTexture* texture, RwUInt32 stage);
		static RwBool _rwD3D9RWSetRenderState(RwRenderState state, void* param);

		static void D3D9RenderStateCacheClear();
		static void D3D9TextureStageStateCacheClear();
		static void D3D9SamplerStageStateCacheClear();
		static void _rwD3D9RenderStateOpen();

		static void DrawIndexedPrimitiveUP(uint32_t primitiveType, uint32_t minIndex, uint32_t numVertices, uint32_t primitiveCount, const void* indexData,
			const void* vertexStreamZeroData, uint32_t vertexStreamZeroStride);
		static void DrawPrimitiveUP(uint32_t primitiveType, uint32_t primitiveCount, const void* vertexStreamZeroData, uint32_t vertexStreamZeroStride);
		static void DrawIndexedPrimitive(uint32_t primitiveType, int32_t baseVertexIndex, uint32_t minIndex, uint32_t numVertices, uint32_t startIndex, uint32_t primitiveCount);
		static void DrawPrimitive(uint32_t primitiveType, uint32_t startVertex, uint32_t primitiveCount);

		static bool SetTransform(uint32_t state, const void* matrix);
		static void GetTransform(uint32_t state, void* matrix);
		static bool SetTransformWorld(const RwMatrix* matrix);

		static bool IndexBufferCreate(uint32_t numIndices, IDirect3DIndexBuffer9** indexBuffer);
		static bool IndexBuffer32bitsCreate(uint32_t numIndices, IDirect3DIndexBuffer9** indexBuffer);
		
		static void SetVertexShader(IDirect3DVertexShader9* shader);
		static void SetPixelShader(IDirect3DPixelShader9* shader);

		static void SetFVF(uint32_t fvf);
		static void SetVertexDeclaration(IDirect3DVertexDeclaration9* vertexDeclaration);

		static bool CameraBeginUpdate(RwCamera* camera);
		static bool CameraClear(RwCamera* camera, RwRGBA* color, RwInt32 clearFlags);
		static bool CameraEndUpdate(RwCamera* camera);
		static bool RasterShowRaster(void* raster, void* inOut, RwInt32 flags);

		static bool BeginScene();
		static bool TestState();


		static bool RestoreVideoMemory();
		static void ReleaseVideoMemory();
		static const void* D3D9GetCaps();

		static void D3D9SetPresentParameters(const D3DDISPLAYMODE* mode, RwUInt32 flags, D3DFORMAT adapterFormat);
		static RwBool D3D9DeviceSystemOpen(void* out __RWUNUSED__, void* inOut, RwInt32 in __RWUNUSED__);
		static RwBool D3D9DeviceSystemClose(void* pOut __RWUNUSED__, void* pInOut __RWUNUSED__, RwInt32 nIn __RWUNUSED__);
		static RwBool D3D9DeviceSystemStart(void* out __RWUNUSED__, void* inOut __RWUNUSED__, RwInt32 in __RWUNUSED__);
		static RwBool D3D9DeviceSystemFinalizeStart(void* out __RWUNUSED__, void* inOut __RWUNUSED__, RwInt32 in __RWUNUSED__);
		static RwBool D3D9DeviceSystemStop(void* out __RWUNUSED__, void* inOut __RWUNUSED__, RwInt32 in __RWUNUSED__);
		static void D3D9CreateDisplayModesList(void);
		static RwBool D3D9System(RwInt32 request, void* out, void* inOut, RwInt32 in);
		static RwDevice* _rwDeviceGetHandle(void);

		static void D3D9ClearCacheShaders();
		static void D3D9ClearCacheMatrix();
		static void	D3D9ClearCache();
		static void	D3D9ClearCacheLights();

		static RwBool D3D9RasterLock(void* pixelsIn, void* rasterIn, RwInt32 accessMode);
		static RwBool D3D9RasterUnlock(void* unused1 __RWUNUSED__, void* rasterIn, RwInt32 unused3 __RWUNUSED__);

		static RwInt32 D3D9FindCorrectRasterFormat(RwRasterType type, RwInt32 flags);
		static RwBool D3D9CheckRasterFormat(void* rasterIn, RwInt32 flags);

		static RwUInt32 rwD3D9FindRwFormat(D3DFORMAT d3dFormat);
		static RwBool _rwD3D9CheckValidTextureFormat(RwInt32 format);
		static RwBool _rwD3D9CheckValidCameraTextureFormat(RwInt32 format);
		static RwBool rwD3D9SetRasterFormat(void* rasterIn, RwInt32 flags);
#undef _rwD3D9CheckValidZBufferTextureFormat
		static RwBool _rwD3D9CheckValidZBufferTextureFormat(RwInt32 format);
		static RwBool _rwD3D9CheckValidCubeTextureFormat(RwInt32 format);
		static RwBool _rwD3D9CheckValidCameraCubeTextureFormat(RwInt32 format);
		static RwBool _rwD3D9CheckValidZBufferFormat(RwInt32 format);
		static RwBool _rwD3D9CheckAutoMipmapGenCubeTextureFormat(RwInt32 format);
		static RwBool _rwD3D9CheckAutoMipmapGenTextureFormat(RwInt32 format);
		static RwBool _rwD3D9CheckAutoMipmapGenCameraTextureFormat(RwInt32 format);
		static RwBool _rwD3D9RasterHasAlpha(const RwRaster* raster);
		static RwBool _rwD3D9TextureHasAlpha(const RwTexture* texture);

		static RwBool _rwD3D9NativeTextureRead(void* streamIn, void* textureIn, RwInt32 unused3 __RWUNUSED__);
#undef _rwD3D9CubeRasterCreate
		static RwBool _rwD3D9CubeRasterCreate(RwRaster* raster, RwUInt32 d3dformat, RwInt32 mipmapLevels);
#undef _rwD3D9CubeRasterSelectFace
		static void _rwD3D9CubeRasterSelectFace(RwRaster* raster, RwUInt32 face);
		static RwBool _rwD3D9RasterIsCubeRaster(const RwRaster* raster);
		static void  _rwD3D9CheckRasterSize(RwInt32* width, RwInt32* height, RwUInt32 rasterFlags);
		static RwBool rwD3D9CreateTextureRaster(RwRaster* raster, _rwD3D9RasterExt* rasExt);
		static RwBool _rwD3D9RasterCreate(void* unused1 __RWUNUSED__, void* rasterIn, RwInt32 flags);
		static RwBool rwD3D9CreateZBufferRaster(RwRaster* raster, _rwD3D9RasterExt* rasExt);
		static RwBool rwD3D9CreateCameraTextureRaster(RwRaster* raster, _rwD3D9RasterExt* rasExt);
		static RwBool _rwD3D9RasterDestroy(void* unused1 __RWUNUSED__, void* raster, RwInt32 unused3 __RWUNUSED__);

		static void* rwD3D9RasterCtor(void* object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__);
		static void* rwD3D9RasterDtor(void* object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__);
		static RwBool _rwD3D9RasterPluginAttach();
		static void _rwD3D9RasterOpen();
		static void _rwD3D9RasterClose();

		static void rxD3D9VideoMemoryRasterListDestroy();
		static void _rxD3D9VideoMemoryRasterListRelease();
		static RwBool _rxD3D9VideoMemoryRasterListRestore();
		static RwBool rxD3D9VideoMemoryRasterListCreate();
		static void rxD3D9VideoMemoryRasterListRemove(RwRaster* raster);
		static void rxD3D9VideoMemoryRasterListAdd(RwRaster* raster);
	};
}
