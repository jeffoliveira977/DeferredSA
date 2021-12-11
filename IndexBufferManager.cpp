#include "IndexBufferManager.h"

std::list<RwIndexBuffer*> IndexBufferManager::mIndexBufferList;
// Same thing for dynamic index buffers



void IndexBufferManager::AddToList(RwIndexBuffer* buffer)
{
	auto it = std::find(mIndexBufferList.begin(), mIndexBufferList.end(), buffer);
	if(it != mIndexBufferList.end())
		
	MessageBox(0, "not", "Error", MB_OK);

	mIndexBufferList.push_back(buffer);
}

void IndexBufferManager::Restore()
{
	for(auto& buffer : mIndexBufferList)
	{
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
