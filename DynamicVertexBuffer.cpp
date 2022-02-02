#include "DynamicVertexBuffer.h"

list<DynamicVertexBuffer::dVB> DynamicVertexBuffer::m_dynamicVertexBufferList;

VertexBuffer* DynamicVertexBuffer::CreateDynamicVertexBuffer(int32_t size, int32_t stride)
{
    VertexBuffer* vertexbuffer = new VertexBuffer();
    vertexbuffer->Initialize(size, stride);
    m_dynamicVertexBufferList.push_back({ size, stride, vertexbuffer });

    return vertexbuffer;
}

void DynamicVertexBuffer::DestroyDynamicVertexBuffer(VertexBuffer* vb)
{
    for (auto it = m_dynamicVertexBufferList.begin(); it != m_dynamicVertexBufferList.end(); ++it)
    {
        if ((*it).vb == vb)
        {
            m_dynamicVertexBufferList.erase(it);
            SAFE_DELETE(vb);
        }
    }
}

void DynamicVertexBuffer::DestroyAll()
{
    for (auto vertexBuffer : m_dynamicVertexBufferList)
    {
        SAFE_DELETE(vertexBuffer.vb);
    }

    m_dynamicVertexBufferList.clear();
}

void DynamicVertexBuffer::Release()
{
    for (auto vertexBuffer : m_dynamicVertexBufferList)
    {
        if (vertexBuffer.vb)
        {
            vertexBuffer.vb->Release();
        }
    }
}

void DynamicVertexBuffer::Restore()
{
    for (auto vertexBuffer : m_dynamicVertexBufferList)
    {
        if (vertexBuffer.vb)
        {
            vertexBuffer.vb->Restore();
        }
    }
}
