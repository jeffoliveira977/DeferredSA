#pragma once
#include "CommonD.h"

namespace DeferredRenderingEngine
{
	class IndexBuffer
	{
	public:
		IndexBuffer(uint32_t size, bool dynamic = true);
		~IndexBuffer();

		void Initialize();
		void Deinitialize();
		void Map(uint32_t size, void** data);
		void Unmap();
		void Apply();

		IDirect3DIndexBuffer9* GetObject();
	private:

		uint32_t mSize;
		bool mDynamic;
		IDirect3DIndexBuffer9* m_buffer;
	};
}