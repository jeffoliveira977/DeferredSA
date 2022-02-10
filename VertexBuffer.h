#pragma once
#include "CommonD.h"

namespace DeferredRenderingEngine
{
	class VertexBuffer
	{
	public:
		VertexBuffer(uint32_t size, uint32_t stride, bool dynamic = true);
		~VertexBuffer();

		void Initialize();
		void Deinitialize();
		void Apply();

		void Map(uint32_t size, void** data);
		void Unmap();

		IDirect3DVertexBuffer9* GetObject();
	private:
		IDirect3DVertexBuffer9* mVertexBuffer;
		uint32_t m_stride;
		uint32_t mSize;
		bool mDynamic;
		static uint32_t mCount;
	};
}

