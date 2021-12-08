#include "IndexBufferManager.h"

std::list<RwIndexBuffer*> IndexBufferManager::mIndexBufferList;

void IndexBufferManager::AddToList(RwIndexBuffer* buffer)
{
	mIndexBufferList.push_back(buffer);
}

void IndexBufferManager::Restore()
{
	for(auto& buffer : mIndexBufferList)
	{
		/*if(buffer)
			buffer->Release();*/
		if(buffer)
			buffer->Restore();
	}
}

void IndexBufferManager::RemoveFromList(RwIndexBuffer* indexBuffer)
{
	mIndexBufferList.remove(indexBuffer);
}

void IndexBufferManager::Release()
{
	for(auto& buffer : mIndexBufferList)
	{
		if(buffer)
			buffer->Release();
	}
}

void IndexBufferManager::Destroy()
{
	for(auto& buffer : mIndexBufferList)
	{
		delete buffer;
	}
	mIndexBufferList.clear();
}
