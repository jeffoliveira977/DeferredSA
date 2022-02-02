#include "IndexBuffer.h"
#include "D3DResourceSystem.h"
#include "DynamicIndexBuffer.h"

namespace DeferredRenderingEngine
{
    IndexBuffer::IndexBuffer()
    {
        m_buffer = nullptr;
        mDynamic = false;
        mSize = 0;
    }

    IndexBuffer::~IndexBuffer()
    {
        SAFE_RELEASE(m_buffer);
    }

    void IndexBuffer::Initialize(uint32_t size, bool dynamic)
    {
        mDynamic = dynamic;
        mSize = size;

        HRESULT result;
        if (dynamic)
            result = RwD3DDevice->CreateIndexBuffer(size * sizeof(uint16_t), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_buffer, 0);
        else
            result = RwD3DDevice->CreateIndexBuffer(size * sizeof(uint16_t), D3DUSAGE_WRITEONLY,
                D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_buffer, 0);

        if (FAILED(result))
            throw std::runtime_error("RwIndexBuffer::Initialize");
    }

    void IndexBuffer::Release()
    {
        SAFE_RELEASE(m_buffer);
    }

    void IndexBuffer::Restore()
    {
        if (m_buffer)
            return;

        HRESULT result;
        if (mDynamic)
        {
            result = RwD3DDevice->CreateIndexBuffer(mSize * sizeof(uint16_t), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_buffer, nullptr);
        }
        else
        {
            result = RwD3DDevice->CreateIndexBuffer(mSize * sizeof(uint16_t), D3DUSAGE_WRITEONLY,
                D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_buffer, nullptr);
        }

        if (FAILED(result))
            throw std::runtime_error("RwIndexBuffer::Restore");

    }

    void IndexBuffer::Map(uint32_t size, void** data)
    {
        if (m_buffer == nullptr || data == nullptr)
            return;

        m_buffer->Lock(0, size, data, D3DLOCK_DISCARD);
    }

    void IndexBuffer::Unmap()
    {
        if (m_buffer == nullptr)
            return;

        m_buffer->Unlock();
    }

    void IndexBuffer::Apply()
    {
        _rwD3D9SetIndices(m_buffer);
    }

    IDirect3DIndexBuffer9* IndexBuffer::GetObject()
    {
        return m_buffer;
    }
}
