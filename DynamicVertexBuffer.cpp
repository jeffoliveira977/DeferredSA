#include "DynamicVertexBuffer.h"

list<VertexBuffer*> DynamicVertexBuffer::m_dynamicVertexBufferList;

VertexBuffer* DynamicVertexBuffer::Create(uint32_t size, uint32_t stride)
{
    VertexBuffer* vertexbuffer = new VertexBuffer();
    try
    {
        vertexbuffer->Initialize(size, stride);
    }
    catch (const std::exception&e)
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
            vertexBuffer->Release();
        }
    }
}

void DynamicVertexBuffer::Restore()
{
    for (auto vertexBuffer : m_dynamicVertexBufferList)
    {
        if (vertexBuffer)
        {
            vertexBuffer->Restore();
        }
    }
}
