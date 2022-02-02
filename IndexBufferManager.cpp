#include "IndexBufferManager.h"

std::list<IndexBuffer*> DynamicIndexBuffer::mIndexBufferList;

IndexBuffer* DynamicIndexBuffer::Create(uint32_t size)
{
	IndexBuffer* indexBuffer = new IndexBuffer();
	indexBuffer->Initialize(size);

	mIndexBufferList.push_back(indexBuffer);
	return indexBuffer;
}

void DynamicIndexBuffer::Destroy(IndexBuffer* indexBuffer)
{
	mIndexBufferList.remove(indexBuffer);
	SAFE_DELETE(indexBuffer);
}

void DynamicIndexBuffer::Release()
{
	for (auto& buffer : mIndexBufferList)
	{
		if (buffer)
		{
			buffer->Release();
		}
	}
}

void DynamicIndexBuffer::Restore()
{
	for (auto& buffer : mIndexBufferList)
	{
		if (buffer)
		{
			buffer->Restore();
		}
	}
}

void DynamicIndexBuffer::DestroyAll()
{
	for(auto& buffer : mIndexBufferList)
	{
		SAFE_DELETE(buffer);
	}

	mIndexBufferList.clear();
}
