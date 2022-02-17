#include "RenderingEngine.h"
#include "GTADef.h"

RwFreeList* RwFreeListCreate(int32_t entrySize, int32_t entriesPerBlock, int32_t alignment, uint32_t hint) {
    return ((RwFreeList * (__cdecl*)(int32_t, int32_t, int32_t, uint32_t))0x801980)(entrySize, entriesPerBlock, alignment, hint);
}


RwBool _rwD3D9RWSetRenderState(RwRenderState state, void* param)
{
    return ((RwBool  (__cdecl*)(RwRenderState, void*))0x007FE420)(state, param);
}

RwBool _rwD3D9RWGetRenderState(RwRenderState state, void* param)
{
    return ((RwBool(__cdecl*)(RwRenderState, void*))0x007FD810)(state, param);
}

RwBool  _rwD3D9Im2DRenderLine(RwIm2DVertex* verts, RwInt32 numVerts,
    RwInt32 vert1, RwInt32 vert2)
{
    return ((RwBool(__cdecl*)(RwIm2DVertex*, RwInt32, RwInt32, RwInt32))0x007FB650)(verts, numVerts, vert1, vert2);
}

RwBool  _rwD3D9Im2DRenderTriangle(RwIm2DVertex* verts, RwInt32 numVerts,
    RwInt32 vert1, RwInt32 vert2, RwInt32 vert3)
{
    return ((RwBool(__cdecl*)(RwIm2DVertex*, RwInt32, RwInt32, RwInt32, RwInt32))0x007FB8E0)(verts, numVerts, vert1, vert2, vert3);
}

RwBool  _rwD3D9Im2DRenderPrimitive(RwPrimitiveType primType, RwIm2DVertex* verts, RwInt32 numVerts)
{
    return ((RwBool(__cdecl*)(RwPrimitiveType, RwIm2DVertex*, RwInt32))0x007FBB00)(primType, verts, numVerts);
}

RwBool  _rwD3D9Im2DRenderIndexedPrimitive(RwPrimitiveType primType, RwIm2DVertex* verts, RwInt32 numVerts,
    RwImVertexIndex* indices, RwInt32 numIndices)
{
    return ((RwBool(__cdecl*)(RwPrimitiveType, RwIm2DVertex*, RwInt32, RwImVertexIndex*, RwInt32))0x007FBD30)(primType, verts, numVerts, indices, numIndices);
}

RwBool _rwVectorSetMultFn(rwVectorMultFn multPoint, rwVectorsMultFn multPoints, rwVectorMultFn multVector,  rwVectorsMultFn multVectors)
{
    return ((RwBool  (__cdecl*)(rwVectorMultFn, rwVectorsMultFn, rwVectorMultFn, rwVectorsMultFn))0x007ED5F0)(multPoint, multPoints, multVector, multVectors);
}

RwV3d*_rwIntelSSEP4V3dTransformPoint(RwV3d* pointOut, const RwV3d* pointIn, const RwMatrix* matrix)
{
    return ((RwV3d*(__cdecl*)(RwV3d*, const RwV3d*, const RwMatrix*))0x0080D570)(pointOut, pointIn, matrix);
}

RwV3d*_rwIntelSSEP4V3dTransformPoints(RwV3d* pointsOut, const RwV3d* pointsIn, RwInt32 numPoints, const RwMatrix* matrix)
{
    return ((RwV3d * (__cdecl*)(RwV3d*, const RwV3d*, RwInt32, const RwMatrix*))0x0080D440)(pointsOut, pointsIn, numPoints, matrix);
}

RwV3d*_rwIntelSSEP4V3dTransformVector(RwV3d* vectorOut,  const RwV3d* vectorIn,  const RwMatrix* matrix)
{
    return ((RwV3d * (__cdecl*)(RwV3d*, const RwV3d*, const RwMatrix*))0x0080D600)(vectorOut, vectorIn, matrix);
}

RwV3d*
_rwIntelSSEP4V3dTransformVectors(RwV3d* vectorsOut,  const RwV3d* vectorsIn,  RwInt32        numVectors,  const RwMatrix* matrix)
{
    return ((RwV3d * (__cdecl*)(RwV3d*, const RwV3d*, RwInt32, const RwMatrix*))0x0080D4E0)(vectorsOut, vectorsIn, numVectors, matrix);
}

RwV3d*_rwIntelSSEV3dTransformPoint(RwV3d* pointOut, const RwV3d* pointIn, const RwMatrix* matrix)
{
    return ((RwV3d * (__cdecl*)(RwV3d*, const RwV3d*, const RwMatrix*))0x0080D280)(pointOut, pointIn, matrix);
}

RwV3d*
_rwIntelSSEV3dTransformPoints(RwV3d* pointsOut, const RwV3d* pointsIn, RwInt32        numPoints, const RwMatrix* matrix)
{
    return ((RwV3d * (__cdecl*)(RwV3d*, const RwV3d*, RwInt32, const RwMatrix*))0x0080D070)(pointsOut, pointsIn, numPoints, matrix);
}

RwV3d*_rwIntelSSEV3dTransformVector(RwV3d* vectorOut,const RwV3d* vectorIn, const RwMatrix* matrix)
{
    return ((RwV3d * (__cdecl*)(RwV3d*, const RwV3d*, const RwMatrix*))0x0080D370)(vectorOut, vectorIn, matrix);
}

RwV3d*
_rwIntelSSEV3dTransformVectors(RwV3d* vectorsOut, const RwV3d* vectorsIn, RwInt32        numVectors,const RwMatrix* matrix)
{
    return ((RwV3d * (__cdecl*)(RwV3d*, const RwV3d*, RwInt32, const RwMatrix*))0x0080D180)(vectorsOut, vectorsIn, numVectors, matrix);
}


#define D3D9DeviceSystemStandards(out, inOut, numStandardsFunctions) ((signed int (__cdecl*) (void*, void*,RwInt32))0x007F72E0)(out,inOut,numStandardsFunctions)

#define _rwCPUHaveMMX() ((signed int (__cdecl*) ())0x80DBE0)()
#define _rwCPUHaveSSE() ((signed int (__cdecl*) ())0x80DC40)()
#define _rwCPUHaveSSE2() ((signed int (__cdecl*) ())0x80DCA0)()
#define _rwCPUHave3DNow() ((signed int (__cdecl*) ())0x80DD00)()
#define _rwProcessorInitialize() ((void (__cdecl*)())0x00857404)()

#define D3D9CalculateMaxMultisamplingLevels() ((void (__cdecl*)())0x007F6BF0)()

#define _rwD3D9RasterClose() ((void (__cdecl*)())0x004CC170)()
#define _rwD3D9VertexBufferManagerClose() ((void (__cdecl*)())0x007F5D20)()
#define _rwD3D9Im2DRenderClose() ((void (__cdecl*)())0x007FB5F0)()
#define _rwD3D9Im3DRenderClose() ((void (__cdecl*)())0x0080DFB0 )()
#define _rwD3D9DynamicVertexBufferRelease() ((void (__cdecl*)())0x007F5840 )()
#define _rxD3D9VideoMemoryRasterListRelease() ((void (__cdecl*)())0x004CB640 )()
#define RwFreeListPurgeAllFreeLists() ((void (__cdecl*)())0x00801F90 )()

#define _rwD3D9RasterOpen() ((void (__cdecl*)())0x004CC150)()
#define _rwD3D9RenderStateOpen() ((void (__cdecl*)())0x007FCAC0)()
#define _rwD3D9VertexBufferManagerOpen() ((signed int (__cdecl*)())0x007F5CB0)()
#define _rwProcessorRelease() ((void (__cdecl*)())0x0085742B)()

#define D3D9DeviceReleaseVideoMemory() ((signed int (__cdecl*) ())0x7F7F70)()
#define InitVideoMemoryRasters() ((signed int (__cdecl*)())0x4CC970)()
#define rwD3D9DynamicVertexBufferRestore() ((signed int (__cdecl*)())0x7F58D0)()
#define rwD3D9RenderStateReset() ((int (__cdecl*)())0x7FD100)()
#define rwD3D9Im2DRenderOpen() ((signed int (__cdecl*) ())0x7FB480)()
#define rwD3D9Im3DRenderOpen() ((signed int (__cdecl*) ())0x80E020)()
//#define D3D9RestoreDeviceCallback ((int (*)(void))0xC980B0)
#define RenderStatesReset() ((int (__cdecl*)())0x700E00)()
#define RenderStatesSave() ((int (__cdecl*)())0x700CC0)()
#define _rwProcessorForceSinglePrecision() ((void (__cdecl*)())0x857432)()
#define _rwD3D9DynamicVertexBufferManagerForceDiscard() ((void (__cdecl*)())0x7F5800)() 
#define RWRETURN(value) return(value)
#define RWRETURNVOID() return



bool bWindowIconic;
bool bChangeVideo;

struct rwD3D9AdapterInformation
{
    RwChar          name[MAX_DEVICE_IDENTIFIER_STRING];
    int32_t         modeCount;
    D3DDISPLAYMODE  mode;
    int32_t         displayDepth;
    int32_t         flags;
};

 static D3DPRESENT_PARAMETERS& Present = *(D3DPRESENT_PARAMETERS*)0xC9C040;
rwD3D9AdapterInformation& _RwD3D9AdapterInformation = *(rwD3D9AdapterInformation*)0x00C9BCE0;
uint32_t& _RwD3DAdapterIndex = *(uint32_t*)0xC97C24;
HWND* WindowHandle = (HWND*)0xC97C1C;
IDirect3D9*& _RwDirect3DObject = *(IDirect3D9**)0xC97C20;
LPDIRECT3DDEVICE9& _RwD3DDevice = *(IDirect3DDevice9**)0xC97C28;

#undef RwD3D9RenderSurface
#undef RwD3D9DepthStencilSurface
#undef RwD3D9D3D9ProjTransform
#undef RwD3D9D3D9ViewTransform
#undef CurrentDepthStencilSurface
#undef CurrentRenderSurface
IDirect3DSurface9*& RwD3D9RenderSurface = (*(IDirect3DSurface9**)0xC97C30);
IDirect3DSurface9*& RwD3D9DepthStencilSurface = (*(IDirect3DSurface9**)0xC97C2C);
D3DMATRIX& _RwD3D9D3D9ViewTransform = (*(D3DMATRIX*)0xC9BC80);
D3DMATRIX& _RwD3D9D3D9ProjTransform = (*(D3DMATRIX*)0x8E2458);
RwCamera*& dgGGlobals_lastCam = *(RwCamera**)0xC9BCC0;
RwBool& NeedToCopyFromBackBuffer = *(RwBool*)0xC97C34;
RwBool&     InsideScene = *(RwBool*)0xC97C54;
D3DDEVTYPE&                   _RwD3DAdapterType = *(D3DDEVTYPE*)0X8E2428;
RwUInt32& SelectedMultisamplingLevels = *(RwUInt32*)0x008E2430;
RwUInt32& SelectedMultisamplingLevelsNonMask = *(RwUInt32*)0x008E2438;

RwInt32& _RwD3D9ZBufferDepth = *(RwInt32*)0x00C9BEFC;
RwUInt32& FullScreenRefreshRateInHz = *(RwUInt32*)0x8E243C;
RwBool& EnableMultithreadSafe = *(RwBool*)0x00C97C4C;
RwBool& EnableSoftwareVertexProcessing = *(RwBool*)0xC97C50;

RwBool& _rwD3D9CPUSupportsMMX = *(RwBool*)0xC980A0;
RwBool& _rwD3D9CPUSupportsSSE = *(RwBool*)0xC980A4;
RwBool& _rwD3D9CPUSupportsSSE2 = *(RwBool*)0xC980A8;
RwBool& _rwD3D9CPUSupports3DNow = *(RwBool*)0xC980AC;
RwInt32& NumDisplayModes = *(RwInt32*)0x00C97C40;

RwInt32& _RwD3D9CurrentModeIndex = *(RwInt32*)0xC97C18;
D3DDISPLAYMODE& DesktopDisplayMode = *(D3DDISPLAYMODE*)0xC97BC8;
D3DCAPS9& _RwD3D9DeviceCaps = *(D3DCAPS9*)0xC9BF00;

LPSURFACE& CurrentDepthStencilSurface = *(LPSURFACE*)0xC9808C;
LPSURFACE(&CurrentRenderSurface)[4] = *(LPSURFACE(*)[4])0x0C98090;

RwUInt32& _rwD3D9LastFVFUsed = *(RwUInt32*)0x8E2440;
void*& _rwD3D9LastVertexDeclarationUsed = *(void**)0x8E2444;
void*& _rwD3D9LastVertexShaderUsed = *(void**)0x8E2448;
void*& _rwD3D9LastPixelShaderUsed = *(void**)0x8E244C;
void*& _rwD3D9LastIndexBufferUsed = *(void**)0x8E2450;

typedef D3DMATRIX* LPD3DMATRIX;
RwBool& LastWorldMatrixUsedIdentity = *(RwBool*)0xC97C68;
LPD3DMATRIX(&LastMatrixUsed)[260] = *(LPD3DMATRIX(*)[260])0xC97C70;
RwFreeList*& MatrixFreeList = *(RwFreeList**)0xC98080;

D3DMATRIX& IdentityMatrix = *(D3DMATRIX*)0x8847A0;
typedef struct RxD3D9Light RxD3D9Light;
struct RxD3D9Light
{
    D3DLIGHT9   light;
    RwBool      enabled;
};

RwInt32& MaxNumLights = *(RwInt32*)0xC98084;
RxD3D9Light*& LightsCache = *(RxD3D9Light**)0xC98088;

RwBool& SystemStarted = *(RwBool*)0xC97C38;
RwUInt32& NumVertexDeclarations = *(RwUInt32*)0xC97C58;
RwUInt32& MaxVertexDeclarations = *(RwUInt32*)0xC97C5C;

typedef struct rxD3D9VertexDeclarationItem rxD3D9VertexDeclarationItem;
struct rxD3D9VertexDeclarationItem
 {
     LPDIRECT3DVERTEXDECLARATION9 vertexdeclaration;
     RwUInt32 numElements;
     D3DVERTEXELEMENT9* elements;
 };
rxD3D9VertexDeclarationItem*& VertexDeclarations = *(rxD3D9VertexDeclarationItem**)0xC97C60;

#define MAX_STREAMS  4

RxD3D9VertexStream(&LastVertexStreamUsed)[MAX_STREAMS] = *(RxD3D9VertexStream(*)[MAX_STREAMS])0x00C97BD8;

typedef struct rxD3D9DisplayMode rxD3D9DisplayMode;
struct rxD3D9DisplayMode
{
    D3DDISPLAYMODE      mode;
    RwInt32             flags;
};

rxD3D9DisplayMode*& DisplayModes = *(rxD3D9DisplayMode**)0XC97C48; /* Just used during the selection process */

#define MAX_BACKBUFFER_FORMATS  3
const D3DFORMAT BackBufferFormats[MAX_BACKBUFFER_FORMATS] =
{
    D3DFMT_R5G6B5,
    D3DFMT_X8R8G8B8,
    D3DFMT_A2R10G10B10
};

typedef struct
{
    uint32_t value;
    bool changed;
} RxStateCache;
RxStateCache(&RenderStateCache)[210] = *(RxStateCache(*)[210])0x00C991D0;
uint32_t(&RenderStateD3D)[210] = *(uint32_t(*)[210])0x00C98E88;

uint32_t& NumRenderStateChanges = *(uint32_t*)0xC9A5EC;
D3DRENDERSTATETYPE(&RenderStateChanges)[210] = *(D3DRENDERSTATETYPE(*)[210])0x00C98B40;

RxStateCache(&TextureStageStateCache)[RWD3D9_MAX_TEXTURE_STAGES][33] = *(RxStateCache(*)[RWD3D9_MAX_TEXTURE_STAGES][33])0x00C980F8;
uint32_t(&TextureStageStateD3D)[RWD3D9_MAX_TEXTURE_STAGES][33] = *(uint32_t(*)[RWD3D9_MAX_TEXTURE_STAGES][33])0x00C99860;

typedef struct
{
    uint32_t stage;
    D3DTEXTURESTAGESTATETYPE type;
} RxTextureStageChangeType;


uint32_t& NumTextureStageStateChanges = *(uint32_t*)0xC9A5F0;
RxTextureStageChangeType(&TextureStageStateChanges)[RWD3D9_MAX_TEXTURE_STAGES* 33] = *(RxTextureStageChangeType(*)[RWD3D9_MAX_TEXTURE_STAGES* 33])0x00C99C80;
uint32_t(&SamplerStageStateD3D)[RWD3D9_MAX_TEXTURE_STAGES][14] = *(uint32_t(*)[RWD3D9_MAX_TEXTURE_STAGES][14])0x00C98938;

struct RwRwDeviceGlobals
{
    RwCamera* curCamera;
    RwMemoryFunctions* memFuncs;
};

RwRwDeviceGlobals& dgGGlobals = *(RwRwDeviceGlobals*)0xC9BCC0;

int32_t D3D9FindDepth(D3DFORMAT format)
{
    int32_t             depth;
    switch (format)
    {
    case D3DFMT_A2R10G10B10:
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
        depth = 32;
        break;

    case D3DFMT_R5G6B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_X1R5G5B5:
        depth = 16;
        break;

    default:
        depth = 0;
        break;
    }

    return depth;
}

RwInt32 rwD3D9FindRWFormat(D3DFORMAT format)
{
    RwInt32 rwformat;


    switch (format)
    {
    case D3DFMT_A2R10G10B10:
        rwformat = rwRASTERFORMAT8888;
        break;

    case D3DFMT_A8R8G8B8:
        rwformat = rwRASTERFORMAT8888;
        break;

    case D3DFMT_X8R8G8B8:
        rwformat = rwRASTERFORMAT888;
        break;

    case D3DFMT_R5G6B5:
        rwformat = rwRASTERFORMAT565;
        break;

    case D3DFMT_A1R5G5B5:
        rwformat = rwRASTERFORMAT1555;
        break;

    case D3DFMT_X1R5G5B5:
        rwformat = rwRASTERFORMAT555;
        break;

    default:
        rwformat = 0;
        break;
    }

    RWRETURN(rwformat);
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

    bool RenderingEngine::SetRenderTarget(uint32_t index, RwRaster* raster)
    {
        IDirect3DSurface9* surface = nullptr;

        if (raster != nullptr)
        {
            auto rasExt = RASTEREXTFROMCONSTRASTER(raster->parent);
            if (rasExt->cube)
            {
                reinterpret_cast<IDirect3DCubeTexture9*>(rasExt->texture)->GetCubeMapSurface((D3DCUBEMAP_FACES)rasExt->face, 0, &surface);
            }
            else
            {
                IDirect3DTexture9_GetSurfaceLevel(rasExt->texture, 0, &surface);
            }
        }

        auto rv = _SetRenderTarget(index, surface);
        SAFE_RELEASE(surface);

        return rv;
    }

    bool RenderingEngine::_SetRenderTarget(uint32_t index, IDirect3DSurface9* rendertarget)
    {
        if (CurrentRenderSurface[index] != rendertarget)
        {
            CurrentRenderSurface[index] = rendertarget;

            return SUCCEEDED(IDirect3DDevice9_SetRenderTarget(RwD3DDevice, index, rendertarget));
        }
        return true;
    }

    bool RenderingEngine::SetDepthStencilSurface(IDirect3DSurface9* depthStencilSurface)
    {
        if (CurrentDepthStencilSurface != depthStencilSurface)
        {
            CurrentDepthStencilSurface = depthStencilSurface;

            return SUCCEEDED(RwD3DDevice->SetDepthStencilSurface(depthStencilSurface));
        }

        return true;
    }

    IDirect3DSurface9* RenderingEngine::GetCurrentD3DRenderTarget(uint32_t index)
    {
        return CurrentRenderSurface[index];
    }

    bool RenderingEngine::CreateVertexShader(DWORD* function, IDirect3DVertexShader9** shader)
    {
        auto hr = RwD3DDevice->CreateVertexShader(function, shader);

        if (SUCCEEDED(hr))
        {
            _rwD3D9LastVertexShaderUsed = (void*)0xffffffff;
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
            _rwD3D9LastPixelShaderUsed = (void*)0xffffffff;
        }

        return SUCCEEDED(hr);
    }

    void RenderingEngine::DeletePixelShader(IDirect3DPixelShader9* shader)
    {
        SAFE_RELEASE(shader);
    }

    void RenderingEngine::SetStreamSource(uint32_t streamNumber, IDirect3DVertexBuffer9* streamData, uint32_t offset, uint32_t stride)
    {
        if (LastVertexStreamUsed[streamNumber].vertexBuffer != streamData ||
            LastVertexStreamUsed[streamNumber].offset != offset ||
            LastVertexStreamUsed[streamNumber].stride != stride)
        {
            LastVertexStreamUsed[streamNumber].vertexBuffer = streamData;
            LastVertexStreamUsed[streamNumber].offset = offset;
            LastVertexStreamUsed[streamNumber].stride = stride;

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
                    if (LastVertexStreamUsed[n].vertexBuffer != vertexBuffer ||
                        LastVertexStreamUsed[n].offset != 0 ||
                        LastVertexStreamUsed[n].stride != streams[n].stride)
                    {
                        LastVertexStreamUsed[n].vertexBuffer = vertexBuffer;
                        LastVertexStreamUsed[n].offset = 0;
                        LastVertexStreamUsed[n].stride = streams[n].stride;

                        RwD3DDevice->SetStreamSource(n, vertexBuffer, 0, streams[n].stride);
                    }
                }
                else
                {
                    if (LastVertexStreamUsed[n].vertexBuffer != nullptr)
                    {
                        LastVertexStreamUsed[n].vertexBuffer = nullptr;
                        LastVertexStreamUsed[n].offset = 0;
                        LastVertexStreamUsed[n].stride = 0;

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
                    if (LastVertexStreamUsed[n].vertexBuffer != vertexBuffer ||
                        LastVertexStreamUsed[n].offset != streams[n].offset ||
                        LastVertexStreamUsed[n].stride != streams[n].stride)
                    {
                        LastVertexStreamUsed[n].vertexBuffer = vertexBuffer;
                        LastVertexStreamUsed[n].offset = streams[n].offset;
                        LastVertexStreamUsed[n].stride = streams[n].stride;

                        RwD3DDevice->SetStreamSource(n, vertexBuffer, streams[n].offset, streams[n].stride);
                    }
                    else
                    {
                        if (LastVertexStreamUsed[n].vertexBuffer != nullptr)
                        {
                            LastVertexStreamUsed[n].vertexBuffer = nullptr;
                            LastVertexStreamUsed[n].offset = 0;
                            LastVertexStreamUsed[n].stride = 0;

                            RwD3DDevice->SetStreamSource(n, nullptr, 0, 0);
                        }
                    }
                }

            }
        }
    }

    void RenderingEngine::SetIndices(IDirect3DIndexBuffer9* indexBuffer)
    {
        if (_rwD3D9LastIndexBufferUsed != indexBuffer)
        {
            _rwD3D9LastIndexBufferUsed = indexBuffer;

            RwD3DDevice->SetIndices(indexBuffer);
        }
    }

    void RenderingEngine::RenderStateFlushCache()
    {
        if (NumRenderStateChanges)
        {
            auto n = 0;
            do
            {
                const D3DRENDERSTATETYPE stateChanged = RenderStateChanges[n];
                const RwUInt32 newValue = RenderStateCache[stateChanged].value;

                RenderStateCache[stateChanged].changed = FALSE;

                if (RenderStateD3D[stateChanged] != newValue)
                {
                    RenderStateD3D[stateChanged] = newValue;

                    IDirect3DDevice9_SetRenderState(_RwD3DDevice, stateChanged, newValue);
                }

            } while (++n < NumRenderStateChanges);

            NumRenderStateChanges = 0;
        }

        if (NumTextureStageStateChanges)
        {
            auto n = 0;
            do
            {
                const RwUInt32 stage = TextureStageStateChanges[n].stage;
                const D3DTEXTURESTAGESTATETYPE type = TextureStageStateChanges[n].type;
                const RwUInt32 newValue = TextureStageStateCache[stage][type].value;

                TextureStageStateCache[stage][type].changed = FALSE;

                if (TextureStageStateD3D[stage][type] != newValue)
                {
                    TextureStageStateD3D[stage][type] = newValue;

                    IDirect3DDevice9_SetTextureStageState(_RwD3DDevice, stage, type, newValue);
                }


            } while (++n < NumTextureStageStateChanges);

            NumTextureStageStateChanges = 0;
        }
    }


    void RenderingEngine::GetRenderState(uint32_t state, void* value)
    {
        *reinterpret_cast<uint32_t*>(value) = RenderStateCache[state].value;
    }

    void RenderingEngine::SetTextureStageState(uint32_t stage, uint32_t type, uint32_t value)
    {
        if (TextureStageStateCache[stage][type].value != value)
        {
            TextureStageStateCache[stage][type].value = value;

            if (!TextureStageStateCache[stage][type].changed)
            {
                TextureStageStateCache[stage][type].changed = true;
                TextureStageStateChanges[NumTextureStageStateChanges].stage = stage;
                TextureStageStateChanges[NumTextureStageStateChanges].type = (D3DTEXTURESTAGESTATETYPE)type;
                NumTextureStageStateChanges++;
            }
        }
    }

    void RenderingEngine::GetTextureStageState(uint32_t stage, uint32_t type, void* value)
    {
        *reinterpret_cast<uint32_t*>(value) = TextureStageStateCache[stage][type].value;
    }

    void RenderingEngine::SetSamplerState(uint32_t stage, uint32_t type, uint32_t value)
    {
        if (SamplerStageStateD3D[stage][type] != value)
        {
            SamplerStageStateD3D[stage][type] = value;

            RwD3DDevice->SetSamplerState(stage, (D3DSAMPLERSTATETYPE)type, value);
        }
    }

    void RenderingEngine::GetSamplerState(uint32_t stage, uint32_t type, void* value)
    {
        *reinterpret_cast<uint32_t*>(value) = SamplerStageStateD3D[stage][type];
    }

    void RenderingEngine::ForceSamplerState(uint32_t stage, uint32_t type, uint32_t value)
    {
        SamplerStageStateD3D[stage][type] = value;

        RwD3DDevice->SetSamplerState(stage, (D3DSAMPLERSTATETYPE)type, value);
    }

    void RenderingEngine::ForceRenderState(uint32_t state, uint32_t value)
    {
        RenderStateCache[state].value = value;
        RenderStateD3D[state] = value;

        RwD3DDevice->SetRenderState((D3DRENDERSTATETYPE)state, value);
    }

    void RenderingEngine::ForceTextureStageState(uint32_t stage, uint32_t type, uint32_t value)
    {
        TextureStageStateCache[stage][type].value = value;
        TextureStageStateD3D[stage][type] = value;

        RwD3DDevice->SetTextureStageState(stage, (D3DTEXTURESTAGESTATETYPE)type, value);
    }

    void RenderingEngine::DrawIndexedPrimitiveUP(uint32_t primitiveType, uint32_t minIndex, uint32_t numVertices, uint32_t primitiveCount, const void* indexData, const void* vertexStreamZeroData, uint32_t vertexStreamZeroStride)
    {
        SetStreamSource(0, nullptr, 0, 0);
        SetIndices(nullptr);

        RenderStateFlushCache();

        RwD3DDevice->DrawIndexedPrimitiveUP((D3DPRIMITIVETYPE)primitiveType, minIndex, numVertices, primitiveCount, indexData, D3DFMT_INDEX16,
            vertexStreamZeroData, vertexStreamZeroStride);
    }

    void RenderingEngine::DrawPrimitiveUP(uint32_t primitiveType, uint32_t primitiveCount, const void* vertexStreamZeroData, uint32_t vertexStreamZeroStride)
    {
        SetStreamSource(0, nullptr, 0, 0);
        SetIndices(nullptr);

        RenderStateFlushCache();

        RwD3DDevice->DrawPrimitiveUP((D3DPRIMITIVETYPE)primitiveType, primitiveCount, vertexStreamZeroData, vertexStreamZeroStride);
    }

    void RenderingEngine::DrawIndexedPrimitive(uint32_t primitiveType, int32_t baseVertexIndex, uint32_t minIndex, uint32_t numVertices, uint32_t startIndex, uint32_t primitiveCount)
    {
        RenderStateFlushCache();

        RwD3DDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)primitiveType, baseVertexIndex, minIndex, numVertices, startIndex, primitiveCount);
    }

    void RenderingEngine::DrawPrimitive(uint32_t primitiveType, uint32_t startVertex, uint32_t primitiveCount)
    {
        RenderStateFlushCache();

        RwD3DDevice->DrawPrimitive((D3DPRIMITIVETYPE)primitiveType, startVertex, primitiveCount);
    }

    bool RenderingEngine::SetTransform(uint32_t state, const void* matrix)
    {
        HRESULT hr;

        if (matrix)
        {
            if (LastMatrixUsed[state] && 
                memcmp(LastMatrixUsed[state], matrix, sizeof(D3DMATRIX)) == 0)
            {
                return true;
            }
            else
            {
                if (!LastMatrixUsed[state])
                {
                    LastMatrixUsed[state] = (D3DMATRIX*)RwFreeListAlloc(MatrixFreeList, rwID_DRIVERMODULE | rwMEMHINTDUR_EVENT);
                }

                memcpy(LastMatrixUsed[state], matrix, sizeof(D3DMATRIX));

                hr = RwD3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)state, (const D3DMATRIX*)matrix);

                if (state == D3DTS_WORLD)
                {
                    LastWorldMatrixUsedIdentity = false;
                }
            }
        }
        else
        {
            if (LastMatrixUsed[state] &&
                memcmp(LastMatrixUsed[state], &IdentityMatrix, sizeof(D3DMATRIX))==0)
            {
                return true;
            }
            else
            {
                if (!LastMatrixUsed[state])
                {
                    LastMatrixUsed[state] = (D3DMATRIX*)RwFreeListAlloc(MatrixFreeList, rwID_DRIVERMODULE | rwMEMHINTDUR_EVENT);
                }

                memcpy(LastMatrixUsed[state], &IdentityMatrix, sizeof(D3DMATRIX));

                hr = RwD3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)state, &IdentityMatrix);

                if (state == D3DTS_WORLD)
                {
                    LastWorldMatrixUsedIdentity = true;
                }
            }
        }

        return SUCCEEDED(hr);
    }

    void RenderingEngine::GetTransform(uint32_t state, void* matrix)
    {
        if (matrix)
        {
            if (LastMatrixUsed[state])
            {
                memcpy(matrix, LastMatrixUsed[state], sizeof(D3DMATRIX));
            }
            else
            {
                memcpy(matrix, &IdentityMatrix, sizeof(D3DMATRIX));
            }
        }
    }

    bool RenderingEngine::SetTransformWorld(const RwMatrix* matrix)
    {
        static D3DMATRIX d3dMatrix =
               {0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f};

        if (matrix && !rwMatrixTestFlags(matrix, rwMATRIXINTERNALIDENTITY))
        {
            d3dMatrix.m[0][0] = matrix->right.x;
            d3dMatrix.m[0][1] = matrix->right.y;
            d3dMatrix.m[0][2] = matrix->right.z;

            d3dMatrix.m[1][0] = matrix->up.x;
            d3dMatrix.m[1][1] = matrix->up.y;
            d3dMatrix.m[1][2] = matrix->up.z;

            d3dMatrix.m[2][0] = matrix->at.x;
            d3dMatrix.m[2][1] = matrix->at.y;
            d3dMatrix.m[2][2] = matrix->at.z;

            d3dMatrix.m[3][0] = matrix->pos.x;
            d3dMatrix.m[3][1] = matrix->pos.y;
            d3dMatrix.m[3][2] = matrix->pos.z;


            if (LastWorldMatrixUsedIdentity ||
                !memcmp(LastMatrixUsed[D3DTS_WORLD], &d3dMatrix, sizeof(D3DMATRIX)) == 0)
            {
                memcpy(LastMatrixUsed[D3DTS_WORLD], &d3dMatrix, sizeof(D3DMATRIX));
                LastWorldMatrixUsedIdentity = false;
               
                return SUCCEEDED(RwD3DDevice->SetTransform(D3DTS_WORLD, &d3dMatrix));
            }
        }
        else
        {
            if (!LastWorldMatrixUsedIdentity)
            {
                memcpy(LastMatrixUsed[D3DTS_WORLD], &IdentityMatrix, sizeof(D3DMATRIX));
                LastWorldMatrixUsedIdentity = true;

                return SUCCEEDED(RwD3DDevice->SetTransform(D3DTS_WORLD, &IdentityMatrix));
            }
        }

        return true;
    }

    bool RenderingEngine::IndexBufferCreate(uint32_t numIndices, IDirect3DIndexBuffer9** indexBuffer)
    {
        auto hr = RwD3DDevice->CreateIndexBuffer(numIndices * sizeof(RwUInt16), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED,
            indexBuffer, nullptr);

        return SUCCEEDED(hr);
    }

    bool RenderingEngine::IndexBuffer32bitsCreate(uint32_t numIndices, IDirect3DIndexBuffer9** indexBuffer)
    {
        auto hr = RwD3DDevice->CreateIndexBuffer(numIndices * sizeof(RwUInt16), D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED,
            indexBuffer, nullptr);

        return SUCCEEDED(hr);
    }

    void RenderingEngine::SetVertexShader(IDirect3DVertexShader9* shader)
    {
        if (_rwD3D9LastVertexShaderUsed != shader)
        {
            auto hr = RwD3DDevice->SetVertexShader(shader);
            if (FAILED(hr))
            {
                _rwD3D9LastVertexShaderUsed = (void*)0xffffffff;
            }
            else
            {
                _rwD3D9LastVertexShaderUsed = shader;
            }
        }
    }

    void RenderingEngine::SetPixelShader(IDirect3DPixelShader9* shader)
    {
        if (_rwD3D9LastPixelShaderUsed != shader)
        {
            auto hr = RwD3DDevice->SetPixelShader(shader);
            if (FAILED(hr))
            {
                _rwD3D9LastPixelShaderUsed = (void*)0xffffffff;
            }
            else
            {
                _rwD3D9LastPixelShaderUsed = shader;
            }
        }
    }

    void RenderingEngine::SetFVF(uint32_t fvf)
    {
        if (_rwD3D9LastFVFUsed != fvf)
        {
            auto hr = RwD3DDevice->SetFVF(fvf);
            if (FAILED(hr))
            {
                _rwD3D9LastFVFUsed = 0xffffffff;
            }
            else
            {
                _rwD3D9LastFVFUsed = fvf;
            }

            _rwD3D9LastVertexDeclarationUsed = (void*)0xffffffff;
        }

    }

    void RenderingEngine::SetVertexDeclaration(IDirect3DVertexDeclaration9* vertexDeclaration)
    {
        if (_rwD3D9LastVertexDeclarationUsed != vertexDeclaration)
        {
            auto hr = RwD3DDevice->SetVertexDeclaration(vertexDeclaration);
            if (FAILED(hr))
            {
                _rwD3D9LastVertexDeclarationUsed = (void*)0xffffffff;
            }
            else
            {
                _rwD3D9LastVertexDeclarationUsed = vertexDeclaration;
            }

            _rwD3D9LastFVFUsed = 0xffffffff;
        }

    }

    bool RenderingEngine::CameraClear(RwCamera* camera, RwRGBA* color, RwInt32 clearFlags)
    {
        BOOL				bCurrIconic;
        RwBool				bIconicRestore = FALSE;
        RwRaster* raster;
        RwRaster* topRaster;
        _rwD3D9RasterExt* topRasterExt;
        RwRaster* zRaster;
        RwRaster* zTopRaster;
        const _rwD3D9RasterExt* zrasExt;
        D3DCOLOR            packedColor = 0;
        HRESULT             hr = D3D_OK;
        RECT                rect;
        static const DWORD  clearFlagsConvTable[8] = {
            0,                      /* Nothing */
            D3DCLEAR_TARGET,        /* Clear Frame buffer only */
            D3DCLEAR_ZBUFFER,       /* Clear Z buffer only */
            D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, /* Clear Frame & Z buffer */
            D3DCLEAR_STENCIL,       /* Clear Stencil buffer */
            D3DCLEAR_TARGET | D3DCLEAR_STENCIL, /* Clear Frame & Stencil buffer */
            D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, /* Clear Z Buffer & Stencil */
            D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, /* Clear Frame, Z & stencil */
        };


        /* The clear color */
        if (rwCAMERACLEARIMAGE & clearFlags)
        {
            RwRGBA* col = (RwRGBA*)color;



            /* Pack the clear color */
            packedColor = (D3DCOLOR)(((col->alpha) << 24) |
                ((col->red) << 16) |
                ((col->green) << 8) | (col->blue));
        }

        raster = RwCameraGetRaster((RwCamera*)camera);

        /*
         * Got to get the top level raster as this is the only one with a
         * texture surface
         */
        topRaster = (raster)->parent;

        topRasterExt = RASTEREXTFROMRASTER(topRaster);

        if (topRasterExt->swapChain)
        {
            D3DVIEWPORT9        viewport;

            GetClientRect(topRasterExt->window, &rect);

            if (!rect.right || !rect.bottom)
            {
                return(FALSE);
            }
            else
            {
                if (rect.right != topRaster->originalWidth ||
                    rect.bottom != topRaster->originalHeight)
                {
                    if (raster == topRaster)
                    {
                        RwD3D9CameraAttachWindow(camera, topRasterExt->window);

                        /* Update raster pointers */
                        raster = RwCameraGetRaster((RwCamera*)camera);
                        topRaster = (raster)->parent;
                        topRasterExt = RASTEREXTFROMRASTER(topRaster);
                    }

                    zRaster = RwCameraGetZRaster((RwCamera*)camera);
                }
                else
                {
                    zRaster = RwCameraGetZRaster((RwCamera*)camera);

                    if (zRaster && zRaster == (zRaster)->parent)
                    {
                        if (rect.right != zRaster->width ||
                            rect.bottom != zRaster->height)
                        {
                            (void)RwCameraSetZRaster((RwCamera*)camera,
                                NULL);

                            RwRasterDestroy(zRaster);

                            zRaster =
                                RwRasterCreate(rect.right, rect.bottom, 0,
                                    rwRASTERTYPEZBUFFER);

                            if (zRaster)
                            {
                                (void)RwCameraSetZRaster((RwCamera*)camera,
                                    zRaster);
                            }
                        }
                    }
                }

                /* Set the render & depth/stencil surface */
                __rwD3D9SetRenderTarget(0, (LPSURFACE)topRasterExt->texture);

                if (zRaster)
                {
                    zTopRaster = (zRaster)->parent;

                    zrasExt = RASTEREXTFROMCONSTRASTER(zTopRaster);

                    _rwD3D9SetDepthStencilSurface((LPSURFACE)zrasExt->texture);

                }
                else
                {
                    _rwD3D9SetDepthStencilSurface(NULL);
                }

                /* Initialize the viewport to clear */
                viewport.X = 0;
                viewport.Y = 0;
                viewport.Width = raster->width;
                viewport.Height = raster->height;
                viewport.MinZ = 0;
                viewport.MaxZ = 1;

                /* Set the viewport to clear */
                IDirect3DDevice9_SetViewport(RwD3DDevice, &viewport);

                /* Clear */
                IDirect3DDevice9_Clear
                (RwD3DDevice, 0, NULL,
                    clearFlagsConvTable[clearFlags], packedColor,
                    1, 0);
            }
        }
        else
        {
            /* Check if the size of the main window has changed */
            GetClientRect(*WindowHandle, &rect);

            bCurrIconic = IsIconic(*WindowHandle);
            if (!bCurrIconic && bWindowIconic)
                bIconicRestore = TRUE;
            
            auto&  LastScreenWidth = *(int*)0x00C9BEE4;
            auto&  LastScreenHeight = *(int*)0x00C9BEE8;

            if (!rect.right || !rect.bottom || bCurrIconic)
            {
                //RWRETURN(FALSE);
            }
            else if ((RwUInt32)rect.right != _RwD3D9AdapterInformation.mode.Width
                || (RwUInt32)rect.bottom != _RwD3D9AdapterInformation.mode.Height ||
                bIconicRestore)
            {
                D3D9DeviceReleaseVideoMemory();
        
                hr = IDirect3DDevice9_TestCooperativeLevel(RwD3DDevice);        

                Log::Debug("RenderingEngine::CameraClear - rezing window %i %i %i", bIconicRestore, ((RwUInt32)rect.right), ((RwUInt32)rect.bottom));
                if (Present.Windowed)
                {
                    Present.BackBufferWidth = rect.right;
                    Present.BackBufferHeight = rect.bottom;
                    Log::Debug("RenderingEngine::CameraClear - rezing window %i %i", Present.BackBufferWidth, Present.BackBufferHeight);
                }

                hr = IDirect3DDevice9_Reset(RwD3DDevice, &Present);
                if (hr == D3DERR_DEVICELOST)
                    Log::Debug("RenderingEngine::CameraClear - device lost");

                if (hr == D3DERR_DEVICENOTRESET)
                    Log::Debug("RenderingEngine::CameraClear - device reset");

                if (hr == D3DERR_DEVICEREMOVED)
                    Log::Debug("RenderingEngine::CameraClear - device removed");

                if (hr == D3DERR_DRIVERINTERNALERROR)
                    Log::Debug("RenderingEngine::CameraClear - device internal error");

                if (hr == D3DERR_OUTOFVIDEOMEMORY)
                    Log::Debug("RenderingEngine::CameraClear - device out memory");

                if (SUCCEEDED(hr))
                {
                    Log::Debug("RenderingEngine::CameraClear - clear");
                    if (!RestoreVideoMemory())
                    {
                        D3D9DeviceReleaseVideoMemory();

                        hr = E_FAIL;
                    }
                }

                if (FAILED(hr))
                {
                    if (Present.Windowed)
                    {

                        Present.BackBufferWidth = _RwD3D9AdapterInformation.mode.Width;
                        Present.BackBufferHeight = _RwD3D9AdapterInformation.mode.Height;

                        hr = IDirect3DDevice9_Reset(RwD3DDevice, &Present);

                        if (FAILED(hr))
                        {
                            return(FALSE);
                        }

                        /* Change window size */
                        rect.right = rect.left + _RwD3D9AdapterInformation.mode.Width;
                        rect.bottom = rect.top + _RwD3D9AdapterInformation.mode.Height;

                        if (GetWindowLong(*WindowHandle, GWL_STYLE) & WS_MAXIMIZE)
                        {
                            SetWindowLong(*WindowHandle, GWL_STYLE, GetWindowLong(*WindowHandle, GWL_STYLE) & ~WS_MAXIMIZE);
                        }

                        AdjustWindowRectEx(&rect,
                            GetWindowLong(*WindowHandle, GWL_STYLE),
                            (GetMenu(*WindowHandle) != NULL),
                            GetWindowLong(*WindowHandle, GWL_EXSTYLE));

                        SetWindowPos(*WindowHandle, 0,
                            rect.left, rect.bottom,
                            rect.right - rect.left,
                            rect.bottom - rect.top,
                            SWP_NOMOVE | SWP_NOZORDER);

                        RestoreVideoMemory();
                    }
                    else
                    {
                        return(FALSE);
                    }
                }
                else
                {
                    _RwD3D9AdapterInformation.mode.Width = rect.right;
                    _RwD3D9AdapterInformation.mode.Height = rect.bottom;
                }
            }

            /* Check raster type */
            if (rwRASTERTYPECAMERATEXTURE == topRaster->cType)
            {
                LPSURFACE           surfaceLevel;
                D3DSURFACE_DESC     d3d9Desc;
                D3DVIEWPORT9        viewport;
                RwUInt32            d3d9ClearFlags;

                if (!IsWindowVisible(*WindowHandle))
                {
                    topRasterExt->texture = topRasterExt->texture;
                }

                if (topRasterExt->cube)
                {
                    IDirect3DCubeTexture9_GetCubeMapSurface((LPDIRECT3DCUBETEXTURE9)topRasterExt->texture,
                        (D3DCUBEMAP_FACES)topRasterExt->face,
                        0,
                        &surfaceLevel);
                }
                else
                {
                    IDirect3DTexture9_GetSurfaceLevel(topRasterExt->texture, 0,
                        &surfaceLevel);
                }

                IDirect3DSurface9_GetDesc(surfaceLevel, &d3d9Desc);

                /* Set the render & depth/stencil surface */
                if (d3d9Desc.Usage & D3DUSAGE_RENDERTARGET)
                {
                    __rwD3D9SetRenderTarget(0, surfaceLevel);

                    zRaster = RwCameraGetZRaster((RwCamera*)camera);
                    if (zRaster)
                    {
                        /*
                        * Got to get the top level raster as this is the only one with a
                        * texture surface
                        */
                        zTopRaster = (zRaster)->parent;

                        zrasExt = RASTEREXTFROMCONSTRASTER(zTopRaster);

                        if ((RwRasterGetType(zTopRaster) & rwRASTERTYPEMASK) == rwRASTERTYPEZBUFFER)
                        {
                            _rwD3D9SetDepthStencilSurface((LPSURFACE)zrasExt->texture);
                        }
                        else
                        {
                            LPSURFACE   surfaceLevelZ;

                            IDirect3DTexture9_GetSurfaceLevel(zrasExt->texture, 0,
                                &surfaceLevelZ);

                            _rwD3D9SetDepthStencilSurface(surfaceLevelZ);

                            IDirect3DSurface9_Release(surfaceLevelZ);
                        }
                    }
                    else
                    {
                        _rwD3D9SetDepthStencilSurface(NULL);
                    }

                    hr = D3D_OK;
                }
                else
                {
                    hr = E_FAIL;
                }

                /* Initialize the viewport to clear */
                viewport.X = raster->nOffsetX;
                viewport.Y = raster->nOffsetY;
                viewport.Width = raster->width;
                viewport.Height = raster->height;
                viewport.MinZ = 0;
                viewport.MaxZ = 1;

                /* Set the viewport to clear */
                IDirect3DDevice9_SetViewport
                (RwD3DDevice, &viewport);

                /* check clear flags */
                d3d9ClearFlags = clearFlagsConvTable[clearFlags];

                if (d3d9ClearFlags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL))
                {
                    if (RwCameraGetZRaster((RwCamera*)camera) == NULL)
                    {
                        d3d9ClearFlags &= ~(D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
                    }
                }

                /* Clear */
                IDirect3DDevice9_Clear
                (RwD3DDevice, 0, NULL, d3d9ClearFlags, packedColor,
                    1, 0);
                /* If the set render target fails, we need to copy from the back buffer */
                if (FAILED(hr))
                {
                    RECT    rectSrc;

                    rectSrc.left = raster->nOffsetX;
                    rectSrc.top = raster->nOffsetY;
                    rectSrc.right = rectSrc.left + raster->width;
                    rectSrc.bottom = rectSrc.top + raster->height;

                    if (RwD3D9DeviceCaps->StretchRectFilterCaps & (D3DPTFILTERCAPS_MINFPOINT | D3DPTFILTERCAPS_MAGFPOINT))
                    {
                        hr = IDirect3DDevice9_StretchRect(RwD3DDevice, RwD3D9RenderSurface, &rectSrc,
                            surfaceLevel, &rectSrc, D3DTEXF_POINT);
                    }
                    else
                    {
                        hr = IDirect3DDevice9_StretchRect(RwD3DDevice, RwD3D9RenderSurface, &rectSrc,
                            surfaceLevel, &rectSrc, D3DTEXF_NONE);
                    }
                }

                /* Release the cleared surface */
                IDirect3DSurface9_Release(surfaceLevel);
            }
            else
            {
                D3DVIEWPORT9        viewport;
                RwUInt32            d3d9ClearFlags = 0;

                /* Set the render & depth/stencil surface */
                __rwD3D9SetRenderTarget(0, RwD3D9RenderSurface);
                _rwD3D9SetDepthStencilSurface(RwD3D9DepthStencilSurface);

                /* Initialize the viewport to clear */
                viewport.X = raster->nOffsetX;
                viewport.Y = raster->nOffsetY;
                viewport.Width = raster->width;
                viewport.Height = raster->height;
                viewport.MinZ = 0;
                viewport.MaxZ = 1;

                /* Set the viewport to clear */
                IDirect3DDevice9_SetViewport
                (RwD3DDevice, &viewport);

                /* Clear */
                if (rwCAMERACLEARIMAGE & clearFlags)
                {
                    d3d9ClearFlags |= D3DCLEAR_TARGET;
                }

                if (rwCAMERACLEARZ & clearFlags)
                {
                    d3d9ClearFlags |= D3DCLEAR_ZBUFFER;

                    if (1)
                    {
                        d3d9ClearFlags |= D3DCLEAR_STENCIL;
                    }
                }
                else if (rwCAMERACLEARSTENCIL & clearFlags)
                {
                    if (1)
                    {
                        d3d9ClearFlags |= D3DCLEAR_STENCIL;
                    }
                }

                IDirect3DDevice9_Clear
                (RwD3DDevice, 0, NULL, d3d9ClearFlags, packedColor,
                    1, 0);
            }
            bWindowIconic = bCurrIconic;
        }
        return true;
    }

    bool RenderingEngine::CameraBeginUpdate(RwCamera* cameraIn)
    {
        BOOL				bCurrIconic;
        RwBool				bIconicRestore = FALSE;
        RwCamera* camera;
        const RwMatrix* camLTM;
        RwRaster* raster;
        RwRaster* topRaster;
        _rwD3D9RasterExt* topRasterExt;
        RwRaster* zRaster;
        RwRaster* zTopRaster;
        const _rwD3D9RasterExt* zRasterExt;
        D3DVIEWPORT9        viewport;
        HRESULT             hr;
        RECT                rect;


        /* set the FPU to single precision */
        _rwProcessorForceSinglePrecision();


        /*
         * Save the camera pointer
         */
        camera = (RwCamera*)cameraIn;
        dgGGlobals.curCamera = camera;

        /*
         * View matrix - (camera matrix)
         */
        camLTM = RwFrameGetLTM(RwCameraGetFrame(camera));

        RwMatrixInvert((RwMatrix*)&_RwD3D9D3D9ViewTransform, camLTM);

        _RwD3D9D3D9ViewTransform.m[0][0] = -_RwD3D9D3D9ViewTransform.m[0][0];
        _RwD3D9D3D9ViewTransform.m[0][3] = 0.0f;

        _RwD3D9D3D9ViewTransform.m[1][0] = -_RwD3D9D3D9ViewTransform.m[1][0];
        _RwD3D9D3D9ViewTransform.m[1][3] = 0.0f;

        _RwD3D9D3D9ViewTransform.m[2][0] = -_RwD3D9D3D9ViewTransform.m[2][0];
        _RwD3D9D3D9ViewTransform.m[2][3] = 0.0f;

        _RwD3D9D3D9ViewTransform.m[3][0] = -_RwD3D9D3D9ViewTransform.m[3][0];
        _RwD3D9D3D9ViewTransform.m[3][3] = 1.0f;

        RwD3D9SetTransform(D3DTS_VIEW, &_RwD3D9D3D9ViewTransform);

        /*
         * Projection matrix
         */
        _RwD3D9D3D9ProjTransform.m[0][0] = camera->recipViewWindow.x;
        _RwD3D9D3D9ProjTransform.m[1][1] = camera->recipViewWindow.y;

        /* Shear X, Y by view offset with Z invariant */
        _RwD3D9D3D9ProjTransform.m[2][0] =
            camera->recipViewWindow.x * camera->viewOffset.x;
        _RwD3D9D3D9ProjTransform.m[2][1] =
            camera->recipViewWindow.y * camera->viewOffset.y;

        /* Translate to shear origin */
        _RwD3D9D3D9ProjTransform.m[3][0] =
            -camera->recipViewWindow.x * camera->viewOffset.x;

        _RwD3D9D3D9ProjTransform.m[3][1] =
            -camera->recipViewWindow.y * camera->viewOffset.y;

        /* Projection type */
        if (camera->projectionType == rwPARALLEL)
        {
            _RwD3D9D3D9ProjTransform.m[2][2] =
                1.0f / (camera->farPlane - camera->nearPlane);
            _RwD3D9D3D9ProjTransform.m[2][3] = 0.0f;
            _RwD3D9D3D9ProjTransform.m[3][3] = 1.0f;
        }
        else
        {
            _RwD3D9D3D9ProjTransform.m[2][2] =
                camera->farPlane / (camera->farPlane - camera->nearPlane);
            _RwD3D9D3D9ProjTransform.m[2][3] = 1.0f;
            _RwD3D9D3D9ProjTransform.m[3][3] = 0.0f;
        }

        _RwD3D9D3D9ProjTransform.m[3][2] = -_RwD3D9D3D9ProjTransform.m[2][2] * camera->nearPlane;

        RwD3D9SetTransform(D3DTS_PROJECTION, &_RwD3D9D3D9ProjTransform);


        raster = RwCameraGetRaster(camera);

        topRaster = (raster)->parent;

        topRasterExt = RASTEREXTFROMRASTER(topRaster);

        if (topRasterExt->swapChain)
        {
            GetClientRect(topRasterExt->window, &rect);

            if (!rect.right || !rect.bottom)
            {
                return(FALSE);
            }
            else
            {
                if (rect.right != topRaster->originalWidth ||
                    rect.bottom != topRaster->originalHeight)
                {
                    if (raster == topRaster)
                    {
                        RwD3D9CameraAttachWindow(camera, topRasterExt->window);

                        /* Update raster pointers */
                        raster = RwCameraGetRaster(camera);
                        topRaster = (raster)->parent;
                        topRasterExt = RASTEREXTFROMRASTER(topRaster);
                    }

                    zRaster = RwCameraGetZRaster((RwCamera*)camera);
                }
                else
                {
                    zRaster = RwCameraGetZRaster((RwCamera*)camera);

                    if (zRaster && zRaster == (zRaster)->parent)
                    {
                        if (rect.right != zRaster->width ||
                            rect.bottom != zRaster->height)
                        {
                            (void)RwCameraSetZRaster(camera, NULL);

                            RwRasterDestroy(zRaster);

                            zRaster =
                                RwRasterCreate(rect.right, rect.bottom, 0,
                                    rwRASTERTYPEZBUFFER);

                            if (zRaster)
                            {
                                RwCameraSetZRaster(camera, zRaster);
                            }
                        }
                    }
                }

                __rwD3D9SetRenderTarget(0, (LPSURFACE)topRasterExt->texture);

                if (zRaster)
                {
                    zTopRaster = (zRaster)->parent;

                    zRasterExt = RASTEREXTFROMCONSTRASTER(zTopRaster);

                    _rwD3D9SetDepthStencilSurface((LPSURFACE)zRasterExt->texture);
                }
                else
                {
                    _rwD3D9SetDepthStencilSurface(NULL);
                }
            }
        }
        else
        {
            /* Check if the main window has changed its size */
            GetClientRect(*WindowHandle, &rect);

            bCurrIconic = IsIconic(*WindowHandle);
            if (!bCurrIconic && bWindowIconic)
                bIconicRestore = TRUE;

            bWindowIconic = bCurrIconic;

            if (!rect.right || !rect.bottom || bCurrIconic)
            {
                //         RWRETURN(FALSE);
            }
            else if ((RwUInt32)rect.right != _RwD3D9AdapterInformation.mode.Width
                || (RwUInt32)rect.bottom != _RwD3D9AdapterInformation.mode.Height
                || bIconicRestore || bChangeVideo)
            {
                bChangeVideo = FALSE;

                D3D9DeviceReleaseVideoMemory();

                hr = IDirect3DDevice9_TestCooperativeLevel(RwD3DDevice);

                if (Present.Windowed)
                {
                    Present.BackBufferWidth = rect.right;
                    Present.BackBufferHeight = rect.bottom;
                }

                hr = IDirect3DDevice9_Reset(RwD3DDevice, &Present);

                if (SUCCEEDED(hr))
                {
                    if (!RestoreVideoMemory())
                    {
                        D3D9DeviceReleaseVideoMemory();

                        hr = E_FAIL;
                    }
                }

                if (FAILED(hr))
                {
                    if (Present.Windowed)
                    {

                        Present.BackBufferWidth = _RwD3D9AdapterInformation.mode.Width;
                        Present.BackBufferHeight = _RwD3D9AdapterInformation.mode.Height;

                        hr = IDirect3DDevice9_Reset(RwD3DDevice, &Present);

                        if (FAILED(hr))
                        {
                            return(FALSE);
                        }

                        /* Change window size */
                        rect.right = rect.left + _RwD3D9AdapterInformation.mode.Width;
                        rect.bottom = rect.top + _RwD3D9AdapterInformation.mode.Height;

                        if (GetWindowLong(*WindowHandle, GWL_STYLE) & WS_MAXIMIZE)
                        {
                            SetWindowLong(*WindowHandle, GWL_STYLE, GetWindowLong(*WindowHandle, GWL_STYLE) & ~WS_MAXIMIZE);
                        }

                        AdjustWindowRectEx(&rect,
                            GetWindowLong(*WindowHandle, GWL_STYLE),
                            (GetMenu(*WindowHandle) != NULL),
                            GetWindowLong(*WindowHandle, GWL_EXSTYLE));

                        SetWindowPos(*WindowHandle, 0,
                            rect.left, rect.bottom,
                            rect.right - rect.left,
                            rect.bottom - rect.top,
                            SWP_NOMOVE | SWP_NOZORDER);

                        RestoreVideoMemory();
                    }
                    else
                    {
                        return(FALSE);
                    }
                }
                else
                {
                    _RwD3D9AdapterInformation.mode.Width = rect.right;
                    _RwD3D9AdapterInformation.mode.Height = rect.bottom;
                }
            }

            /* Check if it's a camera texture */
            if (rwRASTERTYPECAMERATEXTURE == topRaster->cType)
            {
                LPSURFACE           surfaceLevel;
                D3DSURFACE_DESC     d3d9Desc;

                /* Set the render & depth/stencil surface */
                if (topRasterExt->cube)
                {
                    IDirect3DCubeTexture9_GetCubeMapSurface((LPDIRECT3DCUBETEXTURE9)topRasterExt->texture,
                        (D3DCUBEMAP_FACES)topRasterExt->face,
                        0,
                        &surfaceLevel);
                }
                else
                {
                    IDirect3DTexture9_GetSurfaceLevel(topRasterExt->texture, 0,
                        &surfaceLevel);
                }

                IDirect3DSurface9_GetDesc(surfaceLevel, &d3d9Desc);

                if (d3d9Desc.Usage & D3DUSAGE_RENDERTARGET)
                {
                    __rwD3D9SetRenderTarget(0, surfaceLevel);

                    zRaster = RwCameraGetZRaster((RwCamera*)camera);
                    if (zRaster)
                    {
                        /*
                        * Got to get the top level raster as this is the only one with a
                        * texture surface
                        */
                        zTopRaster = (zRaster)->parent;

                        zRasterExt = RASTEREXTFROMCONSTRASTER(zTopRaster);

                        if ((RwRasterGetType(zTopRaster) & rwRASTERTYPEMASK) == rwRASTERTYPEZBUFFER)
                        {
                            _rwD3D9SetDepthStencilSurface((LPSURFACE)zRasterExt->texture);
                        }
                        else
                        {
                            LPSURFACE   surfaceLevelZ;

                            IDirect3DTexture9_GetSurfaceLevel(zRasterExt->texture, 0,
                                &surfaceLevelZ);

                            _rwD3D9SetDepthStencilSurface(surfaceLevelZ);

                            IDirect3DSurface9_Release(surfaceLevelZ);
                        }
                    }
                    else
                    {
                        _rwD3D9SetDepthStencilSurface(NULL);
                    }

                    hr = D3D_OK;
                }
                else
                {
                    hr = E_FAIL;
                }

                if (FAILED(hr))
                {
                    NeedToCopyFromBackBuffer = TRUE;
                    __rwD3D9SetRenderTarget(0, RwD3D9RenderSurface);
                    _rwD3D9SetDepthStencilSurface(RwD3D9DepthStencilSurface);
                }

                IDirect3DSurface9_Release(surfaceLevel);
            }
            else
            {
                __rwD3D9SetRenderTarget(0, RwD3D9RenderSurface);
                _rwD3D9SetDepthStencilSurface(RwD3D9DepthStencilSurface);
            }
        }

        /*
         * Set the viewport
         */
        viewport.X = raster->nOffsetX;
        viewport.Y = raster->nOffsetY;
        viewport.Width = raster->width;
        viewport.Height = raster->height;
        viewport.MinZ = 0;
        viewport.MaxZ = 1;

        IDirect3DDevice9_SetViewport(RwD3DDevice, &viewport);

        /* Update fog range */
        RwD3D9SetRenderState(D3DRS_FOGSTART, FLOATASDWORD(camera->fogPlane));
        RwD3D9SetRenderState(D3DRS_FOGEND, FLOATASDWORD(camera->farPlane));


        return (BeginScene());
    }

    bool RenderingEngine::CameraEndUpdate(RwCamera* camera)
    {
        RwRaster* raster;
        RwRaster* topRaster;
        const _rwD3D9RasterExt* topRasterExt;


        raster = RwCameraGetRaster(dgGGlobals.curCamera);

        /*
         * Got to get the top level raster as this is the only one with a
         * texture surface
         */
        topRaster = (raster)->parent;

        topRasterExt = RASTEREXTFROMCONSTRASTER(topRaster);

        if (topRasterExt->swapChain == NULL)
        {
            /* If the cameras raster was a camera texture, set the render target and
             * depth/stencil surfaces back
             */
            if (rwRASTERTYPECAMERATEXTURE == (topRaster->cType))
            {
                if (NeedToCopyFromBackBuffer)
                {
                    LPSURFACE           surfaceLevel;
                    RECT                rectSrc;

                    if (topRasterExt->cube)
                    {
                        IDirect3DCubeTexture9_GetCubeMapSurface((LPDIRECT3DCUBETEXTURE9)topRasterExt->texture,
                            (D3DCUBEMAP_FACES)topRasterExt->face,
                            0,
                            &surfaceLevel);
                    }
                    else
                    {
                        IDirect3DTexture9_GetSurfaceLevel(topRasterExt->texture, 0,
                            &surfaceLevel);
                    }

                    /* Copy from the backbuffer to the camera texture */
                    rectSrc.left = raster->nOffsetX;
                    rectSrc.top = raster->nOffsetY;
                    rectSrc.right = rectSrc.left + raster->width;
                    rectSrc.bottom = rectSrc.top + raster->height;

                    if (RwD3D9DeviceCaps->StretchRectFilterCaps & (D3DPTFILTERCAPS_MINFPOINT | D3DPTFILTERCAPS_MAGFPOINT))
                    {
                        IDirect3DDevice9_StretchRect
                        (RwD3DDevice,
                            RwD3D9RenderSurface, &rectSrc,
                            surfaceLevel, &rectSrc, D3DTEXF_POINT);
                    }
                    else
                    {
                        IDirect3DDevice9_StretchRect
                        (RwD3DDevice,
                            RwD3D9RenderSurface, &rectSrc,
                            surfaceLevel, &rectSrc, D3DTEXF_NONE);
                    }

                    IDirect3DSurface9_Release(surfaceLevel);

                    NeedToCopyFromBackBuffer = FALSE;
                }
            }
        }

        /* Remove current camera, so we don't use a camera if it becomes destroyed
         * (since we don't get a look in at destruction time).
         */
        dgGGlobals.curCamera = NULL;
        return(TRUE);
    }

    bool RenderingEngine::RasterShowRaster(void* raster, void* inOut, RwInt32 flags)
    {
        RwBool              doReset = FALSE;
        RwRaster* topRaster;
        _rwD3D9RasterExt* topRasterExt;
        HRESULT             hr;

        if (InsideScene)
        {
            hr = IDirect3DDevice9_EndScene(RwD3DDevice);

            InsideScene = false;
        }


        /* Check if we need to change the VSync present flags */
        if ((flags & rwRASTERFLIPWAITVSYNC) != 0)
        {
            if (Present.PresentationInterval == D3DPRESENT_INTERVAL_IMMEDIATE)
            {
                Present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

                doReset = TRUE;
            }
        }
        else
        {
            if (Present.PresentationInterval == D3DPRESENT_INTERVAL_ONE)
            {
                Present.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

                doReset = TRUE;
            }
        }

        /* Change present flags if needed */
        if (doReset)
        {
            D3D9DeviceReleaseVideoMemory();

            hr = IDirect3DDevice9_Reset(RwD3DDevice, &Present);

            if (SUCCEEDED(hr))
            {
                RestoreVideoMemory();
            }
        }

        /* Do the flip */

        /*
         * Got to get the top level raster as this is the only one with a
         * texture surface
         */
        topRaster = ((RwRaster*)raster)->parent;

        topRasterExt = RASTEREXTFROMRASTER(topRaster);

        if (topRasterExt->swapChain)
        {
            hr = IDirect3DSwapChain9_Present
            (topRasterExt->swapChain, NULL, NULL, NULL, NULL, 0);
        }
        else
        {
            hr = IDirect3DDevice9_Present(RwD3DDevice, NULL, NULL, NULL, NULL);
        }

        /* Check for lost devices */
        if (hr == D3DERR_DEVICELOST)
        {
            hr = IDirect3DDevice9_TestCooperativeLevel(RwD3DDevice);

            if (hr == D3DERR_DEVICENOTRESET)
            {


                D3D9DeviceReleaseVideoMemory();

                hr = IDirect3DDevice9_Reset(RwD3DDevice, &Present);

                if (SUCCEEDED(hr))
                {
                    RestoreVideoMemory();
                }
            }
        }

        _rwD3D9DynamicVertexBufferManagerForceDiscard();
        return SUCCEEDED(hr);
    }

    bool RenderingEngine::BeginScene()
    {
        if (TestState())
        {
            if (!InsideScene)
            {
                HRESULT hr;

                hr = IDirect3DDevice9_BeginScene(RwD3DDevice);

                if (SUCCEEDED(hr))
                {
                    InsideScene = true;
                }
            }
        }
        else
        {
            InsideScene = false;
        }

        return InsideScene;
    }

    bool RenderingEngine::TestState()
    {
        auto hr = IDirect3DDevice9_TestCooperativeLevel(RwD3DDevice);

        if (hr == D3DERR_DEVICENOTRESET)
        {
            ReleaseVideoMemory();

            hr = IDirect3DDevice9_Reset(RwD3DDevice, &Present);

            if (hr == D3DERR_INVALIDCALL)
            {

                if (Present.Windowed)
                {
                    D3DDISPLAYMODE      adapterDisplayMode;

                    IDirect3D9_GetAdapterDisplayMode(_RwDirect3DObject, _RwD3DAdapterIndex, &adapterDisplayMode);
                    Present.BackBufferFormat = _RwD3D9AdapterInformation.mode.Format = adapterDisplayMode.Format;
                    _RwD3D9AdapterInformation.displayDepth = D3D9FindDepth(_RwD3D9AdapterInformation.mode.Format);

                    hr = IDirect3DDevice9_Reset(RwD3DDevice, &Present);
                }
            }

            if (SUCCEEDED(hr))
            {
                RestoreVideoMemory();
            }
        }


       return SUCCEEDED(hr);
    }
    typedef void (*rwD3D9DeviceRestoreCallBack)(void);
     rwD3D9DeviceRestoreCallBack D3D9RestoreDeviceCallback = *(rwD3D9DeviceRestoreCallBack*)0x00C980B0;
    
     typedef void (*rwD3D9DeviceReleaseCallBack)(void);
    
      rwD3D9DeviceReleaseCallBack D3D9ReleaseDeviceCallback = NULL;
    bool RenderingEngine::RestoreVideoMemory()
    {
        IDirect3DDevice9_GetRenderTarget(RwD3DDevice, 0, &RwD3D9RenderSurface);
        IDirect3DSurface9_Release(RwD3D9RenderSurface);

        IDirect3DDevice9_GetDepthStencilSurface(RwD3DDevice, &RwD3D9DepthStencilSurface);
        IDirect3DSurface9_Release(RwD3D9DepthStencilSurface);

        bool rv;
        rv = InitVideoMemoryRasters();

        rwD3D9DynamicVertexBufferRestore();

        if (rv)
        {
            rwD3D9RenderStateReset();
        }

        rv = rv && rwD3D9Im2DRenderOpen();

        rv = rv && rwD3D9Im3DRenderOpen();

        if (D3D9RestoreDeviceCallback)
        {
            D3D9RestoreDeviceCallback();
        }

        return rv;
    }

    void RenderingEngine::ReleaseVideoMemory()
    {
        RwUInt32            i;

        D3D9ClearCacheShaders();
        D3D9ClearCacheMatrix();

        for (i = 0; i < RWD3D9_MAX_TEXTURE_STAGES; i++)
        {
            IDirect3DDevice9_SetTexture(_RwD3DDevice, i, NULL);
        }

        IDirect3DDevice9_SetIndices(_RwD3DDevice, NULL);

        for (i = 0; i < MAX_STREAMS; i++)
        {
            IDirect3DDevice9_SetStreamSource(_RwD3DDevice, i, NULL, 0, 0);
        }

        IDirect3DDevice9_SetPixelShader(_RwD3DDevice, NULL);

        IDirect3DDevice9_SetVertexDeclaration(_RwD3DDevice, NULL);

        IDirect3DDevice9_SetVertexShader(_RwD3DDevice, NULL);

        __rwD3D9SetRenderTarget(0, RwD3D9RenderSurface);
        for (i = 1; i < 4; i++)
        {
            __rwD3D9SetRenderTarget(i, NULL);
        }

        _rwD3D9SetDepthStencilSurface(RwD3D9DepthStencilSurface);
        _rwD3D9Im2DRenderClose();
        _rwD3D9Im3DRenderClose();
        _rwD3D9DynamicVertexBufferRelease();
        _rxD3D9VideoMemoryRasterListRelease();
        RwFreeListPurgeAllFreeLists();

        if (D3D9ReleaseDeviceCallback != NULL)
        {
            D3D9ReleaseDeviceCallback();
        }
    }

    void RenderingEngine::D3D9SetPresentParameters(const D3DDISPLAYMODE* mode, RwUInt32 flags, D3DFORMAT adapterFormat)
    {
        if (flags & rwVIDEOMODEEXCLUSIVE)
        {
            /* On full screen the adapter format is the display format selected */
            adapterFormat = mode->Format;

            /* fullscreen */
            Present.Windowed = FALSE;

            Present.BackBufferCount = 1;

            /* As fast as we can */
            Present.FullScreen_RefreshRateInHz = min( mode->RefreshRate,  FullScreenRefreshRateInHz);

#ifdef LOCK_AT_VSYNC
            Present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
#else
            Present.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
#endif

            Present.BackBufferWidth = mode->Width;
            Present.BackBufferHeight = mode->Height;

            Present.SwapEffect = D3DSWAPEFFECT_FLIP;
        }
        else
        {
            RECT rect;

            /* windowed */
            Present.Windowed = TRUE;

            Present.BackBufferCount = 1;

            Present.FullScreen_RefreshRateInHz = 0;

            /* As fast as we can */
#ifdef LOCK_AT_VSYNC
            Present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
#else
            Present.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
#endif

            /* Check window size */
            GetWindowRect(*WindowHandle, &rect);

            if ((rect.right - rect.left) == 0)
            {
                Present.BackBufferWidth = 1;
            }
            else
            {
                Present.BackBufferWidth = 0;
            }

            if ((rect.bottom - rect.top) == 0)
            {
                Present.BackBufferHeight = 1;
            }
            else
            {
                Present.BackBufferHeight = 0;
            }

            Present.SwapEffect = D3DSWAPEFFECT_COPY;
        }

       /* if (EnableFullScreenDialogBoxMode)
        {
            Present.SwapEffect = D3DSWAPEFFECT_DISCARD;
            Present.BackBufferFormat = adapterFormat;
        }
        else
        {*/
            /* Try backbuffer with alpha channel if using standard 32 bits format*/
            if (D3DFMT_X8R8G8B8 == adapterFormat)
            {
                if (SUCCEEDED(IDirect3D9_CheckDeviceType(_RwDirect3DObject,_RwD3DAdapterIndex, _RwD3DAdapterType,
                    adapterFormat,D3DFMT_A8R8G8B8,Present.Windowed)))
                {
                    Present.BackBufferFormat = D3DFMT_A8R8G8B8;
                }
                else
                {
                    Present.BackBufferFormat = D3DFMT_X8R8G8B8;
                }
            }
            else
            {
                Present.BackBufferFormat = adapterFormat;
            }
        //}

        /* TRUE so depth/stencil buffer are managed for us */
        Present.EnableAutoDepthStencil = TRUE;

        /*
         * Find the Z-Buffer depth
         * Best perfomance results when using same bit depth than back buffer
         */
        _RwD3D9ZBufferDepth = D3D9FindDepth(Present.BackBufferFormat);

        if (_RwD3D9ZBufferDepth == 32)
        {
            if (D3D_OK ==
                IDirect3D9_CheckDeviceFormat(_RwDirect3DObject, _RwD3DAdapterIndex, _RwD3DAdapterType,
                    adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8)
                && D3D_OK ==
                IDirect3D9_CheckDepthStencilMatch(_RwDirect3DObject, _RwD3DAdapterIndex,
                    _RwD3DAdapterType, adapterFormat, Present.BackBufferFormat, D3DFMT_D24S8))
            {
                Present.AutoDepthStencilFormat = D3DFMT_D24S8;

                // _RwHasStencilBuffer = TRUE;
            }
            else
            {
                if (D3D_OK ==
                    IDirect3D9_CheckDeviceFormat(_RwDirect3DObject, _RwD3DAdapterIndex,
                        _RwD3DAdapterType, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X4S4)
                    && D3D_OK == IDirect3D9_CheckDepthStencilMatch(_RwDirect3DObject, _RwD3DAdapterIndex, _RwD3DAdapterType,
                        adapterFormat, Present.BackBufferFormat, D3DFMT_D24X4S4))
                {
                    Present.AutoDepthStencilFormat = D3DFMT_D24X4S4;

                    //_RwHasStencilBuffer = TRUE;
                }
                else
                {
                    /* !!!! we don't have stencil buffer */
                    if (D3D_OK == IDirect3D9_CheckDeviceFormat(_RwDirect3DObject, _RwD3DAdapterIndex, _RwD3DAdapterType,
                        adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D32)
                        && D3D_OK == IDirect3D9_CheckDepthStencilMatch(_RwDirect3DObject, _RwD3DAdapterIndex,
                            _RwD3DAdapterType, adapterFormat, Present.BackBufferFormat, D3DFMT_D32))
                    {
                        Present.AutoDepthStencilFormat = D3DFMT_D32;

                        //  _RwHasStencilBuffer = FALSE;
                    }
                    else
                    {
                        if (D3D_OK == IDirect3D9_CheckDeviceFormat(_RwDirect3DObject, _RwD3DAdapterIndex, _RwD3DAdapterType,
                            adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8)
                            && D3D_OK ==
                            IDirect3D9_CheckDepthStencilMatch(_RwDirect3DObject, _RwD3DAdapterIndex,
                                _RwD3DAdapterType, adapterFormat, Present.BackBufferFormat, D3DFMT_D24X8))
                        {
                            Present.AutoDepthStencilFormat = D3DFMT_D24X8;

                            //  _RwHasStencilBuffer = FALSE;
                        }
                        else
                        {
                            _RwD3D9ZBufferDepth = 16;
                        }
                    }
                }
            }
        }

        if (_RwD3D9ZBufferDepth == 16)
        {
            if (D3D_OK == IDirect3D9_CheckDeviceFormat(_RwDirect3DObject, _RwD3DAdapterIndex, _RwD3DAdapterType,
                adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D15S1)
                && D3D_OK == IDirect3D9_CheckDepthStencilMatch(_RwDirect3DObject, _RwD3DAdapterIndex,
                    _RwD3DAdapterType, adapterFormat, Present.BackBufferFormat, D3DFMT_D15S1))
            {
                Present.AutoDepthStencilFormat = D3DFMT_D15S1;

                // _RwHasStencilBuffer = TRUE;
            }
            else
            {
                Present.AutoDepthStencilFormat = D3DFMT_D16;

                // _RwHasStencilBuffer = FALSE;
            }
        }

        Present.MultiSampleType = D3DMULTISAMPLE_NONE;
        Present.MultiSampleQuality = 0;

        if ((SelectedMultisamplingLevels > 1 || SelectedMultisamplingLevelsNonMask > 1))
        {
            DWORD maxQualityLevels = 0;
            HRESULT hr;

            hr = IDirect3D9_CheckDeviceMultiSampleType(_RwDirect3DObject, _RwD3DAdapterIndex, _RwD3DAdapterType,
                Present.BackBufferFormat, Present.Windowed, (D3DMULTISAMPLE_TYPE)SelectedMultisamplingLevels,  &maxQualityLevels);

            if (SUCCEEDED(hr) &&
                maxQualityLevels > 0)
            {
                Present.MultiSampleType = ((D3DMULTISAMPLE_TYPE)  SelectedMultisamplingLevels);

                Present.MultiSampleQuality = maxQualityLevels - 1;
            }
            else
            {
                maxQualityLevels = 0;

                hr = IDirect3D9_CheckDeviceMultiSampleType(_RwDirect3DObject, _RwD3DAdapterIndex, _RwD3DAdapterType,
                    Present.BackBufferFormat, Present.Windowed, D3DMULTISAMPLE_NONMASKABLE, &maxQualityLevels);

                if (SUCCEEDED(hr) && maxQualityLevels > 0)
                {
                    RwUInt32 qualityLevel;

                    qualityLevel = (SelectedMultisamplingLevelsNonMask - 2);

                    if (qualityLevel > (maxQualityLevels - 1))
                    {
                        qualityLevel = (maxQualityLevels - 1);
                    }

                    Present.MultiSampleType = D3DMULTISAMPLE_NONMASKABLE;
                    Present.MultiSampleQuality = qualityLevel;
                }
            }

            if (Present.MultiSampleType != D3DMULTISAMPLE_NONE)
            {
                Present.SwapEffect = D3DSWAPEFFECT_DISCARD;
            }
        }
        RWRETURNVOID();
    }

    RwBool RenderingEngine::D3D9DeviceSystemOpen(void* out __RWUNUSED__, void* inOut, RwInt32 in __RWUNUSED__)
    {
        Log::Debug("RenderingEngine::D3D9DeviceSystemOpen");
        RwUInt32 numAdapters;
        RwInt32  i;

        /* We're interested in the window handle here, so store it away */
        *WindowHandle = (HWND)(((RwEngineOpenParams*)inOut)->displayID);

        /* Create a device */
        _RwDirect3DObject = Direct3DCreate9(D3D_SDK_VERSION);
        if (!_RwDirect3DObject)
        {
            RWRETURN(FALSE);
        }

        /* Name */
        strcpy(_RwD3D9AdapterInformation.name, RWSTRING("D3D9"));

        /* Get video card capabilities */
        numAdapters = IDirect3D9_GetAdapterCount(_RwDirect3DObject);

        _RwD3DAdapterIndex = 0;
        _RwD3DAdapterType = D3DDEVTYPE_HAL;

        while (_RwD3DAdapterIndex < numAdapters)
        {
            if (SUCCEEDED(IDirect3D9_GetDeviceCaps(_RwDirect3DObject,
                _RwD3DAdapterIndex, _RwD3DAdapterType,    &_RwD3D9DeviceCaps)))
            {
                break;
            }

            _RwD3DAdapterIndex++;
        }

        if (_RwD3DAdapterIndex >= numAdapters)
        {
            IDirect3D9_Release(_RwDirect3DObject);

            _RwDirect3DObject = NULL;

            RWRETURN(FALSE);
        }

        /* Mode count */
        _RwD3D9AdapterInformation.modeCount = 0;
        for (i = 0; i < MAX_BACKBUFFER_FORMATS; i++)
        {
            _RwD3D9AdapterInformation.modeCount +=
                IDirect3D9_GetAdapterModeCount(_RwDirect3DObject,  _RwD3DAdapterIndex, BackBufferFormats[i]);
        }

        /* Get the first mode as the default */
        IDirect3D9_GetAdapterDisplayMode(_RwDirect3DObject,  _RwD3DAdapterIndex,   &_RwD3D9AdapterInformation.mode);

        _RwD3D9AdapterInformation.displayDepth =  D3D9FindDepth(_RwD3D9AdapterInformation.mode.Format);

        _RwD3D9AdapterInformation.flags = 0;

        DesktopDisplayMode = _RwD3D9AdapterInformation.mode;

        /* Choose a default mode, we'll take the first */
        _RwD3D9CurrentModeIndex = 0;

        /* Calculate max multisampling levels */
        D3D9CalculateMaxMultisamplingLevels();

        /* Detect CPU Info */
        _rwD3D9CPUSupportsMMX = _rwCPUHaveMMX();
        _rwD3D9CPUSupportsSSE = _rwCPUHaveSSE();
        _rwD3D9CPUSupportsSSE2 = _rwCPUHaveSSE2();
        _rwD3D9CPUSupports3DNow = _rwCPUHave3DNow();

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::D3D9DeviceSystemClose(void* pOut __RWUNUSED__, void* pInOut __RWUNUSED__, RwInt32 nIn __RWUNUSED__)
    {
        Log::Debug("RenderingEngine::D3D9DeviceSystemClose");
        if (DisplayModes != NULL)
        {
            RwFree(DisplayModes);
            DisplayModes = NULL;

            NumDisplayModes = 0;
        }

        if (_RwDirect3DObject)
        {
            IDirect3D9_Release(_RwDirect3DObject);

            _RwDirect3DObject = NULL;
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::D3D9DeviceSystemStart(void* out __RWUNUSED__, void* inOut __RWUNUSED__, RwInt32 in __RWUNUSED__)
    {
        Log::Debug("RenderingEngine::D3D9DeviceSystemStart");
        D3DADAPTER_IDENTIFIER9	adapterInfo;
        D3DDISPLAYMODE			adapterDisplayMode;
        HRESULT					hr;
        RwUInt32				behaviorFlags;
        UINT					AdapterToUse = D3DADAPTER_DEFAULT;
        D3DDEVTYPE				DeviceType = D3DDEVTYPE_HAL;
        D3DADAPTER_IDENTIFIER9	Identifier;
        UINT					Adapter;
        FILE* pFile = NULL;
        RwBool PerfHUD = FALSE;
        RwInt32 PerfInt = 0;


        IDirect3D9_GetAdapterDisplayMode(_RwDirect3DObject,
            _RwD3DAdapterIndex,
            &adapterDisplayMode);

        memset(&Present, 0, sizeof(D3DPRESENT_PARAMETERS));

        D3D9SetPresentParameters(&_RwD3D9AdapterInformation.mode,
            _RwD3D9AdapterInformation.flags,
            adapterDisplayMode.Format);

        Present.hDeviceWindow = *WindowHandle;

        if (0)
        {
            Present.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        }
        else
        {
            Present.Flags = 0;
        }

        IDirect3D9_GetDeviceCaps(_RwDirect3DObject, _RwD3DAdapterIndex,
            _RwD3DAdapterType, &_RwD3D9DeviceCaps);

        IDirect3D9_GetAdapterIdentifier(_RwDirect3DObject,
            _RwD3DAdapterIndex,
            0, &adapterInfo);

        /*
         * Check for doggy video cards
         */

         /* ATI */
        if (adapterInfo.VendorId == 0x00001002)
        {
            /* Radeon 7500 */
            if (adapterInfo.DeviceId == 0x00005157)
            {
                /* Remove support for stream offset */
                _RwD3D9DeviceCaps.DevCaps2 &= ~D3DDEVCAPS2_STREAMOFFSET;
            }
        }

        /* Create device */
        behaviorFlags = 0;

        if (EnableMultithreadSafe)
        {
            behaviorFlags |= D3DCREATE_MULTITHREADED;
        }

        if (EnableSoftwareVertexProcessing)
        {
            behaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

            /* Fix caps */
            _RwD3D9DeviceCaps.DevCaps &= ~(D3DDEVCAPS_HWTRANSFORMANDLIGHT);
            _RwD3D9DeviceCaps.DeclTypes = 0;
        }
        else
        {
            if (_RwD3D9DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
            {
                if (_RwD3D9DeviceCaps.DevCaps & D3DDEVCAPS_PUREDEVICE)
                {
                    behaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING |
                        D3DCREATE_PUREDEVICE;
                }
                else
                {
                    behaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
                }
            }
            else
            {
                behaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
            }
        }

        if (!fopen_s(&pFile, "./_.ph", "rt"))
        {
            fscanf_s(pFile, "%d", &PerfInt);
            if (PerfInt)
            {
                PerfHUD = TRUE;
            }
            fclose(pFile);
        }

        if (PerfHUD)
        {
            for (Adapter = 0; Adapter < IDirect3D9_GetAdapterCount(_RwDirect3DObject); Adapter++)
            {
                IDirect3D9_GetAdapterIdentifier(_RwDirect3DObject, Adapter, 0, &Identifier);

                if (strstr(Identifier.Description, "PerfHUD") != 0)
                {
                    AdapterToUse = Adapter;
                    DeviceType = D3DDEVTYPE_REF;
                    break;
                }
            }

            hr = IDirect3D9_CreateDevice(_RwDirect3DObject, AdapterToUse, D3DDEVTYPE_REF, *WindowHandle, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Present, &_RwD3DDevice);

        }
        else
        {
            hr = IDirect3D9_CreateDevice(_RwDirect3DObject, _RwD3DAdapterIndex, _RwD3DAdapterType, *WindowHandle, behaviorFlags, &Present, &_RwD3DDevice);
        }

        if (FAILED(hr))
        {
            RWRETURN(FALSE);
        }

        //if (EnableFullScreenDialogBoxMode)
        //{
            IDirect3DDevice9_SetDialogBoxMode(_RwD3DDevice, FALSE);
        //}

        /* Get window dimensions */
        if (WindowHandle)
        {
            RECT                rect;

            GetClientRect(*WindowHandle, &rect);

            _RwD3D9AdapterInformation.mode.Width = rect.right;
            _RwD3D9AdapterInformation.mode.Height = rect.bottom;
        }

        /*
         * Calling D3DDevice_GetRenderTarget and D3DDevice_GetDepthStencilSurface
         * increase the internal reference count so they need to be Released.
         */

         /* Get the render surface */
        IDirect3DDevice9_GetRenderTarget(_RwD3DDevice, 0, &RwD3D9RenderSurface);
        IDirect3DSurface9_Release(RwD3D9RenderSurface);

        /* Get the depth/stencil surface */
        IDirect3DDevice9_GetDepthStencilSurface (_RwD3DDevice, &RwD3D9DepthStencilSurface);
        IDirect3DSurface9_Release(RwD3D9DepthStencilSurface);

        /* Reset the cache */
        D3D9ClearCache();

        /*
         * Open some RenderWare stuff
         */

        _rwD3D9RasterOpen();

        rwD3D9Im2DRenderOpen();

        _rwD3D9RenderStateOpen();

        _rwD3D9VertexBufferManagerOpen();

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::D3D9DeviceSystemFinalizeStart(void* out __RWUNUSED__, void* inOut __RWUNUSED__, RwInt32 in __RWUNUSED__)
    {
        Log::Debug("RenderingEngine::D3D9DeviceSystemFinalizeStart");
        /* initialise the FP with single precision mode
         * saving the control register
         */
        _rwProcessorInitialize();


        if (_rwD3D9CPUSupportsSSE)
        {
            RwUInt32 temp;

            _asm
            {
                stmxcsr     temp
                mov         eax, temp
                or eax, 0x8000
                mov         temp, eax
                ldmxcsr     temp
            }
        }

        if (_rwD3D9CPUSupportsSSE)
        {
            /* set the matrix-vector products to the new SSE code */
            if (_rwD3D9CPUSupportsSSE2 &&
                _rwD3D9CPUSupports3DNow == FALSE)
            {
                _rwVectorSetMultFn(_rwIntelSSEP4V3dTransformPoint,
                    _rwIntelSSEP4V3dTransformPoints,
                    _rwIntelSSEP4V3dTransformVector,
                    _rwIntelSSEP4V3dTransformVectors);
            }
            else
            {
                _rwVectorSetMultFn(_rwIntelSSEV3dTransformPoint,
                    _rwIntelSSEV3dTransformPoints,
                    _rwIntelSSEV3dTransformVector,
                    _rwIntelSSEV3dTransformVectors);
            }
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::D3D9DeviceSystemStop(void* out __RWUNUSED__, void* inOut __RWUNUSED__, RwInt32 in __RWUNUSED__)
    {
        Log::Debug("RenderingEngine::D3D9DeviceSystemStop");
        RwUInt32 n;

        SystemStarted = FALSE;

        if (DisplayModes != NULL)
        {
            RwFree(DisplayModes);
            DisplayModes = NULL;

            NumDisplayModes = 0;
        }

        /* Restore the saved floating point control register */
        _rwProcessorRelease();

        /* Disable SSE flush-to-zero */

        if (_rwD3D9CPUSupportsSSE)
        {
            RwUInt32 temp;

            _asm
            {
                stmxcsr     temp
                mov         eax, temp
                and eax, not 0x8000
                mov         temp, eax
                ldmxcsr     temp
            }
        }

        /* Release the matrix cache */
        for (n = 0; n < 260; n++)
        {
            if (LastMatrixUsed[n] != NULL)
            {
                RwFreeListFree(MatrixFreeList, LastMatrixUsed[n]);
                LastMatrixUsed[n] = NULL;
            }
        }

        RwFreeListDestroy(MatrixFreeList);
        MatrixFreeList = NULL;

        /* Release the lights cache */
        MaxNumLights = 0;
        if (LightsCache != NULL)
        {
            RwFree(LightsCache);
            LightsCache = NULL;
        }

        /* Release active resources */
        for (n = 0; n < RWD3D9_MAX_TEXTURE_STAGES; n++)
        {
            IDirect3DDevice9_SetTexture(_RwD3DDevice, n, NULL);
        }

        IDirect3DDevice9_SetIndices(_RwD3DDevice, NULL);

        for (n = 0; n < MAX_STREAMS; n++)
        {
            IDirect3DDevice9_SetStreamSource(_RwD3DDevice, n, NULL, 0, 0);
        }

        IDirect3DDevice9_SetPixelShader(_RwD3DDevice, NULL);

        IDirect3DDevice9_SetVertexDeclaration(_RwD3DDevice, NULL);

        IDirect3DDevice9_SetVertexShader(_RwD3DDevice, NULL);

        /* Release vertex declarations */
        for (n = 0; n < NumVertexDeclarations; n++)
        {
            if (VertexDeclarations[n].vertexdeclaration != NULL)
            {
                while (IDirect3DVertexDeclaration9_Release(VertexDeclarations[n].vertexdeclaration) > 0);
            }

            if (VertexDeclarations[n].elements != NULL)
            {
                RwFree(VertexDeclarations[n].elements);
            }
        }

        if (VertexDeclarations != NULL)
        {
            RwFree(VertexDeclarations);
            VertexDeclarations = NULL;
        }

        NumVertexDeclarations = 0;
        MaxVertexDeclarations = 0;

        /* Close any remaining surfaces */
        _rwD3D9RasterClose();

        _rwD3D9VertexBufferManagerClose();

        _rwD3D9Im2DRenderClose();

        IDirect3DDevice9_Release(_RwD3DDevice);
        _RwD3DDevice = NULL;

        RWRETURN(TRUE);
    }

    void RenderingEngine::D3D9CreateDisplayModesList(void)
    {
        Log::Debug("RenderingEngine::D3D9CreateDisplayModesList");
        RwInt32 i, format, n, maxmodesformat;

        if (DisplayModes)
        {
            DisplayModes = ((rxD3D9DisplayMode*)RwRealloc(DisplayModes, (1 + _RwD3D9AdapterInformation.modeCount) * sizeof(rxD3D9DisplayMode),
                rwID_DRIVERMODULE | rwMEMHINTDUR_GLOBAL | rwMEMHINTFLAG_RESIZABLE));
        }
        else
        {
            DisplayModes = ((rxD3D9DisplayMode*)RwMalloc((1 + _RwD3D9AdapterInformation.modeCount) * sizeof(rxD3D9DisplayMode),
                rwID_DRIVERMODULE | rwMEMHINTDUR_GLOBAL | rwMEMHINTFLAG_RESIZABLE));
        }

        IDirect3D9_GetAdapterDisplayMode(_RwDirect3DObject, _RwD3DAdapterIndex, &DisplayModes[0].mode);

        if (D3D9FindDepth(DisplayModes[0].mode.Format) != 0)
        {
            DisplayModes[0].flags = 0;

            NumDisplayModes = 1;
        }
        else
        {
            NumDisplayModes = 0;
        }

        for (format = 0; format < MAX_BACKBUFFER_FORMATS; format++)
        {
            maxmodesformat = IDirect3D9_GetAdapterModeCount(_RwDirect3DObject, _RwD3DAdapterIndex,
                BackBufferFormats[format]);

            for (i = 0; i < maxmodesformat; i++)
            {
                IDirect3D9_EnumAdapterModes(_RwDirect3DObject, _RwD3DAdapterIndex,
                    BackBufferFormats[format], i, &DisplayModes[NumDisplayModes].mode);

                /* Find a previous similar mode */
                for (n = 1; n < NumDisplayModes; n++)
                {
                    if (DisplayModes[NumDisplayModes].mode.Width == DisplayModes[n].mode.Width
                        && DisplayModes[NumDisplayModes].mode.Height == DisplayModes[n].mode.Height
                        && DisplayModes[NumDisplayModes].mode.Format == DisplayModes[n].mode.Format)
                    {
                        break;
                    }
                }

                if (n >= NumDisplayModes)
                {
                    DisplayModes[NumDisplayModes].flags = rwVIDEOMODEEXCLUSIVE;

                    NumDisplayModes++;
                }
                else
                {
                    if (DisplayModes[n].mode.RefreshRate < DisplayModes[NumDisplayModes].mode.RefreshRate)
                    {
                        DisplayModes[n].mode.RefreshRate = DisplayModes[NumDisplayModes].mode.RefreshRate;
                    }
                }
            }
        }

        if (NumDisplayModes < (1 + _RwD3D9AdapterInformation.modeCount))
        {
            DisplayModes = ((rxD3D9DisplayMode*)RwRealloc(DisplayModes, NumDisplayModes * sizeof(rxD3D9DisplayMode),
                rwID_DRIVERMODULE | rwMEMHINTDUR_GLOBAL | rwMEMHINTFLAG_RESIZABLE));
        }

        RWRETURNVOID();
    }

    RwBool RenderingEngine::D3D9System(RwInt32 request, void* out, void* inOut, RwInt32 in)
    {
        switch (request)
        {
        case rwDEVICESYSTEMUSEMODE:
            if (!SystemStarted && (in >= 0) && (in < NumDisplayModes))
            {
                _RwD3D9CurrentModeIndex = in;
                _RwD3D9AdapterInformation.mode = DisplayModes[in].mode;
                _RwD3D9AdapterInformation.flags = DisplayModes[in].flags;
                _RwD3D9AdapterInformation.displayDepth = D3D9FindDepth(DisplayModes[in].mode.Format);

                /* Calculate max multisampling levels */
                D3D9CalculateMaxMultisamplingLevels();

                RWRETURN(TRUE);
            }
            else
            {
                RWRETURN(FALSE);
            }

        case rwDEVICESYSTEMGETNUMMODES:
            if (!DisplayModes)
            {
                D3D9CreateDisplayModesList();
            }

            *((RwInt32*)out) = NumDisplayModes;
            RWRETURN(TRUE);

        case rwDEVICESYSTEMGETMODEINFO:
            if (!DisplayModes)
            {
                D3D9CreateDisplayModesList();
            }

            if ((in >= 0) && (in < NumDisplayModes))
            {
                RwVideoMode* videoMode = (RwVideoMode*)out;

                videoMode->width = DisplayModes[in].mode.Width;
                videoMode->height = DisplayModes[in].mode.Height;
                videoMode->depth = D3D9FindDepth(DisplayModes[in].mode.Format);
                videoMode->flags = ((RwVideoModeFlag) DisplayModes[in].flags);
                videoMode->refRate = DisplayModes[in].mode.RefreshRate;
                videoMode->format = rwD3D9FindRWFormat(DisplayModes[in].mode.Format);

                RWRETURN(TRUE);
            }
            else
            {
                RWRETURN(FALSE);
            }

        case rwDEVICESYSTEMGETMODE:
            *((RwInt32*)out) = _RwD3D9CurrentModeIndex;
            RWRETURN(TRUE);

        case rwDEVICESYSTEMFOCUS:
            if (_RwD3DDevice != NULL)
            {
                if (!Present.Windowed)
                {
                    if (in)
                    {
                        ShowWindow(*WindowHandle, SW_RESTORE);
                    }
                    else
                    {
                        ShowWindow(*WindowHandle, SW_HIDE);
                    }
                }
                RWRETURN(TRUE);
            }
            RWRETURN(FALSE);

            /*** Register in the library block ***/

        case rwDEVICESYSTEMREGISTER:
        {
            RwDevice* D3D9DriverDevice = _rwDeviceGetHandle();
            RwDevice* deviceOut = (RwDevice*)out;
            RwMemoryFunctions* memoryFunctions = (RwMemoryFunctions*)inOut;

            *deviceOut = *D3D9DriverDevice;
            dgGGlobals.memFuncs = memoryFunctions;

            RWRETURN(TRUE);
        }

        /******************* Opening/Closing **********************/

        case rwDEVICESYSTEMOPEN:
            RWRETURN(D3D9DeviceSystemOpen(out, inOut, in));

        case rwDEVICESYSTEMCLOSE:
            RWRETURN(D3D9DeviceSystemClose(out, inOut, in));

            /********* Selecting a rendering sub system ********/

        case rwDEVICESYSTEMGETNUMSUBSYSTEMS:
        {
            RwInt32* numSubSystems = (RwInt32*)out;
            *numSubSystems = IDirect3D9_GetAdapterCount(_RwDirect3DObject);

            RWRETURN(TRUE);
        }

        case rwDEVICESYSTEMGETSUBSYSTEMINFO:
        {
            RwInt32             subSystemIndex = (RwInt32)in;
            RwSubSystemInfo* subSystemInfo = (RwSubSystemInfo*)out;
            D3DADAPTER_IDENTIFIER9 adapterInfo;

            IDirect3D9_GetAdapterIdentifier(_RwDirect3DObject, subSystemIndex,
                0, &adapterInfo);

            strcpy(subSystemInfo->name, adapterInfo.Description);
            RWRETURN(TRUE);
        }

        case rwDEVICESYSTEMGETCURRENTSUBSYSTEM:
        {
            RwInt32* currentSubSystem = (RwInt32*)out;

            *currentSubSystem = _RwD3DAdapterIndex;

            RWRETURN(TRUE);
        }

        case rwDEVICESYSTEMSETSUBSYSTEM:
        {
            RwInt32 i;

            _RwD3DAdapterIndex = in;

            /* Mode count */
            _RwD3D9AdapterInformation.modeCount = 0;
            for (i = 0; i < MAX_BACKBUFFER_FORMATS; i++)
            {
                _RwD3D9AdapterInformation.modeCount += IDirect3D9_GetAdapterModeCount(_RwDirect3DObject,
                        _RwD3DAdapterIndex, BackBufferFormats[i]);
            }

            /* Get the current mode as the default */
            IDirect3D9_GetAdapterDisplayMode(_RwDirect3DObject, _RwD3DAdapterIndex,
                &_RwD3D9AdapterInformation.mode);

            /* Recreate list of modes */
            D3D9CreateDisplayModesList();

            /* Calculate max multisampling levels */
            D3D9CalculateMaxMultisamplingLevels();

            RWRETURN(TRUE);
        }

        /************** Starting stopping ******************/

        /* Start means that this bit of hardware has been selected for
         * rendering - so get it into a state ready for rendering */

        case rwDEVICESYSTEMSTART:
        {
            SystemStarted = D3D9DeviceSystemStart(out, inOut, in);

            RWRETURN(SystemStarted);
        }

        case rwDEVICESYSTEMSTOP:
            RWRETURN(D3D9DeviceSystemStop(out, inOut, in));

            /************* standard device functions ************************/

        case rwDEVICESYSTEMSTANDARDS:
            RWRETURN(D3D9DeviceSystemStandards(out, inOut, in));

        case rwDEVICESYSTEMINITPIPELINE:
            break;

        case rwDEVICESYSTEMGETTEXMEMSIZE:
        {
            *((RwUInt32*)out) = IDirect3DDevice9_GetAvailableTextureMem(_RwD3DDevice);

            RWRETURN(TRUE);
        }
        break;

        case rwDEVICESYSTEMFINALIZESTART:
        {
            RWRETURN(D3D9DeviceSystemFinalizeStart(out, inOut, in));
        }
        break;

        case rwDEVICESYSTEMINITIATESTOP:
            break;

        case rwDEVICESYSTEMGETMAXTEXTURESIZE:
        {
            RwInt32* maxTextureSize = (RwInt32*)out;


            /* return the maximum dimension of a texture */
            if (_RwD3D9DeviceCaps.MaxTextureWidth >= _RwD3D9DeviceCaps.MaxTextureHeight)
            {
                *maxTextureSize = _RwD3D9DeviceCaps.MaxTextureWidth;
            }
            else
            {
                *maxTextureSize = _RwD3D9DeviceCaps.MaxTextureHeight;
            }

            RWRETURN(TRUE);
        }
        break;

        case rwDEVICESYSTEMRXPIPELINEREQUESTPIPE:
            break;

        case rwDEVICESYSTEMGETID:
        {
            *(RwUInt16*)out = rwDEVICE_D3D9;
            RWRETURN(TRUE);
        }
        break;
        }

        RWRETURN(FALSE);
    }

    RwDevice* RenderingEngine::_rwDeviceGetHandle(void)
    {
        static RwDevice     rwD3D9DriverDevice = {
        1.0f,                   /* default gamma correction */
        D3D9System,
        0.0f, 1.0f, /* near and far Z buffer limits */

        _rwD3D9RWSetRenderState, /* RwRenderStateSetFunction */
        _rwD3D9RWGetRenderState, /* RwRenderStateGetFunction */

        _rwD3D9Im2DRenderLine,  /* RwIm2DRenderLineFunction */
        _rwD3D9Im2DRenderTriangle, /* RwIm2DRenderTriangleFunction */
        _rwD3D9Im2DRenderPrimitive, /* RwIm2DRenderPrimitiveFunction */
        _rwD3D9Im2DRenderIndexedPrimitive, /* RwIm2DRenderIndexedPrimitiveFunction */

        NULL, NULL, NULL, NULL  /* These get set up when the 3D Immediate mode module is setup */
        };

        RWRETURN(&rwD3D9DriverDevice);
    }

    void RenderingEngine::D3D9ClearCacheShaders()
    {
        RwUInt32 n;

        _rwD3D9LastFVFUsed = 0xffffffff;
        _rwD3D9LastVertexDeclarationUsed = (void*)0xffffffff;
        _rwD3D9LastVertexShaderUsed = (void*)0xffffffff;
        _rwD3D9LastPixelShaderUsed = (void*)0xffffffff;

        _rwD3D9LastIndexBufferUsed = (void*)0xffffffff;

        for (n = 0; n < MAX_STREAMS; n++)
        {
            LastVertexStreamUsed[n].vertexBuffer = (void*)0xffffffff;
            LastVertexStreamUsed[n].offset = 0;
            LastVertexStreamUsed[n].stride = 0;
        }

        RWRETURNVOID();
    }

    void RenderingEngine::D3D9ClearCacheMatrix()
    {
        LastWorldMatrixUsedIdentity = FALSE;

        if (MatrixFreeList != NULL)
        {
            RwUInt32 n;

            for (n = 0; n < 260; n++)
            {
                if (LastMatrixUsed[n] != NULL)
                {
                    RwFreeListFree(MatrixFreeList, LastMatrixUsed[n]);
                    LastMatrixUsed[n] = NULL;
                }
            }
            RwFreeListPurge(MatrixFreeList);
        }
        else
        {
            MatrixFreeList = RwFreeListCreate(sizeof(D3DMATRIX), 15, 16,
                rwID_DRIVERMODULE | rwMEMHINTDUR_EVENT);

            memset(LastMatrixUsed, 0, sizeof(D3DMATRIX*) * 260);
        }

        /* World matrix always created */
        LastMatrixUsed[D3DTS_WORLD] = (D3DMATRIX*)RwFreeListAlloc(MatrixFreeList,
            rwID_DRIVERMODULE | rwMEMHINTDUR_EVENT);

        memcpy(LastMatrixUsed[D3DTS_WORLD], &IdentityMatrix, sizeof(D3DMATRIX));

        RWRETURNVOID();
    }

    void RenderingEngine::D3D9ClearCacheLights()
    {
        MaxNumLights = 0;

        if (LightsCache != NULL)
        {
            RwFree(LightsCache);
            LightsCache = NULL;
        }
    }

    void RenderingEngine::D3D9ClearCache()
    {
        RwUInt32 n;

        /* Reset rendertargets cache */
        CurrentDepthStencilSurface = NULL;
        for (n = 0; n < 4; n++)
        {
            CurrentRenderSurface[n] = NULL;
        }

        /* Reset the shaders cache */
        D3D9ClearCacheShaders();

        /* Reset the matrix cache */
        D3D9ClearCacheMatrix();

        /* Reset the lights cache */
        D3D9ClearCacheLights();

        RWRETURNVOID();
    }
}