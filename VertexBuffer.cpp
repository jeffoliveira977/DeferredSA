#include "VertexBuffer.h"

namespace DeferredRenderingEngine
{
    VertexBuffer::VertexBuffer(uint32_t size, uint32_t stride, bool dynamic)
    {
        mVertexBuffer = nullptr;  
        m_stride = stride;
        mSize = size;
        mDynamic = dynamic;
    }

    VertexBuffer::~VertexBuffer()
    {
        SAFE_RELEASE(mVertexBuffer);
    }

    void VertexBuffer::Initialize()
    {
        try
        {
            HRESULT result;

            if (mDynamic)
                result = RwD3DDevice->CreateVertexBuffer(mSize * m_stride, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,
                    D3DPOOL_DEFAULT, &mVertexBuffer, 0);
            else
                result = RwD3DDevice->CreateVertexBuffer(mSize * m_stride, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED,
                    &mVertexBuffer, 0);

            if (FAILED(result) || mVertexBuffer == nullptr)
            {
                throw std::runtime_error("failed to create vertex buffer");
            }
        }
        catch (const std::runtime_error & e)
        {
            SAFE_RELEASE(mVertexBuffer);
            throw std::runtime_error(std::string("VertexBuffer::Initialize - ") + e.what());
        }
    }

    void VertexBuffer::Deinitialize()
    {
        SAFE_RELEASE(mVertexBuffer);
    }

    void VertexBuffer::Apply()
    {
        if (mVertexBuffer == nullptr)
            return;

        RwD3D9SetStreamSource(0, mVertexBuffer, 0, m_stride);
    }

    void VertexBuffer::Map(uint32_t size, void** data)
    {
        if (mVertexBuffer == nullptr || data == nullptr)
            return;

        mVertexBuffer->Lock(0, size, data, D3DLOCK_DISCARD);
    }

    void VertexBuffer::Unmap()
    {
        if (mVertexBuffer == nullptr)
            return;

        mVertexBuffer->Unlock();
    }

    IDirect3DVertexBuffer9* VertexBuffer::GetObject()
    {
        return mVertexBuffer;
    }
}
