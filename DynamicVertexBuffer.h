#pragma once
#include "CommonD.h"
#include "VertexBuffer.h"

namespace DeferredRenderingEngine
{
	class DynamicVertexBuffer
	{
	public:
		static list<VertexBuffer*> mVertexBufferList;
		static VertexBuffer* Create(uint32_t size, uint32_t stride);
		static void Create(VertexBuffer* buffer);
		static void Destroy(VertexBuffer* vb);
		static void DestroyAll();
		static void Release();
		static void Restore();
	};
}
