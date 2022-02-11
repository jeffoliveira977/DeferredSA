#include "RenderingEngine.h"

RwFreeList* RwFreeListCreate(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwUInt32 hint) {
    return ((RwFreeList * (__cdecl*)(RwInt32, RwInt32, RwInt32, RwUInt32))0x801980)(entrySize, entriesPerBlock, alignment, hint);
}

namespace DeferredRenderingEngine
{
    std::list<VertexBuffer*> RenderingEngine::mVertexBufferList;
    uint32_t RenderingEngine::mCurrentDynamicVertexBufferInfo;
    RenderingEngine::DVB_Manager RenderingEngine::mDynamicVertexBufferInfo[4]{};
    std::list<RenderingEngine::dVB> RenderingEngine::mDynamicVertexBufferList;
    std::list<IndexBuffer*> RenderingEngine::mDynamicIndexBufferList;
    std::vector<RenderingEngine::rxD3D9VertexDeclarationItem> RenderingEngine::m_vertexDeclarations;

    bool RenderingEngine::DynamicVertexBufferListCreate()
    {
        return false;
    }

    void RenderingEngine::DynamicVertexBufferListDestroy()
    {
    }

    void RenderingEngine::VertexBufferManagerOpen()
    {
        Log::Debug("RenderingEngine::VertexBufferManagerOpen");
        VertexBufferManagerClose();

        DynamicVertexBufferManagerCreate();
    }

    void RenderingEngine::VertexBufferManagerClose()
    {
        Log::Debug("RenderingEngine::VertexBufferManagerClose");

        DynamicVertexBufferManagerDestroy();

        for (auto& buffer : mDynamicVertexBufferList)
        {
            if (buffer.vb)
            {
                SAFE_DELETE(buffer.vb);
            }
        }
        mDynamicVertexBufferList.clear();

        for (auto& buffer : mVertexBufferList)
        {
            if (buffer)
            {
                SAFE_DELETE(buffer);
            }
        }

        mVertexBufferList.clear();
    }

    bool RenderingEngine::CreateVertexBuffer(uint32_t stride, uint32_t size, IDirect3DVertexBuffer9** vertexBuffer, uint32_t* offset)
    {
        auto buffer = new VertexBuffer(size, 1, false);
        if (buffer == nullptr)
            return false;

        buffer->Initialize();

        *vertexBuffer = buffer->GetObject();
        *offset = 0;

        mVertexBufferList.push_back(buffer);
        return true;
    }

    void RenderingEngine::DestroyVertexBuffer(uint32_t stride, uint32_t size, IDirect3DVertexBuffer9* vertexBuffer, uint32_t offset)
    {
        if (vertexBuffer == nullptr)
            return;

        for (auto& itri = mVertexBufferList.begin(); itri != mVertexBufferList.end(); itri++)
        {
            if ((*itri) && (*itri)->GetObject() == vertexBuffer)
            {
                SAFE_DELETE((*itri));
                mVertexBufferList.erase(itri);
                break;
            }
        }
        return;
    }

    bool RenderingEngine::DynamicVertexBufferManagerCreate()
    {
        Log::Debug("RenderingEngine::DynamicVertexBufferManagerCreate");

        mCurrentDynamicVertexBufferInfo = 0;
        if (RwD3D9DeviceCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        {
            for (size_t i = 0; i < 4; i++)
            {
                mDynamicVertexBufferInfo[i].offset = 0;
                mDynamicVertexBufferInfo[i].size = 0x40000;
                if (!DynamicVertexBufferCreate(0x40000, &mDynamicVertexBufferInfo[i].vertexbuffer))
                    return false;
            }
        }
        else
        {
            mDynamicVertexBufferInfo[0].offset = 0;
            mDynamicVertexBufferInfo[0].size = 0x40000;

            if (!DynamicVertexBufferCreate(0x40000u, &mDynamicVertexBufferInfo[0].vertexbuffer))
                return false;

            for (size_t i = 1; i < 4; i++)
            {
                mDynamicVertexBufferInfo[i].offset = 0;
                mDynamicVertexBufferInfo[i].size = 0;
                mDynamicVertexBufferInfo[1].vertexbuffer = nullptr;
            }
        }
        return true;
    }

    void RenderingEngine::DynamicVertexBufferManagerDestroy()
    {
        Log::Debug("RenderingEngine::DynamicVertexBufferManagerDestroy");
        mCurrentDynamicVertexBufferInfo = 0;

        for (size_t n = 0; n < 4; n++)
        {
            mDynamicVertexBufferInfo[n].offset = 0;
            mDynamicVertexBufferInfo[n].size = 0;

            if (mDynamicVertexBufferInfo[n].vertexbuffer)
            {
                DynamicVertexBufferDestroy(mDynamicVertexBufferInfo[n].vertexbuffer);

                mDynamicVertexBufferInfo[n].vertexbuffer = nullptr;
            }
        }
    }

    bool RenderingEngine::DynamicVertexBufferCreate(uint32_t size, IDirect3DVertexBuffer9** vertexBuffer)
    {
        auto buffer = new VertexBuffer(size, 1);
        if (buffer == nullptr)
            return false;

        buffer->Initialize();

        dVB vb;
        vb.size = size;
        vb.vb = buffer;
        *vertexBuffer = buffer->GetObject();

        mDynamicVertexBufferList.push_back(vb);
        Log::Debug("RenderingEngine::DynamicVertexBufferCreate - base size %i", size);

        return true;
    }

    bool RenderingEngine::DynamicVertexBufferDestroy(IDirect3DVertexBuffer9* vertexbuffer)
    {
        if (vertexbuffer == nullptr)
            return false;

        for (auto itri = mDynamicVertexBufferList.begin(); itri != mDynamicVertexBufferList.end(); itri++)
        {
            if (itri->vb && itri->vb->GetObject() == vertexbuffer)
            {
                Log::Debug("RenderingEngine::DynamicVertexBufferDestroy - %i", vertexbuffer);
                SAFE_DELETE(itri->vb);
                mDynamicVertexBufferList.erase(itri);
                break;
            }
        }

        return true;
    }

    bool RenderingEngine::DynamicIndexBufferCreate(uint32_t size, IDirect3DIndexBuffer9** indexbuffer)
    {
        auto indexBuffer = new IndexBuffer(size);
        if (indexBuffer == nullptr)
            return false;

        indexBuffer->Initialize();
        *indexbuffer = indexBuffer->GetObject();
        mDynamicIndexBufferList.push_back(indexBuffer);
        return true;
    }

    bool RenderingEngine::DynamicVertexBufferLock(uint32_t vertexSize, uint32_t numVertex,
        IDirect3DVertexBuffer9** vertexBufferOut, void** vertexDataOut, uint32_t* baseIndexOut)
    {

        if (vertexSize == 0)
        {
            Log::Error("RenderingEngine::DynamicVertexBufferLock - invalid vertexSize");
            return false;
        }

        if (numVertex == 0)
        {
            Log::Error("RenderingEngine::DynamicVertexBufferLock - invalid numVertex");
            return false;
        }

        uint32_t lockSize = numVertex * vertexSize, lockOffset = 0;

        if (RwD3D9DeviceCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
        {
            lockOffset = mDynamicVertexBufferInfo[mCurrentDynamicVertexBufferInfo].offset;
            if (lockOffset + lockSize > mDynamicVertexBufferInfo[mCurrentDynamicVertexBufferInfo].size)
            {
                for (mCurrentDynamicVertexBufferInfo++; mCurrentDynamicVertexBufferInfo < 4; mCurrentDynamicVertexBufferInfo++)
                {
                    lockOffset = mDynamicVertexBufferInfo[mCurrentDynamicVertexBufferInfo].offset;
                    if (lockOffset + lockSize <= mDynamicVertexBufferInfo[mCurrentDynamicVertexBufferInfo].size)
                        break;
                }

                if (mCurrentDynamicVertexBufferInfo >= 4)
                {
                    lockOffset = 0;
                    mCurrentDynamicVertexBufferInfo = 0;
                }
            }
        }
        if (lockSize > mDynamicVertexBufferInfo[mCurrentDynamicVertexBufferInfo].size)
        {
            DynamicVertexBufferDestroy(mDynamicVertexBufferInfo[mCurrentDynamicVertexBufferInfo].vertexbuffer);
            mDynamicVertexBufferInfo[mCurrentDynamicVertexBufferInfo].size = lockSize;
            DynamicVertexBufferCreate(lockSize, &mDynamicVertexBufferInfo[mCurrentDynamicVertexBufferInfo].vertexbuffer);
        }

        if (lockOffset)
        {
            auto hr = mDynamicVertexBufferInfo[mCurrentDynamicVertexBufferInfo].vertexbuffer->Lock(lockOffset, lockSize, vertexDataOut,
                D3DLOCK_NOSYSLOCK | D3DLOCK_NOOVERWRITE);

            if (FAILED(hr))
            {
                Log::Fatal("RenderingEngine::DynamicVertexBufferLock - failed to lock vertex buffer");
                return false;
            }

            mDynamicVertexBufferInfo[mCurrentDynamicVertexBufferInfo].offset = lockOffset + lockSize;
            *baseIndexOut = lockOffset / vertexSize;
        }
        else
        {
            auto hr = mDynamicVertexBufferInfo[mCurrentDynamicVertexBufferInfo].vertexbuffer->Lock(0, lockSize, vertexDataOut,
                D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD);

            if (FAILED(hr))
            {
                Log::Fatal("RenderingEngine::DynamicVertexBufferLock - failed to lock vertex buffer");
                return false;
            }

            mDynamicVertexBufferInfo[mCurrentDynamicVertexBufferInfo].offset = lockSize;
            *baseIndexOut = 0;
        }

        *vertexBufferOut = mDynamicVertexBufferInfo[mCurrentDynamicVertexBufferInfo].vertexbuffer;
        return true;
    }

    void RenderingEngine::DynamicVertexBufferUnlock(IDirect3DVertexBuffer9* vertexBufferOut)
    {
        if (vertexBufferOut)
            IDirect3DVertexBuffer9_Unlock(vertexBufferOut);
    }

    void RenderingEngine::DynamicVertexBufferRestore()
    {
        Log::Debug("RenderingEngine::DynamicVertexBufferRestore");

        for (auto& buffer : mDynamicVertexBufferList)
        {
            if (buffer.vb)
            {
                buffer.vb->Initialize();
                Log::Debug("RenderingEngine::DynamicVertexBufferRestore - created vertex buffer with base size, %i", buffer.size);
            }
        }

        DynamicVertexBufferManagerCreate();
    }

    void RenderingEngine::DynamicVertexBufferRelease()
    {
        Log::Debug("RenderingEngine::DynamicVertexBufferRelease");

        DynamicVertexBufferManagerDestroy();

        for (auto& buffer : mDynamicVertexBufferList)
        {
            if (buffer.vb)
            {
                buffer.vb->Deinitialize();
                Log::Debug("RenderingEngine::DynamicVertexBufferRelease - %i", buffer.vb);
            }
        }
    }

    bool RenderingEngine::CreateVertexDeclaration(D3DVERTEXELEMENT9* elements, IDirect3DVertexDeclaration9** vertexdeclaration)
    {
        uint32_t numelements = 0;
        while (elements[numelements].Type != D3DDECLTYPE_UNUSED)
        {
            numelements++;
        }
        numelements++;

        uint32_t sizeElements = numelements * sizeof(D3DVERTEXELEMENT9);
        uint32_t n = 0;
        for (size_t i = 0; i < m_vertexDeclarations.size(); i++)
        {
            if (m_vertexDeclarations[i].numElements == numelements)
            {
                if (std::memcmp(m_vertexDeclarations[i].elements, elements, sizeElements) == 0)
                {
                    n = i;
                    break;
                }
            }
        }

        if (n == 0)
        {
            auto hr = IDirect3DDevice9_CreateVertexDeclaration(RwD3DDevice, elements, vertexdeclaration);

            if (SUCCEEDED(hr))
            {
                rxD3D9VertexDeclarationItem item;
                item.vertexdeclaration = *vertexdeclaration;
                item.numElements = numelements;
                item.elements = new D3DVERTEXELEMENT9[sizeElements];
                memcpy(item.elements, elements, sizeElements);

                m_vertexDeclarations.push_back(item);
            }
            else
            {
                *vertexdeclaration = nullptr;
                return false;
            }
            Log::Debug("RenderingEngine::CreateVertexDeclaration - created %i", vertexdeclaration);

        }
        else
        {

            IDirect3DVertexDeclaration9_AddRef(m_vertexDeclarations[n].vertexdeclaration);
            *vertexdeclaration = m_vertexDeclarations[n].vertexdeclaration;

            Log::Debug("RenderingEngine::CreateVertexDeclaration - sharing %i", vertexdeclaration);

        }

        return true;
    }

    void RenderingEngine::DeleteVertexDeclaration(IDirect3DVertexDeclaration9* vertexdeclaration)
    {
        for (uint32_t n = 0; n < m_vertexDeclarations.size(); n++)
        {
            if (m_vertexDeclarations[n].vertexdeclaration == vertexdeclaration)
            {
                Log::Debug("RenderingEngine::DeleteVertexDeclaration - %i", vertexdeclaration);
                SAFE_RELEASE(m_vertexDeclarations[n].vertexdeclaration);
                SAFE_DELETE(m_vertexDeclarations[n].elements);
                m_vertexDeclarations.erase(m_vertexDeclarations.begin() + n);
                break;
            }
        }
    }

    bool RenderingEngine::SetRenderTarget(uint32_t index, IDirect3DSurface9* rendertarget)
    {
        if (mCurrentRenderSurface[index] != rendertarget)
        {
            mCurrentRenderSurface[index] = rendertarget;

            auto hr = IDirect3DDevice9_SetRenderTarget(RwD3DDevice, index, rendertarget);

            return SUCCEEDED(hr);
        }
        return true;
    }

    bool RenderingEngine::SetDepthStencilSurface(IDirect3DSurface9* depthStencilSurface)
    {
        if (mCurrentDepthStencilSurface != depthStencilSurface)
        {
            mCurrentDepthStencilSurface = depthStencilSurface;
            auto hr = RwD3DDevice->SetDepthStencilSurface(depthStencilSurface);

            return SUCCEEDED(hr);
        }

        return true;
    }

    bool RenderingEngine::CreateVertexShader(DWORD* function, IDirect3DVertexShader9** shader)
    {
        auto hr = RwD3DDevice->CreateVertexShader(function, shader);

        if (SUCCEEDED(hr))
        {
            mLastVertexShaderUsed = (void*)0xffffffff;
        }

        return SUCCEEDED(hr);
    }

    void RenderingEngine::DeleteVertexShader(IDirect3DVertexShader9* shader)
    {
        SAFE_RELEASE(shader);
    }

    bool RenderingEngine::CreatePixelShader(DWORD* function, IDirect3DPixelShader9** shader)
    {
        auto hr = RwD3DDevice->CreatePixelShader(function, shader);

        if (SUCCEEDED(hr))
        {
            mLastPixelShaderUsed = (void*)0xffffffff;
        }

        return SUCCEEDED(hr);
    }

    void RenderingEngine::DeletePixelShader(IDirect3DPixelShader9* shader)
    {
        SAFE_RELEASE(shader);
    }

    void RenderingEngine::SetStreamSource(uint32_t streamNumber, IDirect3DVertexBuffer9* streamData, uint32_t offset, uint32_t stride)
    {
        if (mLastVertexStreamUsed[streamNumber].vertexBuffer != streamData ||
            mLastVertexStreamUsed[streamNumber].offset != offset ||
            mLastVertexStreamUsed[streamNumber].stride != stride)
        {
            mLastVertexStreamUsed[streamNumber].vertexBuffer = streamData;
            mLastVertexStreamUsed[streamNumber].offset = offset;
            mLastVertexStreamUsed[streamNumber].stride = stride;

            RwD3DDevice->SetStreamSource(streamNumber, streamData, offset, stride);
        }
    }

    void DeferredRenderingEngine::RenderingEngine::SetStreams(RxD3D9VertexStream* streams, bool useOffsets)
    {
        if (useOffsets == false)
        {
            for (uint32_t n = 0; n < RWD3D9_MAX_VERTEX_STREAMS; n++)
            {
                auto vertexBuffer = reinterpret_cast<IDirect3DVertexBuffer9*>(streams[n].vertexBuffer);

                if (vertexBuffer != nullptr)
                {
                    if (mLastVertexStreamUsed[n].vertexBuffer != vertexBuffer ||
                        mLastVertexStreamUsed[n].offset != 0 ||
                        mLastVertexStreamUsed[n].stride != streams[n].stride)
                    {
                        mLastVertexStreamUsed[n].vertexBuffer = vertexBuffer;
                        mLastVertexStreamUsed[n].offset = 0;
                        mLastVertexStreamUsed[n].stride = streams[n].stride;

                        RwD3DDevice->SetStreamSource(n, vertexBuffer, 0, streams[n].stride);
                    }
                }
                else
                {
                    if (mLastVertexStreamUsed[n].vertexBuffer != nullptr)
                    {
                        mLastVertexStreamUsed[n].vertexBuffer = nullptr;
                        mLastVertexStreamUsed[n].offset = 0;
                        mLastVertexStreamUsed[n].stride = 0;

                        RwD3DDevice->SetStreamSource(n, nullptr, 0, 0);
                    }
                }
            }
        }
        else
        {
            for (uint32_t n = 0; n < RWD3D9_MAX_VERTEX_STREAMS; n++)
            {
                auto vertexBuffer = reinterpret_cast<IDirect3DVertexBuffer9*>(streams[n].vertexBuffer);

                if (vertexBuffer != nullptr)
                {
                    if (mLastVertexStreamUsed[n].vertexBuffer != vertexBuffer ||
                        mLastVertexStreamUsed[n].offset != streams[n].offset ||
                        mLastVertexStreamUsed[n].stride != streams[n].stride)
                    {
                        mLastVertexStreamUsed[n].vertexBuffer = vertexBuffer;
                        mLastVertexStreamUsed[n].offset = streams[n].offset;
                        mLastVertexStreamUsed[n].stride = streams[n].stride;

                        RwD3DDevice->SetStreamSource(n, vertexBuffer, streams[n].offset, streams[n].stride);
                    }
                    else
                    {
                        if (mLastVertexStreamUsed[n].vertexBuffer != nullptr)
                        {
                            mLastVertexStreamUsed[n].vertexBuffer = nullptr;
                            mLastVertexStreamUsed[n].offset = 0;
                            mLastVertexStreamUsed[n].stride = 0;

                            RwD3DDevice->SetStreamSource(n, nullptr, 0, 0);
                        }
                    }
                }

            }
        }
    }

    void RenderingEngine::SetIndices(IDirect3DIndexBuffer9* indexBuffer)
    {
        if (mLastIndexBufferUsed != indexBuffer)
        {
            mLastIndexBufferUsed = indexBuffer;

            RwD3DDevice->SetIndices(indexBuffer);
        }
    }

    void RenderingEngine::RenderStateFlushCache()
    {
        if (mNumRenderStateChanges)
        {
            for (size_t i = 0; i < mNumRenderStateChanges; i++)
            {
                auto state = mRenderStateChanges[i];
                auto value = mRenderStateCache[state].value;
                mRenderStateCache[state].changed = false;

                if (mRenderStateD3D[state] != value)
                {
                    mRenderStateD3D[state] = value;

                    RwD3DDevice->SetRenderState(state, value);
                }

                mNumRenderStateChanges = 0;
            }
        }

        if (mNumTextureStageStateChanges)
        {
            for (size_t i = 0; i < mNumRenderStateChanges; i++)
            {
                auto stage = mTextureStageStateChanges[i].stage;
                auto type = mTextureStageStateChanges[i].type;
                auto value = mTextureStageStateCache[stage][type].value;
                mTextureStageStateCache[stage][type].changed = false;

                if (mTextureStageStateD3D[stage][type] != value)
                {
                    mTextureStageStateD3D[stage][type] = value;

                    RwD3DDevice->SetTextureStageState(stage, type, value);
                }
            }

            mNumTextureStageStateChanges = 0;
        }
    }

    void RenderingEngine::GetRenderState(uint32_t state, void* value)
    {
        *reinterpret_cast<uint32_t*>(value) = mRenderStateCache[state].value;
    }

    void RenderingEngine::SetTextureStageState(uint32_t stage, uint32_t type, uint32_t value)
    {
        if (mTextureStageStateCache[stage][type].value != value)
        {
            mTextureStageStateCache[stage][type].value = value;

            if (!mTextureStageStateCache[stage][type].changed)
            {
                mTextureStageStateCache[stage][type].changed = true;
                mTextureStageStateChanges[mNumTextureStageStateChanges].stage = stage;
                mTextureStageStateChanges[mNumTextureStageStateChanges].type = (D3DTEXTURESTAGESTATETYPE)type;
                mNumTextureStageStateChanges++;
            }
        }
    }

    void RenderingEngine::GetTextureStageState(uint32_t stage, uint32_t type, void* value)
    {
        *reinterpret_cast<uint32_t*>(value) = mTextureStageStateCache[stage][type].value;
    }

    void RenderingEngine::SetSamplerState(uint32_t stage, uint32_t type, uint32_t value)
    {
        if (mSamplerStageStateD3D[stage][type] != value)
        {
            mSamplerStageStateD3D[stage][type] = value;

            RwD3DDevice->SetSamplerState(stage, (D3DSAMPLERSTATETYPE)type, value);
        }
    }

    void RenderingEngine::GetSamplerState(uint32_t stage, uint32_t type, void* value)
    {
        *reinterpret_cast<uint32_t*>(value) = mSamplerStageStateD3D[stage][type];
    }

    void RenderingEngine::ForceSamplerState(uint32_t stage, uint32_t type, uint32_t value)
    {
        mSamplerStageStateD3D[stage][type] = value;

        RwD3DDevice->SetSamplerState(stage, (D3DSAMPLERSTATETYPE)type, value);
    }

    void RenderingEngine::ForceRenderState(uint32_t state, uint32_t value)
    {
        mRenderStateCache[state].value = value;
        mRenderStateD3D[state] = value;

        RwD3DDevice->SetRenderState((D3DRENDERSTATETYPE)state, value);
    }

    void RenderingEngine::ForceTextureStageState(uint32_t stage, uint32_t type, uint32_t value)
    {
        mTextureStageStateCache[stage][type].value = value;
        mTextureStageStateD3D[stage][type] = value;

        RwD3DDevice->SetTextureStageState(stage, (D3DTEXTURESTAGESTATETYPE)type, value);
    }
}