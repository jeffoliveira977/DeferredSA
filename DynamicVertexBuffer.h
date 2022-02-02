#pragma once
#include "CommonD.h"
#include "VertexBuffer.h"

namespace DeferredRenderingEngine
{
	class DynamicVertexBuffer
	{
	public:
		static list<VertexBuffer*> m_dynamicVertexBufferList;

		static void Release();
		static void Restore();

		static VertexBuffer* Create(uint32_t size, uint32_t stride);

		static void Destroy(VertexBuffer* vb);
		static void DestroyAll();
	};
}
