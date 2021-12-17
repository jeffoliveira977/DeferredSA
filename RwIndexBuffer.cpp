#include "RwIndexBuffer.h"
#include "D3DResourceSystem.h"
#include "IndexBufferManager.h"
RwIndexBuffer::RwIndexBuffer()
{
	m_buffer = nullptr;
    mDynamic = false;
    mSize = 0;
}

RwIndexBuffer::~RwIndexBuffer()
{
    //D3DResourceSystem::DestroyIndexBuffer(m_buffer);
    //IndexBufferManager::RemoveFromList(this);
    Release();
}
struct DynamicIB
{
    uint32_t length;
    IDirect3DIndexBuffer9** buf;
    DynamicIB* next;
};
static DynamicIB* dynamicIBs;

void
addDynamicIB(uint32_t length, IDirect3DIndexBuffer9** buf)
{
    DynamicIB* ivb = new DynamicIB;
    ivb->length = length;
    ivb->buf = buf;
    ivb->next = dynamicIBs;
    dynamicIBs = ivb;
}

void
removeDynamicIB(IDirect3DIndexBuffer9** buf)
{
    DynamicIB** p, * ivb;
    for(p = &dynamicIBs; *p; p = &(*p)->next)
        if((*p)->buf == buf)
            goto found;
    return;
found:
    ivb = *p;
    *p = ivb->next;
    delete ivb;
}

void destroyIndexBuffer(void* indexBuffer)
{
    if(indexBuffer)
    {
        ((IUnknown*)indexBuffer)->Release();
    }
}

 void
releaseDynamicIBs(void)
{
    DynamicIB* ivb;
    for(ivb = dynamicIBs; ivb; ivb = ivb->next)
    {
        destroyIndexBuffer(*ivb->buf);
        *ivb->buf = 0;
    }
}

void*
createIndexBuffer(uint32_t length, bool dynamic)
{
    IDirect3DIndexBuffer9* ibuf;
    if(dynamic)
        RwD3DDevice->CreateIndexBuffer(length, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &ibuf, 0);
    else
        RwD3DDevice->CreateIndexBuffer(length, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &ibuf, 0);
    return ibuf;

}

 void
recreateDynamicIBs(void)
{
    DynamicIB* ivb;
    for(ivb = dynamicIBs; ivb; ivb = ivb->next)
    {
        assert(*ivb->buf == 0);
        *ivb->buf = (IDirect3DIndexBuffer9*)createIndexBuffer(ivb->length, true);
    }
}
void RwIndexBuffer::Initialize(RwUInt32 size, bool dynamic)
{


    //HRESULT result;
    //if(0)
    //{
    //    bool insertToList = mSize == 0 ? true : false;


    //    result = RwD3DDevice->CreateIndexBuffer(size * sizeof(RwUInt16), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
    //                                            D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_buffer, nullptr);

    //    if(insertToList)
    //        IndexBufferManager::AddToList(this);    
    //    
    //    mDynamic = dynamic;
    //    mSize = size;

    //}
    //else
    //{
    //    mDynamic = dynamic;
    //    mSize = size;
    //    result = D3DResourceSystem::CreateIndexBuffer(size, 101, (void**)&m_buffer);
    //}
    dynamic = false;
    m_buffer= (IDirect3DIndexBuffer9*)createIndexBuffer(size * sizeof(RwUInt16), dynamic);
    if(dynamic)
    addDynamicIB(size * sizeof(RwUInt16), &m_buffer);
    //if(FAILED(result))
    //    throw std::runtime_error("RwIndexBuffer::Initialize");
}

void RwIndexBuffer::Release()
{
    if(m_buffer)
    {
        m_buffer->Release();
        m_buffer = nullptr;
    }
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

