#include "VertexBuffer.h"

VertexBuffer::VertexBuffer()
{
    mVertexBuffer = nullptr;
}

VertexBuffer::~VertexBuffer()
{
    SAFE_RELEASE(mVertexBuffer);
}

void VertexBuffer::Initialize(uint32_t size, uint32_t stride, bool dynamic)
{
    if (size == 0)
        throw std::invalid_argument("invalid size for allocating vertex buffer");

    if (stride == 0)
        throw std::invalid_argument("invalid stride for allocating vertex buffer");

    m_stride = stride;
    mSize = size;
    mDynamic = dynamic;

    HRESULT result;
    if (dynamic)
        result = RwD3DDevice->CreateVertexBuffer(size * stride, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &mVertexBuffer, 0);
    else
        result = RwD3DDevice->CreateVertexBuffer(size * stride, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &mVertexBuffer, 0);

    if (FAILED(result))
        throw std::runtime_error("VertexBuffer::Initialize");
}

void VertexBuffer::Release()
{
    SAFE_RELEASE(mVertexBuffer);
}

void VertexBuffer::Restore()
{
    if (mVertexBuffer)
        return;

    HRESULT result;

    if (mDynamic)
        result = RwD3DDevice->CreateVertexBuffer(mSize * m_stride, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &mVertexBuffer, 0);
    else
        result = RwD3DDevice->CreateVertexBuffer(mSize * m_stride, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &mVertexBuffer, 0);

    if (FAILED(result))
        throw std::runtime_error("VertexBuffer::Restore");
}

void VertexBuffer::Apply()
{
    RwD3D9SetStreamSource(0, mVertexBuffer, 0, m_stride);
}

void VertexBuffer::Map(uint32_t size, void** data)
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

IDirect3DVertexBuffer9* VertexBuffer::GetObject()
{
    return mVertexBuffer;
}
