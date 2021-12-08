#pragma once
#include "CommonD.h"
class RwIndexBuffer
{
public:
	RwIndexBuffer();
	~RwIndexBuffer();

	void Initialize(RwUInt32 size, bool dynamic=true);
	void Release();
	void Restore();
	void Map(RwUInt32 size, void** data);
	void Unmap();


	IDirect3DIndexBuffer9* GetBuffer();
	int GetSize();
	int IsDynamic();
private:

	int mSize;
	bool mDynamic;
	IDirect3DIndexBuffer9* m_buffer;
};

