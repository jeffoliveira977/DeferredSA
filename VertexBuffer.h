#pragma once
#include "CommonD.h"

class VertexBuffer
{
public:
	VertexBuffer();
	~VertexBuffer();

	void Initialize(RwUInt32 size, RwUInt32 stride, bool dynamic = true);
	void Copy(RwUInt32 size, void *data);
	void Set();

	void Map(RwUInt32 size, void** data);
	void Unmap();

	IDirect3DVertexBuffer9* GetBuffer();
private:
	IDirect3DVertexBuffer9* mVertexBuffer;
	RwUInt32 m_stride;
};

