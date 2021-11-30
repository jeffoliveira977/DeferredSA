#include "VertexBuffer.h"

VertexBuffer::VertexBuffer()
{
    m_vertexBuffer = nullptr;
}

VertexBuffer::~VertexBuffer()
{
    RwD3D9DynamicVertexBufferDestroy(m_vertexBuffer);   
}

void VertexBuffer::Allocate(RwUInt32 size, RwUInt32 stride)
{
    if(size == 0)
        throw std::invalid_argument("invalid size for allocating vertex buffer");

    if(stride == 0)
        throw std::invalid_argument("invalid stride for allocating vertex buffer");

    m_stride = stride;
    auto result = RwD3D9DynamicVertexBufferCreate(m_stride * size, &m_vertexBuffer);

    if(!result)
        throw std::runtime_error("VertexBuffer::Allocate");
}

void VertexBuffer::Copy(RwUInt32 size, void* data)
{
    if(m_vertexBuffer == nullptr || 
       data == nullptr)
        return;

    void* bufferMem = NULL;
    m_vertexBuffer->Lock(0, m_stride * size, &bufferMem, D3DLOCK_DISCARD);
    memcpy(bufferMem, data, m_stride * size);
    m_vertexBuffer->Unlock();
}

void VertexBuffer::Set()
{
    RwD3D9SetStreamSource(0, m_vertexBuffer, 0, m_stride);
}

IDirect3DVertexBuffer9* VertexBuffer::GetBuffer()
{
    return m_vertexBuffer;
}
