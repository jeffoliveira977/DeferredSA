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
		static bool SetDepthStencilRaster(RwRaster* raster);
		static IDirect3DSurface9* GetCurrentD3DRenderTarget(uint32_t index);

		static bool CreateVertexShader(DWORD* function, IDirect3DVertexShader9** shader);
		static void DeleteVertexShader(IDirect3DVertexShader9* shader);
		static bool CreatePixelShader(DWORD* function, IDirect3DPixelShader9** shader);
		static void DeletePixelShader(IDirect3DPixelShader9* shader);

		static void SetStreamSource(uint32_t streamNumber, IDirect3DVertexBuffer9* streamData, uint32_t offset, uint32_t stride);
		static void SetStreams(RxD3D9VertexStream* streams, bool useOffsets);

		static void SetIndices(IDirect3DIndexBuffer9* indexBuffer);

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

		static RwBool CameraClear(void* out __RWUNUSED__, void* cameraIn, RwInt32 nIn __RWUNUSED__);
		static RwBool CameraBeginUpdate(void* out __RWUNUSED__, void* cameraIn, RwInt32 in __RWUNUSED__);
		static RwBool CameraEndUpdate(void* camera, void* color, RwInt32 clearFlags);
		static RwBool RasterShowRaster(void* raster, void* inOut, RwInt32 flags);

		static bool BeginScene();
		static bool TestState();

		static bool RestoreVideoMemory();
		static void ReleaseVideoMemory();
		static const void* D3D9GetCaps();

		static void D3D9SetPresentParameters(const D3DDISPLAYMODE* mode, RwUInt32 flags, D3DFORMAT adapterFormat);
		static RwBool RwD3D9ChangeVideoMode(RwInt32 modeIndex);
		static RwBool D3D9DeviceSystemOpen(void* out __RWUNUSED__, void* inOut, RwInt32 in __RWUNUSED__);
		static RwBool D3D9DeviceSystemClose(void* pOut __RWUNUSED__, void* pInOut __RWUNUSED__, RwInt32 nIn __RWUNUSED__);
		static RwBool D3D9DeviceSystemStart(void* out __RWUNUSED__, void* inOut __RWUNUSED__, RwInt32 in __RWUNUSED__);
		static RwBool D3D9DeviceSystemFinalizeStart(void* out __RWUNUSED__, void* inOut __RWUNUSED__, RwInt32 in __RWUNUSED__);
		static RwBool D3D9DeviceSystemStop(void* out __RWUNUSED__, void* inOut __RWUNUSED__, RwInt32 in __RWUNUSED__);
		static void D3D9CreateDisplayModesList(void);
		static RwBool D3D9System(RwInt32 request, void* out, void* inOut, RwInt32 in);
		 static RwBool D3D9DeviceSystemStandards(void* out, void* inOut __RWUNUSED__, RwInt32 numStandardsFunctions);
		static RwDevice* _rwDeviceGetHandle(void);

		static void D3D9ClearCacheShaders();
		static void D3D9ClearCacheMatrix();
		static void	D3D9ClearCache();
		static void	D3D9ClearCacheLights();
	};
}
