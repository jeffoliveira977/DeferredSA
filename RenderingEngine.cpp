#include "RenderingEngine.h"
#include "GTADef.h"
#include "Raster.h"
#include "RenderStates.h"

std::tuple<int32_t, int32_t> GetDesktopRes()
{
    HMONITOR monitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
    MONITORINFO info = {};
    info.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(monitor, &info);
    int32_t DesktopResW = info.rcMonitor.right - info.rcMonitor.left;
    int32_t DesktopResH = info.rcMonitor.bottom - info.rcMonitor.top;
    return std::make_tuple(DesktopResW, DesktopResH);
}

int nCurrentWidth = 0;
int nCurrentHeight = 0;


RwInt32 D3D9FindDepth(D3DFORMAT format)
{
    RwInt32             depth;

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

    return rwformat;
}

#define RWRETURN(value) return(value)
#define RWRETURNVOID() return

RwUInt32 D3D9ConvR8G8B8A8ToR5G5B5(const RwRGBA* pixIn)
{
    RwUInt32    pixOut;

    pixOut = (0x00000000) | /* Err slam a 0 in here, it ain't used anyway */
        (((RwUInt32)pixIn->red << 7) & 0x7C00) |
        (((RwUInt32)pixIn->green << 2) & 0x03E0) |
        (((RwUInt32)pixIn->blue >> 3) & 0x001F);

    RWRETURN(pixOut);
}

RwUInt32 D3D9ConvR8G8B8A8ToR5G6B5(const RwRGBA* pixIn)
{
    RwUInt32    pixOut;

    pixOut = (((RwUInt32)pixIn->red << 8) & 0xF800) |
        (((RwUInt32)pixIn->green << 3) & 0x07E0) |
        ((RwUInt32)pixIn->blue >> 3);

    RWRETURN(pixOut);
}

RwUInt32 D3D9ConvR8G8B8A8ToA1R5G5B5(const RwRGBA* pixIn)
{
    RwUInt32    pixOut;


    pixOut = (((RwUInt32)pixIn->alpha << 8) & 0x8000) |
        (((RwUInt32)pixIn->red << 7) & 0x7C00) |
        (((RwUInt32)pixIn->green << 2) & 0x03E0) |
        ((RwUInt32)pixIn->blue >> 3);

    RWRETURN(pixOut);
}

RwUInt32 D3D9ConvR8G8B8A8ToA4R4G4B4(const RwRGBA* pixIn)
{
    RwUInt32    pixOut;


    pixOut = (((RwUInt32)pixIn->alpha << 8) & 0xF000) |
        (((RwUInt32)pixIn->red << 4) & 0x0F00) |
        (((RwUInt32)pixIn->green) & 0x00F0) |
        ((RwUInt32)pixIn->blue >> 4);

    RWRETURN(pixOut);
}

RwUInt32 D3D9ConvR8G8B8A8ToA8R3G3B2(const RwRGBA* pixIn)
{
    RwUInt32    pixOut;

    pixOut = ((RwUInt32)pixIn->alpha << 8) |
        (((RwUInt32)pixIn->red) & 0xE0) |
        (((RwUInt32)pixIn->green >> 3) & 0x1C) |
        ((RwUInt32)pixIn->blue >> 6);

    RWRETURN(pixOut);
}

RwUInt32 D3D9ConvR8G8B8A8ToR4G4B4(const RwRGBA* pixIn)
{
    RwUInt32    pixOut;

    pixOut = 0xF000 |
        (((RwUInt32)pixIn->red << 4) & 0x0F00) |
        (((RwUInt32)pixIn->green) & 0x00F0) |
        ((RwUInt32)pixIn->blue >> 4);

    RWRETURN(pixOut);
}

RwUInt32 D3D9ConvR8G8B8A8ToL6V5U5(const RwRGBA* pixIn)
{
    RwUInt32    pixOut;

    pixOut = (((RwUInt32)pixIn->blue << 8) & 0xFC00) |
        (((RwUInt32)pixIn->green << 2) & 0x03E0) |
        ((RwUInt32)pixIn->red >> 3);

    RWRETURN(pixOut);
}

RwUInt32 D3D9ConvR8G8B8A8ToX8R8G8B8(const RwRGBA* pixIn)
{
    RwUInt32    pixOut;

    pixOut = ((RwUInt32)0xFF << 24) |
        ((RwUInt32)pixIn->red << 16) |
        ((RwUInt32)pixIn->green << 8) |
        ((RwUInt32)pixIn->blue);

    RWRETURN(pixOut);
}

RwUInt32 D3D9ConvR8G8B8A8ToX8B8G8R8(const RwRGBA* pixIn)
{
    RwUInt32    pixOut;

    pixOut = ((RwUInt32)0xFF << 24) |
        ((RwUInt32)pixIn->blue << 16) |
        ((RwUInt32)pixIn->green << 8) |
        ((RwUInt32)pixIn->red);

    RWRETURN(pixOut);
}

RwUInt32 D3D9ConvR8G8B8A8ToA8R8G8B8(const RwRGBA* pixIn)
{
    RwUInt32    pixOut;

    pixOut = ((RwUInt32)pixIn->alpha << 24) |
        ((RwUInt32)pixIn->red << 16) |
        ((RwUInt32)pixIn->green << 8) |
        ((RwUInt32)pixIn->blue);

    RWRETURN(pixOut);
}

RwUInt32 D3D9ConvR8G8B8A8ToA8B8G8R8(const RwRGBA* pixIn)
{
    RwUInt32    pixOut;

    pixOut = ((RwUInt32)pixIn->alpha << 24) |
        ((RwUInt32)pixIn->blue << 16) |
        ((RwUInt32)pixIn->green << 8) |
        ((RwUInt32)pixIn->red);

    RWRETURN(pixOut);
}

RwUInt32 D3D9ConvR8G8B8A8ToV8U8(const RwRGBA* pixIn)
{
    RwUInt32    pixOut;

    pixOut = ((RwUInt32)pixIn->red) | ((RwUInt32)pixIn->green << 8);

    RWRETURN(pixOut);
}

RwUInt32 D3D9ConvR8G8B8A8ToL8(const RwRGBA* pixIn)
{
    RwUInt32    pixOut;

    pixOut = ((pixIn->red * 30 +
        pixIn->green * 59 +
        pixIn->blue * 11) / 100);

    pixOut = ((pixOut * (pixIn->alpha)) / 0xff);

    RWRETURN(pixOut);
}

RwUInt32 D3D9ConvR8G8B8A8ToA8L8(const RwRGBA* pixIn)
{
    RwUInt32    pixOut;

    pixOut = ((pixIn->red * 30 +
        pixIn->green * 59 +
        pixIn->blue * 11) / 100);

    pixOut = (pixOut | (pixIn->alpha << 8));

    RWRETURN(pixOut);
}

RwUInt32 D3D9ConvR8G8B8A8ToL16(const RwRGBA* pixIn)
{
    RwUInt32    pixOut;

    pixOut = (((pixIn->red * 30 +
        pixIn->green * 59 +
        pixIn->blue * 11) * (pixIn->alpha)) / (100 * 0xff));

    pixOut = (pixOut * 0xff);

    RWRETURN(pixOut);
}

RwBool _rwD3D9RGBToPixel(void* pixelOut, void* colIn, RwInt32 format)
{
    RwRGBA* rgba = (RwRGBA*)colIn;
    RwInt32 pixVal;

    switch (format & rwRASTERFORMATPIXELFORMATMASK)
    {
    case rwRASTERFORMATDEFAULT:
    {
        /* Default is 8888 */
        if (!_rwD3D9RGBToPixel(&pixVal, colIn, rwRASTERFORMAT8888))
        {
            return false;
        }
        break;
    }

    case rwRASTERFORMAT555:
        pixVal = D3D9ConvR8G8B8A8ToR5G5B5(rgba);
        break;

    case rwRASTERFORMAT1555:
        pixVal = D3D9ConvR8G8B8A8ToA1R5G5B5(rgba);
        break;

    case rwRASTERFORMAT565:
        pixVal = D3D9ConvR8G8B8A8ToR5G6B5(rgba);
        break;

    case rwRASTERFORMAT4444:
        pixVal = D3D9ConvR8G8B8A8ToA4R4G4B4(rgba);
        break;

    case rwRASTERFORMATLUM8:
        pixVal = D3D9ConvR8G8B8A8ToL8(rgba);
        break;

    case rwRASTERFORMAT8888:
        pixVal = D3D9ConvR8G8B8A8ToA8R8G8B8(rgba);
        break;

    case rwRASTERFORMAT888:
        pixVal = D3D9ConvR8G8B8A8ToX8R8G8B8(rgba);
        break;

    default:
    {
        return false;
    }
    }

    *(RwInt32*)pixelOut = pixVal;

    return true;
}

void D3D9ConvR5G5B5ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{
    pixOut->red = (RwUInt8)((pixIn >> 7) & 0xF8);
    pixOut->green = (RwUInt8)((pixIn >> 2) & 0xF8);
    pixOut->blue = (RwUInt8)((pixIn << 3) & 0xF8);
    pixOut->alpha = 0xFF;

    RWRETURNVOID();
}

void D3D9ConvR5G6B5ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{
    pixOut->red = (RwUInt8)((pixIn >> 8) & 0xF8);
    pixOut->green = (RwUInt8)((pixIn >> 3) & 0xFC);
    pixOut->blue = (RwUInt8)((pixIn << 3) & 0xF8);
    pixOut->alpha = 0xFF;
}

void D3D9ConvL6V5U5ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{
    pixOut->red = (RwUInt8)((pixIn << 3) & 0xF8);
    pixOut->green = (RwUInt8)((pixIn >> 2) & 0xF8);
    pixOut->blue = (RwUInt8)((pixIn >> 8) & 0xFC);
    pixOut->alpha = 0xFF;
}

void D3D9ConvA1R5G5B5ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{
    if (pixIn & 0x8000)
        pixOut->alpha = 0xFF;
    else
        pixOut->alpha = 0x00;

    pixOut->red = (RwUInt8)((pixIn >> 7) & 0xF8);
    pixOut->green = (RwUInt8)((pixIn >> 2) & 0xF8);
    pixOut->blue = (RwUInt8)((pixIn << 3) & 0xF8);
}

void D3D9ConvA4R4G4B4ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{
    if ((pixIn & 0xF000) == 0xF000)
        pixOut->alpha = 0xFF;
    else
        pixOut->alpha = (RwUInt8)((pixIn >> 8) & 0xF0);

    pixOut->red = (RwUInt8)((pixIn >> 4) & 0xF0);
    pixOut->green = (RwUInt8)((pixIn >> 0) & 0xF0);
    pixOut->blue = (RwUInt8)((pixIn << 4) & 0xF0);
}

void D3D9ConvA8R3G3B2ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{
    pixOut->red = (RwUInt8)((pixIn >> 0) & 0xE0);
    pixOut->green = (RwUInt8)((pixIn << 3) & 0xE0);
    pixOut->blue = (RwUInt8)((pixIn << 6) & 0xC0);
    pixOut->alpha = (RwUInt8)(pixIn >> 8);
}

void D3D9ConvR3G3B2ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{

    pixOut->red = (RwUInt8)((pixIn >> 0) & 0xE0);
    pixOut->green = (RwUInt8)((pixIn << 3) & 0xE0);
    pixOut->blue = (RwUInt8)((pixIn << 6) & 0xC0);
    pixOut->alpha = 0xFF;
}

void D3D9ConvX4R4G4B4ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{

    pixOut->red = (RwUInt8)((pixIn >> 4) & 0xF0);
    pixOut->green = (RwUInt8)((pixIn >> 0) & 0xF0);
    pixOut->blue = (RwUInt8)((pixIn << 4) & 0xF0);
    pixOut->alpha = 0xFF;
}

void D3D9ConvX8R8G8B8ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{
    pixOut->red = (RwUInt8)((pixIn >> 16) & 0xFF);
    pixOut->green = (RwUInt8)((pixIn >> 8) & 0xFF);
    pixOut->blue = (RwUInt8)((pixIn) & 0xFF);
    pixOut->alpha = 0xFF;
}

void D3D9ConvX8B8G8R8ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{

    pixOut->red = (RwUInt8)((pixIn) & 0xFF);
    pixOut->green = (RwUInt8)((pixIn >> 8) & 0xFF);
    pixOut->blue = (RwUInt8)((pixIn >> 16) & 0xFF);
    pixOut->alpha = 0xFF;
}

void D3D9ConvV8U8ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{
    pixOut->red = (RwUInt8)((pixIn) & 0xFF);
    pixOut->green = (RwUInt8)((pixIn >> 8) & 0xFF);
    pixOut->blue = 0;
    pixOut->alpha = 0xFF;
}

void D3D9ConvA8R8G8B8ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{
    pixOut->red = (RwUInt8)((pixIn >> 16) & 0xFF);
    pixOut->green = (RwUInt8)((pixIn >> 8) & 0xFF);
    pixOut->blue = (RwUInt8)((pixIn) & 0xFF);
    pixOut->alpha = (RwUInt8)((pixIn >> 24) & 0xFF);
}

void D3D9ConvA8B8G8R8ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{
    pixOut->red = (RwUInt8)((pixIn) & 0xFF);
    pixOut->green = (RwUInt8)((pixIn >> 8) & 0xFF);
    pixOut->blue = (RwUInt8)((pixIn >> 16) & 0xFF);
    pixOut->alpha = (RwUInt8)((pixIn >> 24) & 0xFF);
}

void D3D9ConvL8ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{

    pixOut->red = (RwUInt8)pixIn;
    pixOut->green = (RwUInt8)pixIn;
    pixOut->blue = (RwUInt8)pixIn;
    pixOut->alpha = 0xFF;
}

void D3D9ConvA8ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{

    pixOut->red = 0xFF;
    pixOut->green = 0xFF;
    pixOut->blue = 0xFF;
    pixOut->alpha = (RwUInt8)pixIn;
}

void D3D9ConvA4L4ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{
    pixOut->red = (RwUInt8)(pixIn << 4);
    pixOut->green = (RwUInt8)(pixIn << 4);
    pixOut->blue = (RwUInt8)(pixIn << 4);

    if (pixIn >= 0xF0)
    {
        pixOut->alpha = 0xFF;
    }
    else
    {
        pixOut->alpha = (RwUInt8)(pixIn & 0xF0);
    }
}

void D3D9ConvA8L8ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{
    pixOut->red = (RwUInt8)(pixIn & 0xFF);
    pixOut->green = (RwUInt8)(pixIn & 0xFF);
    pixOut->blue = (RwUInt8)(pixIn & 0xFF);
    pixOut->alpha = (RwUInt8)((pixIn >> 8) & 0xFF);
}

void D3D9ConvL16ToR8G8B8A8(RwRGBA* pixOut, RwUInt32 pixIn)
{
    pixOut->red = (RwUInt8)((pixIn >> 8) & 0xFF);
    pixOut->green = (RwUInt8)((pixIn >> 8) & 0xFF);
    pixOut->blue = (RwUInt8)((pixIn >> 8) & 0xFF);
    pixOut->alpha = 0xFF;
}


RwBool _rwD3D9PixelToRGB(void* rgbOut, void* pixel, RwInt32 format)
{
    RwRGBA* rgba = (RwRGBA*)rgbOut;

    if (format & rwRASTERFORMATPAL8)
    {
        *rgba = *((RwRGBA*)pixel);
    }
    else
    {
        switch (format & rwRASTERFORMATPIXELFORMATMASK)
        {
            /*
            case rwRASTERFORMATDEFAULT:
                D3D9ConvA8R8G8B8ToR8G8B8A8(rgba, *(RwInt32 *)pixel);
                break;
            */

        case rwRASTERFORMAT555:
            D3D9ConvR5G5B5ToR8G8B8A8(rgba, *(RwInt32*)pixel);
            break;

        case rwRASTERFORMAT1555:
            D3D9ConvA1R5G5B5ToR8G8B8A8(rgba, *(RwInt32*)pixel);
            break;

        case rwRASTERFORMAT565:
            D3D9ConvR5G6B5ToR8G8B8A8(rgba, *(RwInt32*)pixel);
            break;

        case rwRASTERFORMAT4444:
            D3D9ConvA4R4G4B4ToR8G8B8A8(rgba, *(RwInt32*)pixel);
            break;

        case rwRASTERFORMATLUM8:
            D3D9ConvL8ToR8G8B8A8(rgba, *(RwInt32*)pixel);
            break;

        case rwRASTERFORMAT8888:
            D3D9ConvA8R8G8B8ToR8G8B8A8(rgba, *(RwInt32*)pixel);
            break;

        case rwRASTERFORMAT888:
            D3D9ConvX8R8G8B8ToR8G8B8A8(rgba, *(RwInt32*)pixel);
            break;

        default:
        {
            return FALSE;
        }
        }
    }

    return TRUE;
}


bool bWindowIconic;
bool bChangeVideo;

RwBool& NeedToCopyFromBackBuffer = *(RwBool*)0xC97C34;
RwBool& InsideScene = *(RwBool*)0xC97C54;
RwUInt32& SelectedMultisamplingLevels = *(RwUInt32*)0x008E2430;
RwUInt32& SelectedMultisamplingLevelsNonMask = *(RwUInt32*)0x008E2438;

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

RwUInt32& _rwD3D9LastFVFUsed = *(RwUInt32*)0x8E2440;
void*& _rwD3D9LastVertexDeclarationUsed = *(void**)0x8E2444;
void*& _rwD3D9LastVertexShaderUsed = *(void**)0x8E2448;
void*& _rwD3D9LastPixelShaderUsed = *(void**)0x8E244C;
void*& _rwD3D9LastIndexBufferUsed = *(void**)0x8E2450;

typedef D3DMATRIX* LPD3DMATRIX;
RwBool& LastWorldMatrixUsedIdentity = *(RwBool*)0xC97C68;
//LPD3DMATRIX(&LastMatrixUsed)[260] = *(LPD3DMATRIX(*)[260])0xC97C70;
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

#define MAX_STREAMS  4

RxD3D9VertexStream(&LastVertexStreamUsed)[MAX_STREAMS] = *(RxD3D9VertexStream(*)[MAX_STREAMS])0x00C97BD8;

typedef struct rxD3D9DisplayMode rxD3D9DisplayMode;
struct rxD3D9DisplayMode
{
    D3DDISPLAYMODE      mode;
    RwInt32             flags;
};

rxD3D9DisplayMode*& DisplayModes = *(rxD3D9DisplayMode**)0XC97C48; /* Just used during the selection process */

std::unordered_map<int, rxD3D9DisplayMode> mDisplayModes;
std::unordered_map<int, LPD3DMATRIX> LastMatrixUsed;

#define MAX_BACKBUFFER_FORMATS  3
const D3DFORMAT BackBufferFormats[MAX_BACKBUFFER_FORMATS] =
{
    D3DFMT_R5G6B5,
    D3DFMT_X8R8G8B8,
    D3DFMT_A2R10G10B10
};

struct RwRwDeviceGlobals
{
    RwCamera* curCamera;
    RwMemoryFunctions* memFuncs;
};

RwRwDeviceGlobals& dgGGlobals = *(RwRwDeviceGlobals*)0xC9BCC0;

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
                SAFE_DELETE(buffer.vb);
        }
        mDynamicVertexBufferList.clear();

        for (auto& buffer : mVertexBufferList)
        {
            if (buffer)
                SAFE_DELETE(buffer);
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
            numelements++;

        numelements++;

        uint32_t sizeElements = numelements * sizeof(D3DVERTEXELEMENT9);
        uint32_t found = 0;

        for (size_t i = 0; i < m_vertexDeclarations.size(); i++)
        {
            if (m_vertexDeclarations[i].numElements == numelements)
            {
                if (std::memcmp(m_vertexDeclarations[i].elements, elements, sizeElements) == 0)
                {
                    found = i;
                    break;
                }
            }
        }

        if (found == 0)
        {
            auto hr = _RwD3DDevice->CreateVertexDeclaration(elements, vertexdeclaration);

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
            IDirect3DVertexDeclaration9_AddRef(m_vertexDeclarations[found].vertexdeclaration);
            *vertexdeclaration = m_vertexDeclarations[found].vertexdeclaration;

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
                auto cube = reinterpret_cast<IDirect3DCubeTexture9*>(rasExt->texture);
                cube->GetCubeMapSurface((D3DCUBEMAP_FACES)rasExt->face, 0, &surface);
            }
            else
                rasExt->texture->GetSurfaceLevel(0, &surface);
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
            return SUCCEEDED(_RwD3DDevice->SetRenderTarget(index, rendertarget));
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

    bool RenderingEngine::SetDepthStencilRaster(RwRaster* zRaster)
    {
        if (zRaster)
        {
            auto zTopRaster = (zRaster);
            auto zRasterExt = RASTEREXTFROMCONSTRASTER(zTopRaster);

            if ((RwRasterGetType(zTopRaster) & rwRASTERTYPEMASK) == rwRASTERTYPEZBUFFER)
                SetDepthStencilSurface((LPSURFACE)zRasterExt->texture);
            else
            {
                LPSURFACE surfaceLevelZ;
                zRasterExt->texture->GetSurfaceLevel(0, &surfaceLevelZ);

                SetDepthStencilSurface(surfaceLevelZ);
                surfaceLevelZ->Release();
            }
            return true;
        }
        else
        {
            SetDepthStencilSurface(nullptr);
            return false;
        }
    }

    IDirect3DSurface9* RenderingEngine::GetCurrentD3DRenderTarget(uint32_t index)
    {
        return CurrentRenderSurface[index];
    }

    bool RenderingEngine::CreateVertexShader(DWORD* function, IDirect3DVertexShader9** shader)
    {
        auto hr = RwD3DDevice->CreateVertexShader(function, shader);

        if (SUCCEEDED(hr))
            _rwD3D9LastVertexShaderUsed = (void*)0xffffffff;

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
            _rwD3D9LastPixelShaderUsed = (void*)0xffffffff;

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

    void RenderingEngine::DrawIndexedPrimitiveUP(uint32_t primitiveType, uint32_t minIndex, uint32_t numVertices, uint32_t primitiveCount, const void* indexData, const void* vertexStreamZeroData, uint32_t vertexStreamZeroStride)
    {
        SetStreamSource(0, nullptr, 0, 0);
        SetIndices(nullptr);

        RenderStates::RenderStateFlushCache();

        RwD3DDevice->DrawIndexedPrimitiveUP((D3DPRIMITIVETYPE)primitiveType, minIndex, numVertices, primitiveCount, indexData, D3DFMT_INDEX16,
            vertexStreamZeroData, vertexStreamZeroStride);
    }

    void RenderingEngine::DrawPrimitiveUP(uint32_t primitiveType, uint32_t primitiveCount, const void* vertexStreamZeroData, uint32_t vertexStreamZeroStride)
    {
        SetStreamSource(0, nullptr, 0, 0);
        SetIndices(nullptr);

        RenderStates::RenderStateFlushCache();

        RwD3DDevice->DrawPrimitiveUP((D3DPRIMITIVETYPE)primitiveType, primitiveCount, vertexStreamZeroData, vertexStreamZeroStride);
    }

    void RenderingEngine::DrawIndexedPrimitive(uint32_t primitiveType, int32_t baseVertexIndex, uint32_t minIndex, uint32_t numVertices, uint32_t startIndex, uint32_t primitiveCount)
    {
        RenderStates::RenderStateFlushCache();

        RwD3DDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)primitiveType, baseVertexIndex, minIndex, numVertices, startIndex, primitiveCount);
    }

    void RenderingEngine::DrawPrimitive(uint32_t primitiveType, uint32_t startVertex, uint32_t primitiveCount)
    {
        RenderStates::RenderStateFlushCache();

        RwD3DDevice->DrawPrimitive((D3DPRIMITIVETYPE)primitiveType, startVertex, primitiveCount);
    }

    bool RenderingEngine::SetTransform(uint32_t state, const void* matrix)
    {
        if (state > 259)
            return false;

        HRESULT hr;
        if (matrix)
        {
            if (LastMatrixUsed[state] && memcmp(LastMatrixUsed[state], matrix, sizeof(D3DMATRIX)) == 0)
                return true;

            if (LastMatrixUsed[state] == nullptr)
                LastMatrixUsed[state] = new D3DMATRIX;

            memcpy(LastMatrixUsed[state], matrix, sizeof(D3DMATRIX));
            hr = RwD3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)state, (const D3DMATRIX*)matrix);

            if (state == D3DTS_WORLD)
                LastWorldMatrixUsedIdentity = false;

        }
        else
        {
            if (LastMatrixUsed[state] && memcmp(LastMatrixUsed[state], &IdentityMatrix, sizeof(D3DMATRIX)) == 0)
                return true;

            if (LastMatrixUsed[state] == nullptr)
                LastMatrixUsed[state] = LastMatrixUsed[state] = new D3DMATRIX;;

            memcpy(LastMatrixUsed[state], &IdentityMatrix, sizeof(D3DMATRIX));
            hr = RwD3DDevice->SetTransform((D3DTRANSFORMSTATETYPE)state, &IdentityMatrix);

            if (state == D3DTS_WORLD)
                LastWorldMatrixUsedIdentity = true;
        }

        return SUCCEEDED(hr);
    }

    void RenderingEngine::GetTransform(uint32_t state, void* matrix)
    {
        if (matrix && state < 260)
        {
            if (LastMatrixUsed[state])
                memcpy(matrix, LastMatrixUsed[state], sizeof(D3DMATRIX));
            else
                memcpy(matrix, &IdentityMatrix, sizeof(D3DMATRIX));
        }
    }

    bool RenderingEngine::SetTransformWorld(const RwMatrix* matrix)
    {
        static D3DMATRIX d3dMatrix =
        { 0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 1.0f };

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
                !(memcmp(LastMatrixUsed[D3DTS_WORLD], &d3dMatrix, sizeof(D3DMATRIX)) == 0))
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
                _rwD3D9LastVertexShaderUsed = (void*)0xffffffff;
            else
                _rwD3D9LastVertexShaderUsed = shader;
        }
    }

    void RenderingEngine::SetPixelShader(IDirect3DPixelShader9* shader)
    {
        if (_rwD3D9LastPixelShaderUsed != shader)
        {
            auto hr = RwD3DDevice->SetPixelShader(shader);
            if (FAILED(hr))
                _rwD3D9LastPixelShaderUsed = (void*)0xffffffff;
            else
                _rwD3D9LastPixelShaderUsed = shader;
        }
    }

    void RenderingEngine::SetFVF(uint32_t fvf)
    {
        if (_rwD3D9LastFVFUsed != fvf)
        {
            auto hr = RwD3DDevice->SetFVF(fvf);
            if (FAILED(hr))
                _rwD3D9LastFVFUsed = 0xffffffff;
            else
                _rwD3D9LastFVFUsed = fvf;

            _rwD3D9LastVertexDeclarationUsed = (void*)0xffffffff;
        }
    }

    void RenderingEngine::SetVertexDeclaration(IDirect3DVertexDeclaration9* vertexDeclaration)
    {
        if (_rwD3D9LastVertexDeclarationUsed != vertexDeclaration)
        {
            auto hr = RwD3DDevice->SetVertexDeclaration(vertexDeclaration);
            if (FAILED(hr))
                _rwD3D9LastVertexDeclarationUsed = (void*)0xffffffff;
            else
                _rwD3D9LastVertexDeclarationUsed = vertexDeclaration;

            _rwD3D9LastFVFUsed = 0xffffffff;
        }
    }

    RwBool RenderingEngine::CameraClear(void* camera, void* color, RwInt32 clearFlags)
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
        topRaster = (raster);


        /* Check if the size of the main window has changed */
        GetClientRect(*WindowHandle, &rect);

        bCurrIconic = IsIconic(*WindowHandle);
        if (!bCurrIconic && bWindowIconic)
            bIconicRestore = TRUE;

        if ((RwUInt32)rect.right != _RwD3D9AdapterInformation.mode.Width
            || (RwUInt32)rect.bottom != _RwD3D9AdapterInformation.mode.Height ||
            bIconicRestore)
        {
            ReleaseVideoMemory();

            hr = _RwD3DDevice->TestCooperativeLevel();

            if (Present.Windowed)
            {
                Present.BackBufferWidth = rect.right;
                Present.BackBufferHeight = rect.bottom;
            }

            hr = _RwD3DDevice->Reset(&Present);
            if (FAILED(hr))
                Log::Error("RenderingEngine::CameraBeginUpdate - failed to reset device");

            RestoreVideoMemory();
        }

        /* Check raster type */
        if (rwRASTERTYPECAMERATEXTURE == topRaster->cType)
        {
            SetRenderTarget(0, topRaster);
            SetDepthStencilRaster(RwCameraGetZRaster((RwCamera*)camera));

            D3DVIEWPORT9  viewport = { raster->nOffsetX, raster->nOffsetY,
                raster->width, raster->height, 0,  1 };
            _RwD3DDevice->SetViewport(&viewport);

            auto d3d9ClearFlags = clearFlagsConvTable[clearFlags];

            if (d3d9ClearFlags & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL))
            {
                if (RwCameraGetZRaster((RwCamera*)camera) == NULL)
                {
                    d3d9ClearFlags &= ~(D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
                }
            }
            _RwD3DDevice->Clear(0, NULL, d3d9ClearFlags, packedColor, 1, 0);
        }
        else
        {
            _SetRenderTarget(0, RwD3D9RenderSurface);
            SetDepthStencilSurface(RwD3D9DepthStencilSurface);

            D3DVIEWPORT9 viewport = { raster->nOffsetX, raster->nOffsetY,
                   raster->width, raster->height, 0,  1 };

            _RwD3DDevice->SetViewport(&viewport);

            auto d3d9ClearFlags = 0u;

            if (rwCAMERACLEARIMAGE & clearFlags)
                d3d9ClearFlags |= D3DCLEAR_TARGET;
            if (rwCAMERACLEARZ & clearFlags)
            {
                d3d9ClearFlags |= D3DCLEAR_ZBUFFER;
                d3d9ClearFlags |= D3DCLEAR_STENCIL;
            }
            else if (rwCAMERACLEARSTENCIL & clearFlags)
                d3d9ClearFlags |= D3DCLEAR_STENCIL;

            _RwD3DDevice->Clear(0, NULL, d3d9ClearFlags, packedColor, 1, 0);
        }

        // bWindowIconic = bCurrIconic;

        return true;
    }

    RwBool RenderingEngine::CameraBeginUpdate(void* out, void* cameraIn, RwInt32 in)
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
        HRESULT             hr;
        RECT                rect;

        /* set the FPU to single precision */
        _rwProcessorForceSinglePrecision();

        camera = (RwCamera*)cameraIn;
        dgGGlobals.curCamera = camera;

        _RwD3D9D3D9ViewTransform = *(D3DMATRIX*)&XMMatrixInverse(nullptr, RwMatrixToXMMATRIX(RwFrameGetLTM(RwCameraGetFrame(camera))));
        _RwD3D9D3D9ViewTransform.m[0][0] = -_RwD3D9D3D9ViewTransform.m[0][0];
        _RwD3D9D3D9ViewTransform.m[1][0] = -_RwD3D9D3D9ViewTransform.m[1][0];
        _RwD3D9D3D9ViewTransform.m[2][0] = -_RwD3D9D3D9ViewTransform.m[2][0];
        _RwD3D9D3D9ViewTransform.m[3][0] = -_RwD3D9D3D9ViewTransform.m[3][0];
        RwD3D9SetTransform(D3DTS_VIEW, &_RwD3D9D3D9ViewTransform);

        _RwD3D9D3D9ProjTransform.m[0][0] = camera->recipViewWindow.x;
        _RwD3D9D3D9ProjTransform.m[1][1] = camera->recipViewWindow.y;
        _RwD3D9D3D9ProjTransform.m[2][0] = camera->recipViewWindow.x * camera->viewOffset.x;
        _RwD3D9D3D9ProjTransform.m[2][1] = camera->recipViewWindow.y * camera->viewOffset.y;
        _RwD3D9D3D9ProjTransform.m[3][0] = -camera->recipViewWindow.x * camera->viewOffset.x;
        _RwD3D9D3D9ProjTransform.m[3][1] = -camera->recipViewWindow.y * camera->viewOffset.y;

        if (camera->projectionType == rwPARALLEL)
        {
            _RwD3D9D3D9ProjTransform.m[2][2] = 1.0f / (camera->farPlane - camera->nearPlane);
            _RwD3D9D3D9ProjTransform.m[2][3] = 0.0f;
            _RwD3D9D3D9ProjTransform.m[3][3] = 1.0f;
        }
        else
        {
            _RwD3D9D3D9ProjTransform.m[2][2] = camera->farPlane / (camera->farPlane - camera->nearPlane);
            _RwD3D9D3D9ProjTransform.m[2][3] = 1.0f;
            _RwD3D9D3D9ProjTransform.m[3][3] = 0.0f;
        }

        _RwD3D9D3D9ProjTransform.m[3][2] = -_RwD3D9D3D9ProjTransform.m[2][2] * camera->nearPlane;
        RwD3D9SetTransform(D3DTS_PROJECTION, &_RwD3D9D3D9ProjTransform);

        raster = RwCameraGetRaster(camera);
        topRaster = (raster);

        /* Check if the main window has changed its size */
        GetClientRect(*WindowHandle, &rect);

        bCurrIconic = IsIconic(*WindowHandle);
        if (!bCurrIconic && bWindowIconic)
            bIconicRestore = TRUE;

        bWindowIconic = bCurrIconic;

        if ((RwUInt32)rect.right != _RwD3D9AdapterInformation.mode.Width
            || (RwUInt32)rect.bottom != _RwD3D9AdapterInformation.mode.Height
            || bIconicRestore)
        {

            ReleaseVideoMemory();

            hr = _RwD3DDevice->TestCooperativeLevel();

            if (Present.Windowed)
            {
                Present.BackBufferWidth = rect.right;
                Present.BackBufferHeight = rect.bottom;
            }

            hr = _RwD3DDevice->Reset(&Present);

            if (FAILED(hr))
                Log::Error("RenderingEngine::CameraBeginUpdate - failed to reset device");

            RestoreVideoMemory();

            _RwD3D9AdapterInformation.mode.Width = rect.right;
            _RwD3D9AdapterInformation.mode.Height = rect.bottom;
            bChangeVideo = FALSE;
        }

        if (rwRASTERTYPECAMERATEXTURE == topRaster->cType)
        {
            SetRenderTarget(0, topRaster);
            SetDepthStencilRaster(RwCameraGetZRaster((RwCamera*)camera));
        }
        else
        {
            _SetRenderTarget(0, RwD3D9RenderSurface);
            SetDepthStencilSurface(RwD3D9DepthStencilSurface);
        }

        D3DVIEWPORT9  viewport = { raster->nOffsetX, raster->nOffsetY,
                       raster->width, raster->height, 0,  1 };

        _RwD3DDevice->SetViewport(&viewport);

        /* Update fog range */
        RenderStates::SetRenderState(D3DRS_FOGSTART, FLOATASDWORD(camera->fogPlane));
        RenderStates::SetRenderState(D3DRS_FOGEND, FLOATASDWORD(camera->farPlane));

        return (BeginScene());
    }

    RwBool RenderingEngine::CameraEndUpdate(void* camera, void* color, RwInt32 clearFlags)
    {
        dgGGlobals.curCamera = NULL;
        return true;
    }

    RwBool RenderingEngine::RasterShowRaster(void* raster, void* inOut, RwInt32 flags)
    {
        RwBool              doReset = FALSE;
        RwRaster* topRaster;
        _rwD3D9RasterExt* topRasterExt;
        HRESULT             hr;

        if (InsideScene)
        {
            _RwD3DDevice->EndScene();
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

            hr = _RwD3DDevice->Reset(&Present);

            if (SUCCEEDED(hr))
            {
                RestoreVideoMemory();
            }
        }

        hr = _RwD3DDevice->Present(NULL, NULL, NULL, NULL);

        if (hr == D3DERR_DEVICELOST)
        {
            hr = _RwD3DDevice->TestCooperativeLevel();

            if (hr == D3DERR_DEVICENOTRESET)
            {
                D3D9DeviceReleaseVideoMemory();

                hr = _RwD3DDevice->Reset(&Present);
                if (SUCCEEDED(hr))
                {
                    RestoreVideoMemory();
                }
            }
        }
        return SUCCEEDED(hr);
    }

    bool RenderingEngine::BeginScene()
    {
        if (!TestState())
            InsideScene = false;

        if (!InsideScene)
        {
            if (SUCCEEDED(_RwD3DDevice->BeginScene()))
                InsideScene = true;
        }

        return InsideScene;
    }

    bool RenderingEngine::TestState()
    {
        auto hr = _RwD3DDevice->TestCooperativeLevel();

        if (hr == D3DERR_DEVICENOTRESET)
        {
            ReleaseVideoMemory();

            hr = _RwD3DDevice->Reset(&Present);
            if (SUCCEEDED(hr))
                RestoreVideoMemory();
        }

        return SUCCEEDED(hr);
    }

    typedef void (*rwD3D9DeviceRestoreCallBack)(void);
    rwD3D9DeviceRestoreCallBack D3D9RestoreDeviceCallback = *(rwD3D9DeviceRestoreCallBack*)0xC980B0;

    typedef void (*rwD3D9DeviceReleaseCallBack)(void);
    rwD3D9DeviceReleaseCallBack D3D9ReleaseDeviceCallback = NULL;

    bool RenderingEngine::RestoreVideoMemory()
    {
        _RwD3DDevice->GetRenderTarget(0, &RwD3D9RenderSurface);
        IDirect3DSurface9_Release(RwD3D9RenderSurface);

        _RwD3DDevice->GetDepthStencilSurface(&RwD3D9DepthStencilSurface);
        IDirect3DSurface9_Release(RwD3D9DepthStencilSurface);

        bool rv = Raster::_rxD3D9VideoMemoryRasterListRestore();

        rwD3D9DynamicVertexBufferRestore();

        if (rv)
            RenderStates::_rwD3D9RenderStateReset();

        rv = rv && rwD3D9Im2DRenderOpen();
        rv = rv && rwD3D9Im3DRenderOpen();

        if (D3D9RestoreDeviceCallback)
            D3D9RestoreDeviceCallback();

        return rv;
    }

    void RenderingEngine::ReleaseVideoMemory()
    {
        RwUInt32            i;

        D3D9ClearCacheShaders();
        D3D9ClearCacheMatrix();

        for (i = 0; i < RWD3D9_MAX_TEXTURE_STAGES; i++)
            _RwD3DDevice->SetTexture(i, NULL);

        _RwD3DDevice->SetIndices(NULL);

        for (i = 0; i < MAX_STREAMS; i++)
            _RwD3DDevice->SetStreamSource(i, NULL, 0, 0);

        _RwD3DDevice->SetPixelShader(NULL);
        _RwD3DDevice->SetVertexDeclaration(NULL);
        _RwD3DDevice->SetVertexShader(NULL);

        _SetRenderTarget(0, RwD3D9RenderSurface);
        for (i = 1; i < 4; i++)
            _SetRenderTarget(i, NULL);

        SetDepthStencilSurface(RwD3D9DepthStencilSurface);

        _rwD3D9Im2DRenderClose();
        _rwD3D9Im3DRenderClose();
        _rwD3D9DynamicVertexBufferRelease();
        Raster::_rxD3D9VideoMemoryRasterListRelease();
        RwFreeListPurgeAllFreeLists();

        if (D3D9ReleaseDeviceCallback != NULL)
            D3D9ReleaseDeviceCallback();
    }

    const void* RenderingEngine::D3D9GetCaps()
    {
        return &_RwD3D9DeviceCaps;
    }

    void RenderingEngine::D3D9SetPresentParameters(const D3DDISPLAYMODE* mode, RwUInt32 flags, D3DFORMAT adapterFormat)
    {
        Present.BackBufferCount = 1;
        Present.BackBufferWidth = mode->Width;
        Present.BackBufferHeight = mode->Height;
        Present.AutoDepthStencilFormat = D3DFMT_D24S8;
        Present.MultiSampleType = D3DMULTISAMPLE_NONE;
        Present.MultiSampleQuality = 0;
        Present.Windowed = TRUE;
        Present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
        Present.FullScreen_RefreshRateInHz = 0;
        Present.EnableAutoDepthStencil = TRUE;
        Present.BackBufferFormat = D3DFMT_A8R8G8B8;
        Present.SwapEffect = D3DSWAPEFFECT_DISCARD;

        _RwD3D9ZBufferDepth = 32;
    }

    RwBool RenderingEngine::RwD3D9ChangeVideoMode(RwInt32 modeIndex)
    {
        bChangeVideo = TRUE;

        if (SystemStarted && (modeIndex >= 0) &&
            (modeIndex < mDisplayModes.size()))
        {
            if (_RwD3D9CurrentModeIndex != modeIndex)
            {
                ReleaseVideoMemory();

                _RwD3DDevice->TestCooperativeLevel();

                D3D9SetPresentParameters(&mDisplayModes[modeIndex].mode, mDisplayModes[modeIndex].flags, DesktopDisplayMode.Format);

                auto hr = _RwD3DDevice->Reset(&Present);
                if (FAILED(hr))
                {
                    Log::Error("RenderingEngine::RwD3D9ChangeVideoMode - failed to reset device");
                    return false;
                }

                RestoreVideoMemory();

                _RwD3D9CurrentModeIndex = modeIndex;
                _RwD3D9AdapterInformation.mode = mDisplayModes[modeIndex].mode;
                _RwD3D9AdapterInformation.flags = mDisplayModes[modeIndex].flags;
                _RwD3D9AdapterInformation.displayDepth = 32;

                SetWindowLong(*WindowHandle, GWL_STYLE, WS_VISIBLE);
                SetWindowPos(*WindowHandle, 0, 0, 0, Present.BackBufferWidth, Present.BackBufferHeight, SWP_NOMOVE | SWP_NOZORDER);
            }
        }

        return true;
    }

    void RenderingEngine::D3D9CreateDisplayModesList(void)
    {
        Log::Debug("RenderingEngine::D3D9CreateDisplayModesList");

        RwInt32 i, format, n, maxmodesformat, numDisplaymodes;

        _RwDirect3DObject->GetAdapterDisplayMode(_RwD3DAdapterIndex, &mDisplayModes[0].mode);

        mDisplayModes[0].flags = 0;
        numDisplaymodes = 1;

        for (format = 0; format < MAX_BACKBUFFER_FORMATS; format++)
        {
            maxmodesformat = _RwDirect3DObject->GetAdapterModeCount(_RwD3DAdapterIndex, BackBufferFormats[format]);

            for (i = 0; i < maxmodesformat; i++)
            {
                _RwDirect3DObject->EnumAdapterModes(_RwD3DAdapterIndex, BackBufferFormats[format], i, &mDisplayModes[numDisplaymodes].mode);

                /* Find a previous similar mode */
                for (n = 1; n < numDisplaymodes; n++)
                {
                    if (mDisplayModes[numDisplaymodes].mode.Width == mDisplayModes[n].mode.Width
                        && mDisplayModes[numDisplaymodes].mode.Height == mDisplayModes[n].mode.Height
                        && mDisplayModes[numDisplaymodes].mode.Format == mDisplayModes[n].mode.Format)
                        break;
                }

                if (n >= numDisplaymodes)
                {
                    mDisplayModes[numDisplaymodes].flags = rwVIDEOMODEEXCLUSIVE;
                    numDisplaymodes++;
                }
                else
                {
                    if (mDisplayModes[n].mode.RefreshRate < mDisplayModes[numDisplaymodes].mode.RefreshRate)
                        mDisplayModes[n].mode.RefreshRate = mDisplayModes[numDisplaymodes].mode.RefreshRate;
                }
            }
        }
    }

    RwBool RenderingEngine::D3D9System(RwInt32 request, void* out, void* inOut, RwInt32 in)
    {
        switch (request)
        {
        case rwDEVICESYSTEMUSEMODE:
            if (!SystemStarted && (in >= 0) && (in < mDisplayModes.size()))
            {
                _RwD3D9CurrentModeIndex = in;
                _RwD3D9AdapterInformation.mode = mDisplayModes[in].mode;
                _RwD3D9AdapterInformation.flags = mDisplayModes[in].flags;
                _RwD3D9AdapterInformation.displayDepth = 32;

                /* Calculate max multisampling levels */
                D3D9CalculateMaxMultisamplingLevels();

                return true;
            }
            else
                return false;

        case rwDEVICESYSTEMGETNUMMODES:
            if (!mDisplayModes.size())
                D3D9CreateDisplayModesList();

            *((RwInt32*)out) = mDisplayModes.size();
            return true;

        case rwDEVICESYSTEMGETMODEINFO:
            if (!mDisplayModes.size())
                D3D9CreateDisplayModesList();

            if ((in >= 0) && (in < mDisplayModes.size()))
            {
                RwVideoMode* videoMode = (RwVideoMode*)out;

                videoMode->width = mDisplayModes[in].mode.Width;
                videoMode->height = mDisplayModes[in].mode.Height;
                videoMode->depth = 32;
                videoMode->flags = ((RwVideoModeFlag)mDisplayModes[in].flags);
                videoMode->refRate = mDisplayModes[in].mode.RefreshRate;
                videoMode->format = rwD3D9FindRWFormat(mDisplayModes[in].mode.Format);

                return true;
            }
            else
                return false;

        case rwDEVICESYSTEMGETMODE:
            *((RwInt32*)out) = _RwD3D9CurrentModeIndex;
            return true;

        case rwDEVICESYSTEMFOCUS:
            if (_RwD3DDevice != NULL)
            {
                if (!Present.Windowed)
                {
                    if (in)
                        ShowWindow(*WindowHandle, SW_RESTORE);
                    else
                        ShowWindow(*WindowHandle, SW_HIDE);
                }
                return true;
            }
            return false;

            /*** Register in the library block ***/

        case rwDEVICESYSTEMREGISTER:
        {
            RwDevice* D3D9DriverDevice = _rwDeviceGetHandle();
            RwDevice* deviceOut = (RwDevice*)out;
            RwMemoryFunctions* memoryFunctions = (RwMemoryFunctions*)inOut;

            *deviceOut = *D3D9DriverDevice;
            dgGGlobals.memFuncs = memoryFunctions;

            return true;
        }

        /******************* Opening/Closing **********************/

        case rwDEVICESYSTEMOPEN:
            RWRETURN(D3D9DeviceSystemOpen(out, inOut, in));

        case rwDEVICESYSTEMCLOSE:
            RWRETURN(D3D9DeviceSystemClose(out, inOut, in));

            /********* Selecting a rendering sub system ********/

        case rwDEVICESYSTEMGETNUMSUBSYSTEMS:
        {
            *(RwInt32*)out = _RwDirect3DObject->GetAdapterCount();

            return true;
        }

        case rwDEVICESYSTEMGETSUBSYSTEMINFO:
        {
            D3DADAPTER_IDENTIFIER9 adapterInfo;
            _RwDirect3DObject->GetAdapterIdentifier((RwInt32)in, 0, &adapterInfo);

            strcpy(((RwSubSystemInfo*)out)->name, adapterInfo.Description);
            return true;
        }

        case rwDEVICESYSTEMGETCURRENTSUBSYSTEM:
        {
            *(RwInt32*)out = _RwD3DAdapterIndex;

            return true;
        }

        case rwDEVICESYSTEMSETSUBSYSTEM:
        {
            RwInt32 i;

            _RwD3DAdapterIndex = in;

            /* Mode count */
            _RwD3D9AdapterInformation.modeCount = 0;
            for (i = 0; i < MAX_BACKBUFFER_FORMATS; i++)
                _RwD3D9AdapterInformation.modeCount += _RwDirect3DObject->GetAdapterModeCount(_RwD3DAdapterIndex, BackBufferFormats[i]);

            /* Get the current mode as the default */
            _RwDirect3DObject->GetAdapterDisplayMode(_RwD3DAdapterIndex, &_RwD3D9AdapterInformation.mode);

            /* Recreate list of modes */
            D3D9CreateDisplayModesList();

            /* Calculate max multisampling levels */
            D3D9CalculateMaxMultisamplingLevels();

            return true;
        }

        /************** Starting stopping ******************/

        /* Start means that this bit of hardware has been selected for
         * rendering - so get it into a state ready for rendering */

        case rwDEVICESYSTEMSTART:
        {
            SystemStarted = D3D9DeviceSystemStart(out, inOut, in);

            return SystemStarted;
        }

        case rwDEVICESYSTEMSTOP:
            return D3D9DeviceSystemStop(out, inOut, in);

            /************* standard device functions ************************/

        case rwDEVICESYSTEMSTANDARDS:
            return D3D9DeviceSystemStandards(out, inOut, in);

        case rwDEVICESYSTEMINITPIPELINE:
            break;

        case rwDEVICESYSTEMGETTEXMEMSIZE:
        {
            *((RwUInt32*)out) = _RwD3DDevice->GetAvailableTextureMem();

            return true;
        }
        break;

        case rwDEVICESYSTEMFINALIZESTART:
        {
            return D3D9DeviceSystemFinalizeStart(out, inOut, in);
        }
        break;

        case rwDEVICESYSTEMINITIATESTOP:
            break;

        case rwDEVICESYSTEMGETMAXTEXTURESIZE:
        {
            *(RwInt32*)out = max(_RwD3D9DeviceCaps.MaxTextureWidth, _RwD3D9DeviceCaps.MaxTextureHeight);
            return true;
        }
        break;

        case rwDEVICESYSTEMRXPIPELINEREQUESTPIPE:
            break;

        case rwDEVICESYSTEMGETID:
        {
            *(RwUInt16*)out = rwDEVICE_D3D9;
            return true;
        }
        break;
        }

        return false;
    }

    RwBool RenderingEngine::D3D9DeviceSystemOpen(void* out, void* inOut, RwInt32 in)
    {
        Log::Debug("RenderingEngine::D3D9DeviceSystemOpen");
        RwUInt32 numAdapters;
        RwInt32  i;

        /* We're interested in the window handle here, so store it away */
        *WindowHandle = (HWND)(((RwEngineOpenParams*)inOut)->displayID);

        /* Create a device */
        _RwDirect3DObject = Direct3DCreate9(D3D_SDK_VERSION);
        if (!_RwDirect3DObject)
            return FALSE;

        /* Name */
        strcpy(_RwD3D9AdapterInformation.name, RWSTRING("D3D9"));

        /* Get video card capabilities */
        numAdapters = _RwDirect3DObject->GetAdapterCount();

        _RwD3DAdapterIndex = 0;
        _RwD3DAdapterType = D3DDEVTYPE_HAL;
        _RwDirect3DObject->GetDeviceCaps(_RwD3DAdapterIndex, _RwD3DAdapterType, &_RwD3D9DeviceCaps);

         while (_RwD3DAdapterIndex < numAdapters)
         {
             if (SUCCEEDED(_RwDirect3DObject->GetDeviceCaps(_RwD3DAdapterIndex, _RwD3DAdapterType, &_RwD3D9DeviceCaps)))
                 break;

             _RwD3DAdapterIndex++;
         }

         if (_RwD3DAdapterIndex >= numAdapters)
         {
             SAFE_RELEASE(_RwDirect3DObject);
             return FALSE;
         }

         /* Mode count */
        _RwD3D9AdapterInformation.modeCount = 0;
        for (i = 0; i < MAX_BACKBUFFER_FORMATS; i++)
        {
            _RwD3D9AdapterInformation.modeCount += _RwDirect3DObject->GetAdapterModeCount(_RwD3DAdapterIndex, BackBufferFormats[i]);
        }

        /* Get the first mode as the default */
        _RwDirect3DObject->GetAdapterDisplayMode(_RwD3DAdapterIndex, &_RwD3D9AdapterInformation.mode);

        _RwD3D9AdapterInformation.displayDepth = 32;
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

        return true;
    }

    RwBool RenderingEngine::D3D9DeviceSystemStart(void* out __RWUNUSED__, void* inOut __RWUNUSED__, RwInt32 in __RWUNUSED__)
    {
        Log::Debug("RenderingEngine::D3D9DeviceSystemStart");

        D3DADAPTER_IDENTIFIER9	adapterInfo;
        D3DDISPLAYMODE			adapterDisplayMode;
        RwUInt32				behaviorFlags;
        UINT					Adapter;

        _RwDirect3DObject->GetAdapterDisplayMode(_RwD3DAdapterIndex, &adapterDisplayMode);

        memset(&Present, 0, sizeof(D3DPRESENT_PARAMETERS));
        D3D9SetPresentParameters(&_RwD3D9AdapterInformation.mode, _RwD3D9AdapterInformation.flags, adapterDisplayMode.Format);

        Present.hDeviceWindow = *WindowHandle;
        Present.Flags = 0;

        SetWindowLong(Present.hDeviceWindow, GWL_STYLE, NULL);
        SetWindowPos(Present.hDeviceWindow, HWND_NOTOPMOST, 0, 0, Present.BackBufferWidth, Present.BackBufferHeight, SWP_NOACTIVATE | SWP_NOZORDER);



        //_RwDirect3DObject->GetDeviceCaps(_RwD3DAdapterIndex, _RwD3DAdapterType, &_RwD3D9DeviceCaps);
        //_RwDirect3DObject->GetAdapterIdentifier(_RwD3DAdapterIndex, 0, &adapterInfo);

        //behaviorFlags = 0;
        //if (_RwD3D9DeviceCaps.DevCaps & D3DDEVCAPS_PUREDEVICE)
        //    behaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING /*| D3DCREATE_PUREDEVICE*/;
        //else
        //    behaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;


        auto hr = _RwDirect3DObject->CreateDevice(_RwD3DAdapterIndex, _RwD3DAdapterType, *WindowHandle, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Present, &_RwD3DDevice);
        if (FAILED(hr))

        {
            Log::Error("RenderingEngine::D3D9DeviceSystemStart - failed to create d3d9 device");
            return FALSE;
        }

        if (*WindowHandle)
        {
            RECT                rect;

            GetClientRect(*WindowHandle, &rect);

            _RwD3D9AdapterInformation.mode.Width = rect.right;
            _RwD3D9AdapterInformation.mode.Height = rect.bottom;
        }

        _RwD3DDevice->GetRenderTarget(0, &RwD3D9RenderSurface);
        RwD3D9RenderSurface->Release();

        _RwD3DDevice->GetDepthStencilSurface(&RwD3D9DepthStencilSurface);
        RwD3D9DepthStencilSurface->Release();

        D3D9ClearCache();

        Raster::_rwD3D9RasterOpen();
        rwD3D9Im2DRenderOpen();
        RenderStates::_rwD3D9RenderStateOpen();
        _rwD3D9VertexBufferManagerOpen();

        return TRUE;
    }

    RwBool RenderingEngine::D3D9DeviceSystemFinalizeStart(void* out __RWUNUSED__, void* inOut __RWUNUSED__, RwInt32 in __RWUNUSED__)
    {
        Log::Debug("RenderingEngine::D3D9DeviceSystemFinalizeStart");

        _rwProcessorInitialize();

        if (_rwD3D9CPUSupportsSSE)
        {
            RwUInt32 temp;

            _asm
            {
                stmxcsr temp
                mov     eax, temp
                or eax, 0x8000
                mov     temp, eax
                ldmxcsr temp
            }
        }

        if (_rwD3D9CPUSupportsSSE)
        {
            /* set the matrix-vector products to the new SSE code */
            if (_rwD3D9CPUSupportsSSE2 && _rwD3D9CPUSupports3DNow == FALSE)
            {
                _rwVectorSetMultFn(_rwIntelSSEP4V3dTransformPoint, _rwIntelSSEP4V3dTransformPoints,
                    _rwIntelSSEP4V3dTransformVector, _rwIntelSSEP4V3dTransformVectors);
            }
            else
            {
                _rwVectorSetMultFn(_rwIntelSSEV3dTransformPoint, _rwIntelSSEV3dTransformPoints,
                    _rwIntelSSEV3dTransformVector, _rwIntelSSEV3dTransformVectors);
            }
        }

        return true;
    }

    RwBool RenderingEngine::D3D9DeviceSystemClose(void* pOut __RWUNUSED__, void* pInOut __RWUNUSED__, RwInt32 nIn __RWUNUSED__)
    {
        mDisplayModes.clear();
        SAFE_RELEASE(_RwDirect3DObject);

        Log::Debug("RenderingEngine::D3D9DeviceSystemClose");

        return true;
    }

    RwBool RenderingEngine::D3D9DeviceSystemStop(void* out __RWUNUSED__, void* inOut __RWUNUSED__, RwInt32 in __RWUNUSED__)
    {
        RwUInt32 n;

        SystemStarted = FALSE;

        mDisplayModes.clear();

        /* Restore the saved floating point control register */
        _rwProcessorRelease();

        /* Disable SSE flush-to-zero */
        if (_rwD3D9CPUSupportsSSE)
        {
            RwUInt32 temp;

            _asm
            {
                stmxcsr  temp
                mov      eax, temp
                and eax, not 0x8000
                mov      temp, eax
                ldmxcsr  temp
            }
        }

        for (auto matrix : LastMatrixUsed)
            SAFE_DELETE(matrix.second);
        LastMatrixUsed.clear();

        MaxNumLights = 0;
        if (LightsCache != NULL)
        {
            RwFree(LightsCache);
            LightsCache = NULL;
        }

        for (n = 0; n < RWD3D9_MAX_TEXTURE_STAGES; n++)
            _RwD3DDevice->SetTexture(n, NULL);

        _RwD3DDevice->SetIndices(NULL);

        for (n = 0; n < MAX_STREAMS; n++)
            _RwD3DDevice->SetStreamSource(n, NULL, 0, 0);

        _RwD3DDevice->SetPixelShader(NULL);
        _RwD3DDevice->SetVertexDeclaration(NULL);
        _RwD3DDevice->SetVertexShader(NULL);

        for (uint32_t n = 0; n < m_vertexDeclarations.size(); n++)
        {
            SAFE_RELEASE(m_vertexDeclarations[n].vertexdeclaration);
            SAFE_DELETE(m_vertexDeclarations[n].elements);
        }

        m_vertexDeclarations.clear();

        Raster::_rwD3D9RasterClose();
        _rwD3D9VertexBufferManagerClose();
        _rwD3D9Im2DRenderClose();

        SAFE_RELEASE(_RwD3DDevice);

        Log::Debug("RenderingEngine::D3D9DeviceSystemStop");

        return true;
    }

    RwBool  D3D9NullStandard(void* pOut, void* pInOut, RwInt32 nIn)
    {
        return FALSE;
    }

    RwBool RenderingEngine::D3D9DeviceSystemStandards(void* out, void* inOut, RwInt32 numStandardsFunctions)
    {
        struct RwStandard
        {
            RwInt32             nStandard;
            RwStandardFunc      fpStandard;
        };

        RwStandard rwD3D9Standards[] = {
            {rwSTANDARDCAMERABEGINUPDATE, CameraBeginUpdate},
            {rwSTANDARDCAMERAENDUPDATE, CameraEndUpdate},
            {rwSTANDARDCAMERACLEAR, CameraClear},
            {rwSTANDARDRASTERSHOWRASTER, RasterShowRaster},
            {rwSTANDARDRGBTOPIXEL,  D3D9NullStandard},
            {rwSTANDARDPIXELTORGB,  D3D9NullStandard},
            {rwSTANDARDRASTERSETIMAGE, (RwStandardFunc)0x8001E0},
            {rwSTANDARDIMAGEGETRASTER,  (RwStandardFunc)0x7FF270},
            {rwSTANDARDRASTERDESTROY,  Raster::_rwD3D9RasterDestroy},
            {rwSTANDARDRASTERCREATE,  Raster::_rwD3D9RasterCreate},
            {rwSTANDARDIMAGEFINDRASTERFORMAT,  (RwStandardFunc)0x7FFF00},
            {rwSTANDARDTEXTURESETRASTER,  (RwStandardFunc)0x4CBD40},
            {rwSTANDARDRASTERLOCK, Raster::D3D9RasterLock},
            {rwSTANDARDRASTERUNLOCK, Raster::D3D9RasterUnlock},
            {rwSTANDARDRASTERLOCKPALETTE,D3D9NullStandard},
            {rwSTANDARDRASTERUNLOCKPALETTE, D3D9NullStandard},
            {rwSTANDARDRASTERCLEAR, D3D9NullStandard},
            {rwSTANDARDRASTERCLEARRECT, D3D9NullStandard},
            {rwSTANDARDRASTERRENDER, D3D9NullStandard},
            {rwSTANDARDRASTERRENDERSCALED, D3D9NullStandard},
            {rwSTANDARDRASTERRENDERFAST, D3D9NullStandard},
            {rwSTANDARDSETRASTERCONTEXT, (RwStandardFunc)0x4CB520},
            {rwSTANDARDRASTERSUBRASTER,  (RwStandardFunc)0x4CB524},
            {rwSTANDARDNATIVETEXTUREGETSIZE, D3D9NullStandard},
            {rwSTANDARDNATIVETEXTUREWRITE, D3D9NullStandard},
            {rwSTANDARDNATIVETEXTUREREAD, Raster::_rwD3D9NativeTextureRead},
            {rwSTANDARDRASTERGETMIPLEVELS,D3D9NullStandard}
        };

        auto standardFunctions = (RwStandardFunc*)out;
        auto numDriverFunctions = sizeof(rwD3D9Standards) / sizeof(RwStandard);

        /* Clear out all of the standards initially */
        for (auto i = 0; i < numStandardsFunctions; i++)
            standardFunctions[i] = D3D9NullStandard;

        /* Fill in all of the standards */
        while (numDriverFunctions--)
        {
            if ((rwD3D9Standards->nStandard < numStandardsFunctions) &&
                (rwD3D9Standards->nStandard >= 0))
                standardFunctions[rwD3D9Standards[numDriverFunctions].nStandard] = rwD3D9Standards[numDriverFunctions].fpStandard;
        }

        return true;
    }

    RwDevice* RenderingEngine::_rwDeviceGetHandle(void)
    {
        static RwDevice     rwD3D9DriverDevice = {
        1.0f,                   /* default gamma correction */
        D3D9System,
        0.0f, 1.0f, /* near and far Z buffer limits */

        RenderStates::_rwD3D9RWSetRenderState, /* RwRenderStateSetFunction */
        RenderStates::_rwD3D9RWGetRenderState, /* RwRenderStateGetFunction */

        _rwD3D9Im2DRenderLine,  /* RwIm2DRenderLineFunction */
        _rwD3D9Im2DRenderTriangle, /* RwIm2DRenderTriangleFunction */
        _rwD3D9Im2DRenderPrimitive, /* RwIm2DRenderPrimitiveFunction */
        _rwD3D9Im2DRenderIndexedPrimitive, /* RwIm2DRenderIndexedPrimitiveFunction */

        NULL, NULL, NULL, NULL  /* These get set up when the 3D Immediate mode module is setup */
        };

        return &rwD3D9DriverDevice;
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
    }

    void RenderingEngine::D3D9ClearCacheMatrix()
    {
        LastWorldMatrixUsedIdentity = FALSE;

        for (auto matrix : LastMatrixUsed)
            SAFE_DELETE(matrix.second);
        LastMatrixUsed.clear();

        LastMatrixUsed[D3DTS_WORLD] = new D3DMATRIX;
        memcpy(LastMatrixUsed[D3DTS_WORLD], &IdentityMatrix, sizeof(D3DMATRIX));
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

        CurrentDepthStencilSurface = NULL;
        for (n = 0; n < 4; n++)
            CurrentRenderSurface[n] = NULL;

        D3D9ClearCacheShaders();
        D3D9ClearCacheMatrix();
        D3D9ClearCacheLights();
    }
}