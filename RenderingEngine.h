#pragma once

#include "CommonD.h"
#include "DynamicVertexBuffer.h"
#include "DynamicIndexBuffer.h"

namespace DeferredRenderingEngine
{
	class RenderingEngine
	{
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

		static IDirect3DSurface9* mCurrentDepthStencilSurface;
		static IDirect3DSurface9* mCurrentRenderSurface[8];

		static void* mLastVertexShaderUsed; 
		static void* mLastPixelShaderUsed;

		static RxD3D9VertexStream mLastVertexStreamUsed[4];

		static IDirect3DIndexBuffer9* mLastIndexBufferUsed;

		typedef struct
		{
			uint32_t value;
			bool changed;
		} RxStateCache;

		static RxStateCache mRenderStateCache[210];
		static uint32_t mRenderStateD3D[210];
		static D3DRENDERSTATETYPE mRenderStateChanges[210];
		static uint32_t mNumRenderStateChanges;

		static RxStateCache mTextureStageStateCache[RWD3D9_MAX_TEXTURE_STAGES][33];
		static RwUInt32 mTextureStageStateD3D[RWD3D9_MAX_TEXTURE_STAGES][33];

		typedef struct
		{
			uint32_t stage;
			D3DTEXTURESTAGESTATETYPE type;
		} RxTextureStageChangeType;

		static uint32_t mNumTextureStageStateChanges;
		static RxTextureStageChangeType mTextureStageStateChanges[RWD3D9_MAX_TEXTURE_STAGES * 33];
		static uint32_t mSamplerStageStateD3D[RWD3D9_MAX_TEXTURE_STAGES][14];

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
		static bool DynamicVertexBufferLock(uint32_t vertexSize, uint32_t numVertex, IDirect3DVertexBuffer9** vertexBufferOut,
			void** vertexDataOut, uint32_t* baseIndexOut);
		static void DynamicVertexBufferUnlock(IDirect3DVertexBuffer9* vertexBufferOut);
		static void DynamicVertexBufferRestore();
		static void DynamicVertexBufferRelease();

		static bool CreateVertexDeclaration(D3DVERTEXELEMENT9* elements, IDirect3DVertexDeclaration9** decl);
		static void DeleteVertexDeclaration(IDirect3DVertexDeclaration9* decl);

		static bool SetRenderTarget(uint32_t index, IDirect3DSurface9* rendertarget);
		static bool SetDepthStencilSurface(IDirect3DSurface9* depthStencilSurface);

		static bool CreateVertexShader(DWORD* function, IDirect3DVertexShader9** shader);
		static void DeleteVertexShader(IDirect3DVertexShader9* shader);
		static bool CreatePixelShader(DWORD* function, IDirect3DPixelShader9** shader);
		static void DeletePixelShader(IDirect3DPixelShader9* shader);

		static void SetStreamSource(uint32_t streamNumber, IDirect3DVertexBuffer9* streamData, uint32_t offset, uint32_t stride);
		static void SetStreams(RxD3D9VertexStream* streams, bool useOffsets);

		static void SetIndices(IDirect3DIndexBuffer9* indexBuffer);

		static void RenderStateFlushCache();

		static void GetRenderState(uint32_t state, void* value);

		static void SetTextureStageState(uint32_t stage, uint32_t type, uint32_t value);
		static void GetTextureStageState(uint32_t stage, uint32_t type, void* value);

		void SetSamplerState(uint32_t stage, uint32_t type, uint32_t value);
		void GetSamplerState(uint32_t stage, uint32_t type, void* value);
		void ForceSamplerState(uint32_t stage, uint32_t type, uint32_t value);

		void ForceRenderState(uint32_t state, uint32_t value);
		void ForceTextureStageState(uint32_t stage, uint32_t type, uint32_t value);
	};
}
