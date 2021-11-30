#pragma once
#include "CommonD.h"
class RwIndexBuffer
{
public:
	RwIndexBuffer();
	~RwIndexBuffer();

	void Allocate(RwUInt32 size);
	void Copy(RwUInt32 size, void* in, void* out);
	void Set();
	IDirect3DIndexBuffer9* GetBuffer();
private:
	IDirect3DIndexBuffer9* m_buffer;
};

