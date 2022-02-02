#pragma once

#include "IndexBuffer.h"

namespace DeferredRenderingEngine
{
	class DynamicIndexBuffer
	{
		static std::list<IndexBuffer*> mIndexBufferList;
	public:
		static IndexBuffer* Create(uint32_t size);
		static void Restore();
		static void Destroy(IndexBuffer* buffer);
		static void Release();
		static void DestroyAll();
	};
}

