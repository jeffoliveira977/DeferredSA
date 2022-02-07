#include "IndexBuffer.h"
#include "D3DResourceSystem.h"
#include "DynamicIndexBuffer.h"

namespace DeferredRenderingEngine
{
    IndexBuffer::IndexBuffer(uint32_t size, bool dynamic)
    {
        mIndexBuffer = nullptr;
        mDynamic = dynamic;
        mSize = size;
    }

    IndexBuffer::~IndexBuffer()
    {
        SAFE_RELEASE(mIndexBuffer);
    }

    void IndexBuffer::Initialize()
    {
        if (mIndexBuffer)
        {
            SAFE_RELEASE(mIndexBuffer);
            Log::Warn("RwIndexBuffer::Initialize - buffer not released property");
        }

        try
        {
            HRESULT result;

            if (mSize == 0)
            {
                Log::Error("RwIndexBuffer::Initialize - invalid size");
                throw std::runtime_error("invalid size");
            }

            if (mDynamic)
                result = RwD3DDevice->CreateIndexBuffer(mSize * sizeof(uint16_t), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                    D3DFMT_INDEX16, D3DPOOL_DEFAULT, &mIndexBuffer, 0);
            else
                result = RwD3DDevice->CreateIndexBuffer(mSize * sizeof(uint16_t), D3DUSAGE_WRITEONLY,
                    D3DFMT_INDEX16, D3DPOOL_MANAGED, &mIndexBuffer, 0);

            if (FAILED(result) || mIndexBuffer == nullptr)
            {
                Log::Fatal("RwIndexBuffer::Initialize - failed to create index buffer");
                throw std::runtime_error("failed to create index buffer");
            }
        }
        catch (const std::runtime_error &e)
        {
            SAFE_RELEASE(mIndexBuffer);
            throw std::runtime_error(std::string("RwIndexBuffer::Initialize - ") + e.what());
        }

        Log::Debug("IndexBuffer::Initialize - base size %i", mSize);
    }

    void IndexBuffer::Deinitialize()
    {
        SAFE_RELEASE(mIndexBuffer); 
        Log::Debug("IndexBuffer::Deinitialize");
    }

    void IndexBuffer::Map(uint32_t size, void** data)
    {
        if (size == 0 || mIndexBuffer == nullptr || data == nullptr)
            return;
        
        mIndexBuffer->Lock(0, size, data, mDynamic ? D3DLOCK_DISCARD : 0);
    }

    void IndexBuffer::Unmap()
    {
        if (mIndexBuffer == nullptr)
            return;

        mIndexBuffer->Unlock();
    }

    void IndexBuffer::Apply()
    {
        if (mIndexBuffer == nullptr)
            return;

        _rwD3D9SetIndices(mIndexBuffer);
    }

    IDirect3DIndexBuffer9* IndexBuffer::GetObject()
    {
        return mIndexBuffer;
    }
}
