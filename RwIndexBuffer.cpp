#include "RwIndexBuffer.h"
#include "D3DResourceSystem.h"

RwIndexBuffer::RwIndexBuffer()
{
	m_buffer = nullptr;
}

RwIndexBuffer::~RwIndexBuffer()
{
    D3DResourceSystem::DestroyIndexBuffer(m_buffer);
}

void RwIndexBuffer::Allocate(RwUInt32 size)
{
	auto result =_rwD3D9IndexBufferCreate(size, &m_buffer);  
    if(!result)
        throw std::runtime_error("RwIndexBuffer::Allocate");
}

void RwIndexBuffer::Copy(RwUInt32 size, void* in, void* out)
{
    if(m_buffer == nullptr ||
       in == nullptr ||
       out == nullptr)
        return;

    m_buffer->Lock(0, size, &out, D3DLOCK_DISCARD);
    memcpy(out, in, size);
    m_buffer->Unlock();
}

void RwIndexBuffer::Set()
{
    _rwD3D9SetIndices(m_buffer);
}

IDirect3DIndexBuffer9* RwIndexBuffer::GetBuffer()
{
	return m_buffer;
}
