#include "VertexBuffer.h"

VertexBuffer::VertexBuffer()
{
    mVertexBuffer = nullptr;
}

VertexBuffer::~VertexBuffer()
{
    //RwD3D9DynamicVertexBufferDestroy(mVertexBuffer);   

    SAFE_RELEASE(mVertexBuffer);
}

void VertexBuffer::Initialize(RwUInt32 size, RwUInt32 stride, bool dynamic)
{
    if (size == 0)
        throw std::invalid_argument("invalid size for allocating vertex buffer");

    if (stride == 0)
        throw std::invalid_argument("invalid stride for allocating vertex buffer");

    m_stride = stride;
    mSize = size;
    RwBool result = FALSE;

    /*if (dynamic)
    {
        result = RwD3D9DynamicVertexBufferCreate(stride * size, &mVertexBuffer);
    }
    else
    {
        RwUInt32 of = 0;
        result = RwD3D9CreateVertexBuffer(stride, size * stride, &mVertexBuffer, &of);
    }*/

    mDynamic = dynamic;

    if (dynamic)
        RwD3DDevice->CreateVertexBuffer(size * stride, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &mVertexBuffer, 0);
    else
        RwD3DDevice->CreateVertexBuffer(size * stride, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &mVertexBuffer, 0);

    /*if (!result)
        throw std::runtime_error("VertexBuffer::Initialize");*/
}

void VertexBuffer::Release()
{
    SAFE_RELEASE(mVertexBuffer);
}

void VertexBuffer::Restore()
{
    if (mDynamic)
       RwD3DDevice->CreateVertexBuffer(mSize * m_stride, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &mVertexBuffer, 0);
   else
      RwD3DDevice->CreateVertexBuffer(mSize * m_stride, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &mVertexBuffer, 0);

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

void VertexBuffer::Apply()
{
    RwD3D9SetStreamSource(0, mVertexBuffer, 0, m_stride);
}

void VertexBuffer::Map(RwUInt32 size, void** data)
{
    if(mVertexBuffer == nullptr || data == nullptr)
        return;

    mVertexBuffer->Lock(0, size, data, D3DLOCK_DISCARD);
}

void VertexBuffer::Unmap()
{
    if(mVertexBuffer == nullptr)
        return;

    mVertexBuffer->Unlock();
}

IDirect3DVertexBuffer9* VertexBuffer::GetBuffer()
{
    return mVertexBuffer;
}
