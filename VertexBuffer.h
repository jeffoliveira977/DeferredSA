#pragma once
#include "CommonD.h"

class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	void Allocate(RwUInt32 size, RwUInt32 stride);
	void Copy(RwUInt32 size, void *data);
	void Set();
	IDirect3DVertexBuffer9* GetBuffer();
private:
	IDirect3DVertexBuffer9* m_vertexBuffer;
	RwUInt32 m_stride;
};

