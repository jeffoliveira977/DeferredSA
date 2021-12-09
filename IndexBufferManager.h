#pragma once

#include "RwIndexBuffer.h"

struct DynamicaIndexBuffer
{
	IDirect3DIndexBuffer9* mIndexBuffer;
	int mSize;

	DynamicaIndexBuffer()
	{};
	DynamicaIndexBuffer(int size, IDirect3DIndexBuffer9* indexBuffer)
	{
		mIndexBuffer = indexBuffer;
		mSize = size;
	}
};

class IndexBufferManager
{
	static std::list<DynamicaIndexBuffer*> mList;
	static std::list<RwIndexBuffer*> mIndexBufferList;
public:
	static void AddToList(RwIndexBuffer*buffer);
	static void AddToList(int size, IDirect3DIndexBuffer9** indexBuffer);
	static void Restore();
	static void RemoveFromList(RwIndexBuffer* buffer);
	static void Release();
	static void Destroy();
};

