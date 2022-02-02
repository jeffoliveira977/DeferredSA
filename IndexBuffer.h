#pragma once
#include "CommonD.h"

namespace DeferredRenderingEngine
{
	class IndexBuffer
	{
	public:
		IndexBuffer();
		~IndexBuffer();

		void Initialize(uint32_t size, bool dynamic = true);
		void Release();
		void Restore();
		void Map(uint32_t size, void** data);
		void Unmap();
		void Apply();

		IDirect3DIndexBuffer9* GetObject();
	private:

		int mSize;
		bool mDynamic;
		IDirect3DIndexBuffer9* m_buffer;
	};
}