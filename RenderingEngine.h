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
			UINT size;
			VertexBuffer* vb;

			bool operator==(dVB& a) 
			{
				return (a.vb == this->vb && a.size == this->size);
			}
		};

		struct DVB_Manager
		{
			uint32_t offset;
			uint32_t size;
			IDirect3DVertexBuffer9* vertexbuffer;
		}; 

		struct RxD3D9DynamicVertexBuffer
		{
			RwUInt32            size;
			RwUInt32            used;
			IDirect3DIndexBuffer9* vertexBuffer;
		};

		struct RxD3D9freeVBlistEntry
		{
			RwUInt32                offset;
			RwUInt32                size;
			IDirect3DIndexBuffer9* vertexBuffer;
		};

		struct RxD3D9createdVBlistEntry
		{
			void* vertexBuffer;
		};

		struct RxD3D9StrideEntry
		{
			RwUInt32                    stride;
			VertexBuffer* vertexBuffer;
			RwUInt32                offset;
			RwUInt32                size;

			bool operator==(RxD3D9StrideEntry& a)
			{
				return (a.stride == this->stride && a.size == this->size && a.offset == this->offset);
			}

			//std::list<RxD3D9freeVBlistEntry*> freelist;
			//std::list<RxD3D9createdVBlistEntry*> vblist;
		};

		static std::list<RxD3D9StrideEntry> StrideList;
		static int mCurrentDynamicVertexBufferInfo;
		static DVB_Manager mDynamicVertexBufferInfo[4];
		static std::list<dVB> mDynamicVertexBufferList;
		static std::list<IndexBuffer*> mDynamicIndexBufferList;
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
	};
}
