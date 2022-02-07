#pragma once

#include "IndexBuffer.h"

namespace DeferredRenderingEngine
{
	class DynamicIndexBuffer
	{
		static std::list<IndexBuffer*> mIndexBufferList;
	public:
		static IndexBuffer* Create(uint32_t size);
		static void Create(IndexBuffer* buffer); 
		static void Destroy(IndexBuffer* buffer);
		static void DestroyAll();
		static void Restore();
		static void Release();
	};
}

