#include "DynamicVertexBuffer.h"

namespace DeferredRenderingEngine
{
    list<VertexBuffer*> DynamicVertexBuffer::m_dynamicVertexBufferList;

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

        m_dynamicVertexBufferList.push_back(vertexbuffer);

        return vertexbuffer;
    }

    void DynamicVertexBuffer::Destroy(VertexBuffer* vertexBuffer)
    {
        if (vertexBuffer == nullptr)
            return;

        m_dynamicVertexBufferList.remove(vertexBuffer);
        SAFE_DELETE(vertexBuffer);
    }

    void DynamicVertexBuffer::DestroyAll()
    {
        for (auto vertexBuffer : m_dynamicVertexBufferList)
        {
            SAFE_DELETE(vertexBuffer);
        }

        m_dynamicVertexBufferList.clear();
    }

    void DynamicVertexBuffer::Release()
    {
        for (auto vertexBuffer : m_dynamicVertexBufferList)
        {
            if (vertexBuffer)
            {
                vertexBuffer->Deinitialize();
            }
        }
    }

    void DynamicVertexBuffer::Restore()
    {
        for (auto vertexBuffer : m_dynamicVertexBufferList)
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
