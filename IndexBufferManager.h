#pragma once

#include "RwIndexBuffer.h"

class IndexBufferManager
{
	static std::list<RwIndexBuffer*> mIndexBufferList;
public:
	static void AddToList(RwIndexBuffer*buffer);
	static void Restore();
	static void RemoveFromList(RwIndexBuffer* buffer);
	static void Release();
	static void Destroy();
};

