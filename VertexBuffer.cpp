#include "VertexBuffer.h"

VertexBuffer::VertexBuffer()
{
    mVertexBuffer = nullptr;
}

VertexBuffer::~VertexBuffer()
{
    RwD3D9DynamicVertexBufferDestroy(mVertexBuffer);   
}

void VertexBuffer::Initialize(RwUInt32 size, RwUInt32 stride)
{
    if(size == 0)
        throw std::invalid_argument("invalid size for allocating vertex buffer");

    if(stride == 0)
        throw std::invalid_argument("invalid stride for allocating vertex buffer");

    m_stride = stride;
    auto result = RwD3D9DynamicVertexBufferCreate(m_stride * size, &mVertexBuffer);

    if(!result)
        throw std::runtime_error("VertexBuffer::Allocate");
}

void VertexBuffer::Copy(RwUInt32 size, void* data)
{
    if(mVertexBuffer == nullptr || 
       data == nullptr)
        return;

    void* bufferMem = NULL;
    mVertexBuffer->Lock(0, m_stride * size, &bufferMem, D3DLOCK_DISCARD);
    memcpy(bufferMem, data, m_stride * size);
    mVertexBuffer->Unlock();
}

void VertexBuffer::Set()
{
    RwD3D9SetStreamSource(0, mVertexBuffer, 0, m_stride);
}

IDirect3DVertexBuffer9* VertexBuffer::GetBuffer()
{
    return mVertexBuffer;
}
