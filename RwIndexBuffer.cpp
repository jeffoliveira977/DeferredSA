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

void RwIndexBuffer::Initialize(RwUInt32 size)
{
	auto result =_rwD3D9IndexBufferCreate(size, &m_buffer);  
    if(!result)
        throw std::runtime_error("RwIndexBuffer::Initialize");
}

void RwIndexBuffer::Copy(RwUInt32 size, void* in)
{
    if(m_buffer == nullptr ||
       in == nullptr)
        return;

    void* bufferMem = NULL;
    m_buffer->Lock(0, size, &bufferMem, D3DLOCK_DISCARD);
    memcpy(bufferMem, in, size);
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
