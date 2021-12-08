#include "RwIndexBuffer.h"
#include "D3DResourceSystem.h"
#include "IndexBufferManager.h"
RwIndexBuffer::RwIndexBuffer()
{
	m_buffer = nullptr;
}

RwIndexBuffer::~RwIndexBuffer()
{
    //D3DResourceSystem::DestroyIndexBuffer(m_buffer);
    //IndexBufferManager::RemoveFromList(this);
    Release();
}

void RwIndexBuffer::Initialize(RwUInt32 size, bool dynamic)
{
    mDynamic = dynamic;
    mSize = size;

    // auto result = _rwD3D9IndexBufferCreate(size, &m_buffer);
    HRESULT result;
    if(dynamic)
    {
        D3DPOOL     d3dPool;
        if(RwD3D9DeviceCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        {
            d3dPool = D3DPOOL_DEFAULT;
        }
        else
        {
            d3dPool = D3DPOOL_SYSTEMMEM;
        }

        result = RwD3DDevice->CreateIndexBuffer(size * sizeof(RwUInt16), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                                                D3DFMT_INDEX16, d3dPool, &m_buffer, nullptr);
    }
    else
    {
        result = RwD3DDevice->CreateIndexBuffer(size * sizeof(RwUInt16), D3DUSAGE_WRITEONLY,
                                       D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_buffer, nullptr);
    }
   
    if(FAILED(result))
        throw std::runtime_error("RwIndexBuffer::Initialize");

    IndexBufferManager::AddToList(this);
}

void RwIndexBuffer::Release()
{
    if(m_buffer)
    m_buffer->Release();
    m_buffer = nullptr;
}

void RwIndexBuffer::Restore()
{
    if(m_buffer)
        return;

    HRESULT result;
    if(mDynamic)
    {
        D3DPOOL     d3dPool;
        if(RwD3D9DeviceCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        {
            d3dPool = D3DPOOL_DEFAULT;
        }
        else
        {
            d3dPool = D3DPOOL_SYSTEMMEM;
        }
        result = RwD3DDevice->CreateIndexBuffer(mSize * sizeof(RwUInt16), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
                                                D3DFMT_INDEX16, d3dPool, &m_buffer, nullptr);
    }
    else
    {
        result = RwD3DDevice->CreateIndexBuffer(mSize * sizeof(RwUInt16), D3DUSAGE_WRITEONLY,
                                                D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_buffer, nullptr);
    }

    if(FAILED(result))
        throw std::runtime_error("RwIndexBuffer::Initialize");

}

void RwIndexBuffer::Map(RwUInt32 size, void** data)
{
    if(m_buffer == nullptr || data == nullptr)
        return;

    m_buffer->Lock(0, size, data, D3DLOCK_DISCARD);
}

void RwIndexBuffer::Unmap()
{
    if(m_buffer == nullptr)
        return;

    m_buffer->Unlock();
}

IDirect3DIndexBuffer9* RwIndexBuffer::GetBuffer()
{
	return m_buffer;
}

int RwIndexBuffer::GetSize()
{
    return mSize;
}

int RwIndexBuffer::IsDynamic()
{
    return mDynamic;
}

