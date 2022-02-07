#include "DynamicIndexBuffer.h"

namespace DeferredRenderingEngine
{
	std::list<IndexBuffer*> DynamicIndexBuffer::mIndexBufferList;

	IndexBuffer* DynamicIndexBuffer::Create(uint32_t size)
	{
		IndexBuffer* indexBuffer = new IndexBuffer(size, false);
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
		Log::Debug("DynamicIndexBuffer::Create - base size %i", size);
		return indexBuffer;
	}

	void DynamicIndexBuffer::Create(IndexBuffer* buffer)
	{
		if (buffer == nullptr)
			return;

		mIndexBufferList.push_back(buffer);
		Log::Debug("DynamicIndexBuffer::Create");
	}

	void DynamicIndexBuffer::Destroy(IndexBuffer* indexBuffer)
	{
		mIndexBufferList.remove(indexBuffer);
		SAFE_DELETE(indexBuffer);
		Log::Debug("DynamicIndexBuffer::Destroy");
	}

	void DynamicIndexBuffer::Release()
	{
		return;
		Log::Debug("DynamicIndexBuffer::Release");
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
		return;
		Log::Debug("DynamicIndexBuffer::Restore");
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