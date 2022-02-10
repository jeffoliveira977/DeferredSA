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


		static bool CreateVertexDeclaration(D3DVERTEXELEMENT9* elements,
			IDirect3DVertexDeclaration9** decl);

		static void DeleteVertexDeclaration(IDirect3DVertexDeclaration9* decl);
	};
}
