#include "DynamicIndexBuffer.h"

namespace DeferredRenderingEngine
{
	std::list<IndexBuffer*> DynamicIndexBuffer::mIndexBufferList;

	IndexBuffer* DynamicIndexBuffer::Create(uint32_t size)
	{
		IndexBuffer* indexBuffer = new IndexBuffer(size);
		try
		{
			indexBuffer->Initialize();
		}
		catch (const std::exception &e)
		{
			MessageBox(NULL, e.what(), "Error!", MB_OK);
			return nullptr;
		}

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
				buffer->Deinitialize();
			}
		}
	}

	void DynamicIndexBuffer::Restore()
	{
		for (auto& buffer : mIndexBufferList)
		{
			if (buffer)
			{			
				try
				{
					buffer->Initialize();
				}
				catch (const std::exception & e)
				{
					MessageBox(GetActiveWindow(), e.what(), "Error!", MB_OK);
				}
			}
		}
	}

	void DynamicIndexBuffer::DestroyAll()
	{
		for (auto& buffer : mIndexBufferList)
		{
			SAFE_DELETE(buffer);
		}

		mIndexBufferList.clear();
	}
}