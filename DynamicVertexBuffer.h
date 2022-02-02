#pragma once
#include "CommonD.h"
#include "VertexBuffer.h"

class DynamicVertexBuffer
{
public:
	struct dVB
	{
		int32_t size;
		int32_t stride;
		VertexBuffer* vb;
	};

	static list<dVB> m_dynamicVertexBufferList;

	static void Release();
	static void Restore();

	static VertexBuffer* CreateDynamicVertexBuffer(int32_t size, int32_t stride);

	static void DestroyDynamicVertexBuffer(VertexBuffer*vb);
	static void DestroyAll();
};

