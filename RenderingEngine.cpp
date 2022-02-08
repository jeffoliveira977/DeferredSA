#include "RenderingEngine.h"

namespace DeferredRenderingEngine
{
    std::list<RenderingEngine::RxD3D9StrideEntry> RenderingEngine::StrideList;
    int RenderingEngine::mCurrentDynamicVertexBufferInfo;
    RenderingEngine::DVB_Manager RenderingEngine::mDynamicVertexBufferInfo[4]{};
    std::list<RenderingEngine::dVB> RenderingEngine::mDynamicVertexBufferList;
    std::list<IndexBuffer*> RenderingEngine::mDynamicIndexBufferList;

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
                //SAFE_RELEASE(buffer.vb);
            }
        }
        mDynamicVertexBufferList.clear();

        for (auto& buffer : StrideList)
        {
            if (buffer.vertexBuffer)
            {
                SAFE_DELETE(buffer.vertexBuffer);
                Log::Debug("RenderingEngine::DestroyVertexBuffer %i", buffer.vertexBuffer);
            }
        }
        StrideList.clear();
    }

    bool RenderingEngine::CreateVertexBuffer(uint32_t stride, uint32_t size, IDirect3DVertexBuffer9** vertexBuffer, uint32_t* offset)
    {

        static uint32_t         DefaultVBSize = 128 * 1024;

        auto& itri = StrideList.begin();
        for (; itri != StrideList.end(); itri++)
        {
            if (itri->stride == stride && itri->size >= size)
            {
                break;
            }
        }

        if (itri == StrideList.end())
        {
            RxD3D9StrideEntry stridelist;
            stridelist.stride = stride;
            stridelist.offset = 0;
            stridelist.size = (((DefaultVBSize + (stride - 1)) / stride) * stride);

            if (size >= stridelist.size)
            {
                stridelist.size = size;
            }

            auto buffer = new VertexBuffer(stridelist.size, 1, false);
            if (buffer == nullptr)
                return false;

            buffer->Initialize();

            stridelist.size -= size;
            stridelist.offset = size;
            stridelist.vertexBuffer = buffer;

            *vertexBuffer = buffer->GetObject();
            *offset = 0;

            StrideList.push_back(stridelist);
            Log::Debug("RenderingEngine::CreateVertexBuffer - base: size %i %i stride %i %i", stridelist.size, size, stridelist.stride, stride);
            return true;
        }
        else
        {
            Log::Debug("RenderingEngine::CreateVertexBuffer - sharing vertex buffer: size %i %i stride %i %i", itri->size, size, itri->stride, stride);
        }

        *vertexBuffer = itri->vertexBuffer->GetObject();
        *offset = itri->offset;
        auto nextOffset = (*offset) + size;

        itri->size -= nextOffset - itri->offset;
        itri->offset = nextOffset;

        return true;
    }

    void RenderingEngine::DestroyVertexBuffer(uint32_t stride, uint32_t size, IDirect3DVertexBuffer9* vertexBuffer, uint32_t offset)
    {
        if (vertexBuffer == nullptr)
            return;

        for (auto& itri = StrideList.begin(); itri != StrideList.end(); itri++)
        {
            if (itri->vertexBuffer && itri->vertexBuffer->GetObject() == vertexBuffer)
            {
                Log::Debug("RenderingEngine::DestroyVertexBuffer %i", vertexBuffer);
                SAFE_DELETE(itri->vertexBuffer);
                SAFE_RELEASE(vertexBuffer);
                itri->size = 0;
                itri->stride = 0;
              //  Log::Debug("RenderingEngine::DestroyVertexBuffer %i", vertexBuffer);
       
                StrideList.erase(itri);
                //Log::Debug("RenderingEngine::DestroyVertexBuffer %i", vertexBuffer);
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
}