#pragma once
#include "CommonD.h"

namespace DeferredRenderingEngine
{
	class VertexBuffer
	{
	public:
		VertexBuffer();
		~VertexBuffer();

		void Initialize(uint32_t size, uint32_t stride, bool dynamic = true);
		void Release();
		void Restore();
		void Apply();

		void Map(RwUInt32 size, void** data);
		void Unmap();

		IDirect3DVertexBuffer9* GetObject();
	private:
		IDirect3DVertexBuffer9* mVertexBuffer;
		uint32_t m_stride;
		uint32_t mSize;
		bool mDynamic;
	};
}

