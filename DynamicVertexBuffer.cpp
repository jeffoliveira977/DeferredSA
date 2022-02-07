#include "DynamicVertexBuffer.h"

namespace DeferredRenderingEngine
{
    list<VertexBuffer*> DynamicVertexBuffer::mVertexBufferList;

    VertexBuffer* DynamicVertexBuffer::Create(uint32_t size, uint32_t stride)
    {
        VertexBuffer* vertexbuffer = new VertexBuffer(size, stride);
        try
        {
            vertexbuffer->Initialize();
        }
        catch (const std::exception &e)
        {
            MessageBox(NULL, e.what(), "Error!", MB_OK);
            return nullptr;
        }

        mVertexBufferList.push_back(vertexbuffer);

        return vertexbuffer;
    }

    void DynamicVertexBuffer::Create(VertexBuffer* buffer)
    {
        if (buffer == nullptr)
            return;

        mVertexBufferList.push_back(buffer);
    }

    void DynamicVertexBuffer::Destroy(VertexBuffer* vertexBuffer)
    {
        if (vertexBuffer == nullptr)
            return;

        mVertexBufferList.remove(vertexBuffer);
        SAFE_DELETE(vertexBuffer);
    }

    void DynamicVertexBuffer::DestroyAll()
    {
        for (auto vertexBuffer : mVertexBufferList)
        {
            SAFE_DELETE(vertexBuffer);
        }

        mVertexBufferList.clear();
    }

    void DynamicVertexBuffer::Release()
    {
        for (auto vertexBuffer : mVertexBufferList)
        {
            if (vertexBuffer)
            {
                vertexBuffer->Deinitialize();
            }
        }
    }

    void DynamicVertexBuffer::Restore()
    {
        for (auto vertexBuffer : mVertexBufferList)
        {
            if (vertexBuffer)
            {
                try
                {
                    vertexBuffer->Initialize();
                }
                catch (const std::exception &e)
                {
                    MessageBox(GetActiveWindow(), e.what(), "Error!", MB_OK);
                }

            }
        }
    }
}
