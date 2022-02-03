#include "IndexBuffer.h"
#include "D3DResourceSystem.h"
#include "DynamicIndexBuffer.h"

namespace DeferredRenderingEngine
{
    IndexBuffer::IndexBuffer(uint32_t size, bool dynamic)
    {
        m_buffer = nullptr;
        mDynamic = dynamic;
        mSize = size;
    }

    IndexBuffer::~IndexBuffer()
    {
        SAFE_RELEASE(m_buffer);
    }

    void IndexBuffer::Initialize()
    {
        try
        {
            HRESULT result;

            if (mDynamic)
                result = RwD3DDevice->CreateIndexBuffer(mSize * sizeof(uint16_t), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC | D3DUSAGE_DONOTCLIP,
                    D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_buffer, 0);
            else
                result = RwD3DDevice->CreateIndexBuffer(mSize * sizeof(uint16_t), D3DUSAGE_WRITEONLY,
                    D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_buffer, 0);

            if (FAILED(result) || m_buffer == nullptr)
            {
                throw std::runtime_error("failed to create index buffer");
            }
        }
        catch (const std::runtime_error &e)
        {
            SAFE_RELEASE(m_buffer);
            throw std::runtime_error(std::string("RwIndexBuffer::Initialize - ") + e.what());
        }
    }

    void IndexBuffer::Deinitialize()
    {
        SAFE_RELEASE(m_buffer);
    }

    void IndexBuffer::Map(uint32_t size, void** data)
    {
        if (m_buffer == nullptr || data == nullptr)
            return;

        m_buffer->Lock(0, size, data, mDynamic ? D3DLOCK_DISCARD : 0);
    }

    void IndexBuffer::Unmap()
    {
        if (m_buffer == nullptr)
            return;

        m_buffer->Unlock();
    }

    void IndexBuffer::Apply()
    {
        if (m_buffer == nullptr)
            return;

        _rwD3D9SetIndices(m_buffer);
    }

    IDirect3DIndexBuffer9* IndexBuffer::GetObject()
    {
        return m_buffer;
    }
}
