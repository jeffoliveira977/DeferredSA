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

#define HAS_ALPHA           (1<<0)
#define IS_CUBE             (1<<1)
#define USE_AUTOMIPMAPGEN   (1<<2)
#define IS_COMPRESSED       (1<<3)

#define D3D9DeviceSystemStandards(out, inOut, numStandardsFunctions) ((signed int (__cdecl*) (void*, void*,RwInt32))0x007F72E0)(out,inOut,numStandardsFunctions)

#define _rwCPUHaveMMX() ((signed int (__cdecl*) ())0x80DBE0)()
#define _rwCPUHaveSSE() ((signed int (__cdecl*) ())0x80DC40)()
#define _rwCPUHaveSSE2() ((signed int (__cdecl*) ())0x80DCA0)()
#define _rwCPUHave3DNow() ((signed int (__cdecl*) ())0x80DD00)()
#define _rwProcessorInitialize() ((void (__cdecl*)())0x00857404)()

#define D3D9CalculateMaxMultisamplingLevels() ((void (__cdecl*)())0x007F6BF0)()

//#define _rwD3D9RasterClose() ((void (__cdecl*)())0x004CC170)()
#define _rwD3D9VertexBufferManagerClose() ((void (__cdecl*)())0x007F5D20)()
#define _rwD3D9Im2DRenderClose() ((void (__cdecl*)())0x007FB5F0)()
#define _rwD3D9Im3DRenderClose() ((void (__cdecl*)())0x0080DFB0 )()
#define _rwD3D9DynamicVertexBufferRelease() ((void (__cdecl*)())0x007F5840 )()
//#define _rxD3D9VideoMemoryRasterListRelease() ((void (__cdecl*)())0x004CB640 )()
#define RwFreeListPurgeAllFreeLists() ((void (__cdecl*)())0x00801F90 )()

//#define _rwD3D9RasterOpen() ((void (__cdecl*)())0x004CC150)()
//#define _rwD3D9RenderStateOpen() ((void (__cdecl*)())0x007FCAC0)()
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

struct _rwD3D9NativeTexture
{
    RwInt32 id; /* RwPlatformID,(rwID_D3D9) defined in batype.h */
    RwInt32 filterAndAddress; /* Same as in babintex.c */
    RwChar  name[rwTEXTUREBASENAMELENGTH]; /* Texture name */
    RwChar  mask[rwTEXTUREBASENAMELENGTH]; /* Texture mask name */
};

struct _rwD3D9NativeRaster
{
    RwUInt32    format;         /* Raster format flags */
    D3DFORMAT   d3dFormat;      /* D3D pixel format */
    RwUInt16    width;          /* Raster width */
    RwUInt16    height;         /* Raster height */
    RwUInt8     depth;          /* Raster depth */
    RwUInt8     numMipLevels;   /* The number of mip levels to load */
    RwUInt8     type;           /* The raster type */
    RwUInt8     flags;          /* This raster has an alpha component, automipmapgen, etc */
};

struct _rwD3D9RasterConvData
{
    D3DFORMAT   format;
    RwUInt8     depth;
    RwUInt8     alpha;
};

struct _rwD3D9FormatInfo
{
    RwUInt8 alpha;
    RwUInt8 depth;
    RwUInt16 rwFormat;
};

#define NUMPIXELFORMATS     11

#define MAX_PIXEL_FORMATS   128

#define SETFORMATINFO(_format, _alpha, _depth, _rwformat)   \
    _rwD3D9PixelFormatInfo[_format].alpha = _alpha; \
    _rwD3D9PixelFormatInfo[_format].depth = _depth; \
    _rwD3D9PixelFormatInfo[_format].rwFormat = _rwformat


#define IS_D3DFORMAT_ZBUFFER(_d3dformat)    ((_d3dformat) >= D3DFMT_D16_LOCKABLE && (_d3dformat) <= D3DFMT_D16)

#define FMT2TBL(_p) (((_p) & rwRASTERFORMATPIXELFORMATMASK) >> 8)

#define NUMSHADEMODES   3
static const D3DSHADEMODE _RwD3D9ShadeModeConvTable[NUMSHADEMODES] =
{
    (D3DSHADEMODE)0,
    D3DSHADE_FLAT,
    D3DSHADE_GOURAUD
};

#define NUMFOGMODES     4
static const D3DFOGMODE _RwD3D9FogConvTable[NUMFOGMODES] =
{
    D3DFOG_NONE,
    D3DFOG_LINEAR,
    D3DFOG_EXP,
    D3DFOG_EXP2
};

#define NUMBLENDMODES   12
static const D3DBLEND _RwD3D9BlendConvTable[NUMBLENDMODES] =
{
    (D3DBLEND)0,
    D3DBLEND_ZERO,
    D3DBLEND_ONE,
    D3DBLEND_SRCCOLOR,
    D3DBLEND_INVSRCCOLOR,
    D3DBLEND_SRCALPHA,
    D3DBLEND_INVSRCALPHA,
    D3DBLEND_DESTALPHA,
    D3DBLEND_INVDESTALPHA,
    D3DBLEND_DESTCOLOR,
    D3DBLEND_INVDESTCOLOR,
    D3DBLEND_SRCALPHASAT
};

#define NUMADDRESSMODES 5
static const D3DTEXTUREADDRESS _RwD3D9AddressConvTable[NUMADDRESSMODES] =
{
    (D3DTEXTUREADDRESS)0,
    D3DTADDRESS_WRAP,
    D3DTADDRESS_MIRROR,
    D3DTADDRESS_CLAMP,
    D3DTADDRESS_BORDER
};
 _rwD3D9RasterConvData _rwRasterConvTable[NUMPIXELFORMATS] =
{
    {D3DFMT_UNKNOWN,       0, 0},   /* rwRASTERFORMATDEFAULT */
    {D3DFMT_A1R5G5B5,     16, 1},   /* rwRASTERFORMAT1555    */
    {D3DFMT_R5G6B5,       16, 0},   /* rwRASTERFORMAT565     */
    {D3DFMT_A4R4G4B4,     16, 1},   /* rwRASTERFORMAT4444    */
    {D3DFMT_L8,            8, 0},   /* rwRASTERFORMATLUM8    */
    {D3DFMT_A8R8G8B8,     32, 1},   /* rwRASTERFORMAT8888    */
    {D3DFMT_X8R8G8B8,     32, 0},   /* rwRASTERFORMAT888     */
    {D3DFMT_D16,          16, 0},   /* rwRASTERFORMAT16      */
    {D3DFMT_D24X8,        32, 0},   /* rwRASTERFORMAT24      */
    {D3DFMT_D32,          32, 0},   /* rwRASTERFORMAT32      */
    {D3DFMT_X1R5G5B5,     16, 0}    /* rwRASTERFORMAT555     */
};

 /* D3D9 pixel format table */
 _rwD3D9FormatInfo _rwD3D9PixelFormatInfo[MAX_PIXEL_FORMATS];

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

typedef struct _rxD3D9VideoMemoryRaster RxD3D9VideoMemoryRaster;
struct _rxD3D9VideoMemoryRaster
{
    RwRaster* raster;
    RxD3D9VideoMemoryRaster* next;
};

typedef struct
{
    uint32_t stage;
    D3DTEXTURESTAGESTATETYPE type;
} RxTextureStageChangeType;

typedef struct _rwD3D9StageStateCache _rwD3D9StageStateCache;
struct _rwD3D9StageStateCache
{
    RwRaster* curTexRaster;
    RwTextureAddressMode    texAddressU;
    RwTextureAddressMode    texAddressV;
    RwTextureFilterMode     filterMode;
    RwUInt32                borderColor;
    RwInt32                 maxAnisotropy;
};

typedef struct _rwD3D9StateCache _rwD3D9StateCache;
struct _rwD3D9StateCache
{
    /* Z Buffer */
    RwBool                  zWriteEnable;
    RwBool                  zTestEnable;

    /* Stencil Buffer */
    RwBool                  stencilEnable;
    RwStencilOperation      stencilFailOp;
    RwStencilOperation      stencilZFailOp;
    RwStencilOperation      stencilPassOp;
    RwStencilFunction       stencilFunction;
    RwInt32                 stencilRef;
    RwInt32                 stencilMask;
    RwInt32                 stencilWriteMask;

    /* Alpha blending */
    RwBool                  vertexAlphaBlendEnable;
    RwBool                  textureAlphaBlendEnable;

    /* Culling mode */
    RwCullMode              cullMode;

    /* Fog */
    RwBool                  fogEnable;
    RwFogType               fogType;
    RwReal                  fogDensity;
    RwUInt32                fogColor;

    /* Shading mode */
    RwShadeMode             shadeMode;

    /* Texturing stuff */
    _rwD3D9StageStateCache  stage[RWD3D9_MAX_TEXTURE_STAGES];

    /* SRC, DST blending modes */
    RwBlendFunction         srcBlend;
    RwBlendFunction         dstBlend;

    /* Alpha Test */
    RwAlphaTestFunction     alphaTestFunction;
    RwBool                  alphaTestEnable;
};

#define FORCE_SAMPLERSTATE(stage, type, newvalue)                           \
{                                                                           \
    SamplerStageStateD3D[stage][type] = newvalue;                           \
    IDirect3DDevice9_SetSamplerState(_RwD3DDevice, stage, type, newvalue);  \
}

_rwD3D9StateCache& _RwD3D9StateCache = *(_rwD3D9StateCache*)0xC9A4C0;

RwInt32& _RwD3D9RasterExtOffset = *(RwInt32*)0xB4E9E0;    /* Raster extension offset */

/* empty index for a new pallete */
 RwUInt32& NumPaletteIndexFree = *(RwUInt32*)0xB4E9E8;
 RwUInt32& MaxPaletteIndexFree = *(RwUInt32*)0xB4E9EC;
 RwUInt32& MaxPaletteIndex = *(RwUInt32*)0xB4E9F0;
 RwUInt16*& PaletteIndexFree = *(RwUInt16**)0xB4E9F4;

 RwFreeList*& PaletteFreeList = *(RwFreeList**)0xB4E9F8;

 struct _rwFilterParams
 {
     D3DTEXTUREFILTERTYPE minmag;
     D3DTEXTUREFILTERTYPE mip;
 };

#define NUMFILTERMODES  7
 static const _rwFilterParams _RwD3D9FilterModeConvTable[NUMFILTERMODES + NUMFILTERMODES] =
 {
     {D3DTEXF_NONE,   D3DTEXF_NONE},
     {D3DTEXF_POINT,  D3DTEXF_NONE},
     {D3DTEXF_LINEAR, D3DTEXF_NONE},
     {D3DTEXF_POINT,  D3DTEXF_POINT},
     {D3DTEXF_LINEAR, D3DTEXF_POINT},
     {D3DTEXF_POINT,  D3DTEXF_LINEAR},
     {D3DTEXF_LINEAR, D3DTEXF_LINEAR},
     /* Anisotropic modes */
     {D3DTEXF_ANISOTROPIC, D3DTEXF_NONE},
     {D3DTEXF_ANISOTROPIC, D3DTEXF_NONE},
     {D3DTEXF_ANISOTROPIC, D3DTEXF_NONE},
     {D3DTEXF_ANISOTROPIC, D3DTEXF_POINT},
     {D3DTEXF_ANISOTROPIC, D3DTEXF_POINT},
     {D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR},
     {D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR}
 };

#define NUMCULLMODES    4
 static const D3DCULL _RwD3D9CullModeConvTable[NUMCULLMODES] =
 {
     (D3DCULL)0,
     D3DCULL_NONE,
     D3DCULL_CW,
     D3DCULL_CCW
 };

#define NUMSTENCILOPERATIONS    9
 static const D3DSTENCILOP _RwD3D9StencilOpConvTable[NUMSTENCILOPERATIONS] =
 {
     (D3DSTENCILOP)0,
     D3DSTENCILOP_KEEP,
     D3DSTENCILOP_ZERO,
     D3DSTENCILOP_REPLACE,
     D3DSTENCILOP_INCRSAT,
     D3DSTENCILOP_DECRSAT,
     D3DSTENCILOP_INVERT,
     D3DSTENCILOP_INCR,
     D3DSTENCILOP_DECR
 };

#define NUMCMPFUNCTIONS    9
 static const D3DCMPFUNC _RwD3D9CmpFuncConvTable[NUMCMPFUNCTIONS] =
 {
     (D3DCMPFUNC)0,
     D3DCMP_NEVER,
     D3DCMP_LESS,
     D3DCMP_EQUAL,
     D3DCMP_LESSEQUAL,
     D3DCMP_GREATER,
     D3DCMP_NOTEQUAL,
     D3DCMP_GREATEREQUAL,
     D3DCMP_ALWAYS
 };

#define FORCE_RENDERSTATE(state, newvalue)                          \
{                                                                   \
    RenderStateCache[state].value = newvalue;                       \
    if (!RenderStateCache[state].changed)                           \
    {                                                               \
        RenderStateCache[state].changed = TRUE;                     \
        RenderStateChanges[NumRenderStateChanges++] = state;        \
    }                                                               \
}

 RwInt32& D3D9AnisotTextureOffset = *(RwInt32*)0xC9A5E8;
#define CONSTTEXTUREGETANISOT(tex) \
    (*((const RwUInt8*)(((const RwUInt8*)tex) + D3D9AnisotTextureOffset)))

 typedef union D3D9UnionParam D3D9UnionParam;
 union D3D9UnionParam
 {
     RwBlendFunction     BlendFunction;
     RwBool              Bool;
     RwCullMode          CullMode;
     RwFogType           FogType;
     RwInt32             Int32;
     RwRaster* RasterPtr;
     RwReal* RealPtr;
     RwShadeMode         ShadeMode;
     RwStencilFunction   StencilFunction;
     RwStencilOperation  StencilOperation;
     RwTextureAddressMode TextureAddressMode;
     RwTextureFilterMode TextureFilterMode;
     RwUInt32            UInt32;
     void* VoidPtr;
     RwAlphaTestFunction AlphaTestFunction;
 };
 static RwUInt32 FindAvailablePaletteIndex(void)
 {
     RwUInt32 index;


     if (NumPaletteIndexFree)
     {
         NumPaletteIndexFree--;
         index = PaletteIndexFree[NumPaletteIndexFree];
     }
     else
     {
         index = MaxPaletteIndex;

         MaxPaletteIndex++;
     }

     RWRETURN(index);
 }

  void
     AddAvailablePaletteIndex(RwUInt32 index)
 {

     if (NumPaletteIndexFree >= MaxPaletteIndexFree)
     {
         MaxPaletteIndexFree += 512;

         if (PaletteIndexFree == NULL)
         {
             PaletteIndexFree = (RwUInt16*)
                 RwMalloc(MaxPaletteIndexFree * sizeof(RwUInt16),
                     rwID_DRIVERMODULE | rwMEMHINTDUR_EVENT |
                     rwMEMHINTFLAG_RESIZABLE);
         }
         else
         {
             PaletteIndexFree = (RwUInt16*)
                 RwRealloc(PaletteIndexFree,
                     MaxPaletteIndexFree * sizeof(RwUInt16),
                     rwID_DRIVERMODULE |
                     rwMEMHINTDUR_EVENT |
                     rwMEMHINTFLAG_RESIZABLE);
         }
     }

     PaletteIndexFree[NumPaletteIndexFree++] = (RwUInt16)index;

     RWRETURNVOID();
 }

  RwInt32
      _rwD3D9FindMSB(RwInt32 num)
  {
      RwInt32 pos = -1;


      while (num)
      {
          pos++;
          num >>= 1;
      }

      RWRETURN(pos);
  }

/* Video memory raster list */
 RxD3D9VideoMemoryRaster*& VideoMemoryRasters = *(RxD3D9VideoMemoryRaster**)0xB4E9FC;
 RwFreeList*& VideoMemoryRastersFreeList = *(RwFreeList**)0xB4EA00;


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

    void RenderingEngine::SetRenderState(RwUInt32 state, RwUInt32 value)
    {
        if (RenderStateCache[state].value != value)
        {
            RenderStateCache[state].value = value;

            if (!RenderStateCache[state].changed)
            {
                RenderStateCache[state].changed = TRUE;
                RenderStateChanges[NumRenderStateChanges++] =(D3DRENDERSTATETYPE)state;
            }
        }
        RWRETURNVOID();
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

    RwBool RenderingEngine::rwD3D9RenderStateFogEnable(RwBool fog)
    {
        if (fog)
        {
            if (!_RwD3D9StateCache.fogEnable)
            {
                if (_RwD3D9DeviceCaps.RasterCaps & (D3DPRASTERCAPS_FOGTABLE | D3DPRASTERCAPS_FOGVERTEX))
                {
                    FORCE_RENDERSTATE(D3DRS_FOGENABLE, TRUE);
                    _RwD3D9StateCache.fogEnable = TRUE;
                }
            }
        }
        else
        {
            if (_RwD3D9StateCache.fogEnable)
            {
                FORCE_RENDERSTATE(D3DRS_FOGENABLE, FALSE);
                _RwD3D9StateCache.fogEnable = FALSE;
            }
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::rwD3D9RenderStateFogColor(RwUInt32 fogColor)
    {
        if (fogColor != _RwD3D9StateCache.fogColor)
        {
            FORCE_RENDERSTATE(D3DRS_FOGCOLOR, fogColor);
            _RwD3D9StateCache.fogColor = fogColor;
        }

        RWRETURN(TRUE);
    }

     RwBool RenderingEngine::rwD3D9RenderStateFogType(RwFogType fogType)
    {
        if (fogType != _RwD3D9StateCache.fogType)
        {
            if ((_RwD3D9DeviceCaps.RasterCaps & D3DPRASTERCAPS_FOGTABLE) != 0 &&
                (_RwD3D9DeviceCaps.RasterCaps & D3DPRASTERCAPS_WFOG) != 0)
            {
                FORCE_RENDERSTATE(D3DRS_FOGTABLEMODE, _RwD3D9FogConvTable[fogType]);

                if (RenderStateCache[D3DRS_FOGVERTEXMODE].value != D3DFOG_NONE)
                {
                    FORCE_RENDERSTATE(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
                }
            }
            else
            {
                if (RenderStateCache[D3DRS_FOGTABLEMODE].value != D3DFOG_NONE)
                {
                    FORCE_RENDERSTATE(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
                }

                FORCE_RENDERSTATE(D3DRS_FOGVERTEXMODE, _RwD3D9FogConvTable[fogType]);
            }

            _RwD3D9StateCache.fogType = fogType;
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::rwD3D9RenderStateFogDensity(RwReal fogDensity)
    {
        if (fogDensity != _RwD3D9StateCache.fogDensity)
        {
            FORCE_RENDERSTATE(D3DRS_FOGDENSITY, FLOATASDWORD(fogDensity));
            _RwD3D9StateCache.fogDensity = fogDensity;
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::_rwD3D9RenderStateTextureAddress(RwTextureAddressMode addressMode)
    {

        if (addressMode != _RwD3D9StateCache.stage[0].texAddressU)
        {
            _RwD3D9StateCache.stage[0].texAddressU = addressMode;

            FORCE_SAMPLERSTATE(0, D3DSAMP_ADDRESSU, _RwD3D9AddressConvTable[addressMode]);
        }

        if (addressMode != _RwD3D9StateCache.stage[0].texAddressV)
        {
            _RwD3D9StateCache.stage[0].texAddressV = addressMode;

            FORCE_SAMPLERSTATE(0, D3DSAMP_ADDRESSV, _RwD3D9AddressConvTable[addressMode]);
        }

        RWRETURN(TRUE);
    }

    RwBool  RenderingEngine::_rwD3D9RenderStateTextureAddressU(RwTextureAddressMode addressMode)
    {
        if (addressMode != _RwD3D9StateCache.stage[0].texAddressU)
        {
            _RwD3D9StateCache.stage[0].texAddressU = addressMode;

            FORCE_SAMPLERSTATE(0, D3DSAMP_ADDRESSU, _RwD3D9AddressConvTable[addressMode]);
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::_rwD3D9RenderStateTextureAddressV(RwTextureAddressMode addressMode)
    {
        if (addressMode != _RwD3D9StateCache.stage[0].texAddressV)
        {
            _RwD3D9StateCache.stage[0].texAddressV = addressMode;

            FORCE_SAMPLERSTATE(0, D3DSAMP_ADDRESSV, _RwD3D9AddressConvTable[addressMode]);
        }

        RWRETURN(TRUE);
    }

    RwBool  RenderingEngine::_rwD3D9RenderStateTextureFilter(RwTextureFilterMode filterMode)
    {
        if (_RwD3D9StateCache.stage[0].maxAnisotropy > 1)
        {
            _RwD3D9StateCache.stage[0].maxAnisotropy = 1;

            FORCE_SAMPLERSTATE(0, D3DSAMP_MAXANISOTROPY, 1);
        }

        if (filterMode != _RwD3D9StateCache.stage[0].filterMode)
        {
            const RwUInt32 minmag = _RwD3D9FilterModeConvTable[filterMode].minmag;
            const RwUInt32 mip = _RwD3D9FilterModeConvTable[filterMode].mip;

            _RwD3D9StateCache.stage[0].filterMode = filterMode;

            FORCE_SAMPLERSTATE(0, D3DSAMP_MINFILTER, minmag);
            FORCE_SAMPLERSTATE(0, D3DSAMP_MAGFILTER, minmag);

            if (SamplerStageStateD3D[0][D3DSAMP_MIPFILTER] != mip)
            {
                FORCE_SAMPLERSTATE(0, D3DSAMP_MIPFILTER, mip);
            }
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::_rwD3D9RenderStateSrcBlend(RwBlendFunction srcBlend)
    {
        if (srcBlend != _RwD3D9StateCache.srcBlend)
        {
            FORCE_RENDERSTATE(D3DRS_SRCBLEND, _RwD3D9BlendConvTable[srcBlend]);

            _RwD3D9StateCache.srcBlend = srcBlend;
        }

        RWRETURN(TRUE);
    }

    RwBool  RenderingEngine::_rwD3D9RenderStateDestBlend(RwBlendFunction dstBlend)
    {
        if (dstBlend != _RwD3D9StateCache.dstBlend)
        {
            FORCE_RENDERSTATE(D3DRS_DESTBLEND, _RwD3D9BlendConvTable[dstBlend]);

            _RwD3D9StateCache.dstBlend = dstBlend;
        }

        RWRETURN(TRUE);
    }
    
    RwBool RenderingEngine::_rwD3D9RWSetRasterStage(RwRaster* raster, RwUInt32 stage)
    {
        _rwD3D9RasterExt* rasExt;


        /*
         * Check alpha blending
         */
        if (raster)
        {
            rasExt = RASTEREXTFROMRASTER(raster);
        }
        else
        {
            rasExt = (_rwD3D9RasterExt*)NULL;
        }

        if (stage == 0)
        {
            if (raster && rasExt->alpha)
            {
                /*
                 * Enable Alpha Blending if the texture has alpha
                 */
                if (!_RwD3D9StateCache.textureAlphaBlendEnable)
                {
                    _RwD3D9StateCache.textureAlphaBlendEnable = TRUE;

                    if (!_RwD3D9StateCache.vertexAlphaBlendEnable)
                    {
                        FORCE_RENDERSTATE(D3DRS_ALPHABLENDENABLE, TRUE);

                        RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, _RwD3D9StateCache.alphaTestEnable);
                    }
                }
            }
            else
            {
                if (raster /*&& g_DXT1AlphaChecker*/)
                {
                    if (!_RwD3D9StateCache.textureAlphaBlendEnable)
                    {
                        _RwD3D9StateCache.textureAlphaBlendEnable = TRUE;

                        if (!_RwD3D9StateCache.vertexAlphaBlendEnable)
                        {
                            FORCE_RENDERSTATE(D3DRS_ALPHABLENDENABLE, TRUE);

                            RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, _RwD3D9StateCache.alphaTestEnable);
                        }
                    }
                }
                else
                {
                    if (_RwD3D9StateCache.textureAlphaBlendEnable)
                    {
                        _RwD3D9StateCache.textureAlphaBlendEnable = FALSE;

                        /*
                        * Disable alpha blending if it has not been explicity set
                        */
                        if (!_RwD3D9StateCache.vertexAlphaBlendEnable)
                        {
                            FORCE_RENDERSTATE(D3DRS_ALPHABLENDENABLE, FALSE);

                            RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
                        }
                    }
                }
            }
        }

        /*
         * Check color
         */
        if (raster != _RwD3D9StateCache.stage[stage].curTexRaster)
        {
            _RwD3D9StateCache.stage[stage].curTexRaster = raster;

            if (raster)
            {
                /* Set the texture */
                IDirect3DDevice9_SetTexture(_RwD3DDevice, stage,
                    (LPDIRECT3DBASETEXTURE9)(rasExt->texture));

                /* Set the palette if it is a palettized texture */
                if (rasExt->palette)
                {
                    IDirect3DDevice9_SetCurrentTexturePalette(_RwD3DDevice, rasExt->palette->globalindex);
                }
            }
            else
            {
                IDirect3DDevice9_SetTexture(_RwD3DDevice, stage, NULL);
            }
        }

        RWRETURN(TRUE);
    }

    RwRaster* RenderingEngine::_rwD3D9RWGetRasterStage(RwUInt32 stage)
    {
        RWRETURN(_RwD3D9StateCache.stage[stage].curTexRaster);
    }

    void  RenderingEngine::_rwD3D9SetTextureAnisotropyOffset(RwInt32 anisotTextureOffset)
    {
        D3D9AnisotTextureOffset = anisotTextureOffset;

    }

    RwBool RenderingEngine::RwD3D9SetTexture(RwTexture* texture, RwUInt32 stage)
    {
        if (texture)
        {
            RwTextureAddressMode    addressingU;
            RwTextureAddressMode    addressingV;
            RwTextureFilterMode     filtering;

            /* Addressing mode */
            addressingU = RwTextureGetAddressingU(texture);
            if (addressingU != _RwD3D9StateCache.stage[stage].texAddressU)
            {
                _RwD3D9StateCache.stage[stage].texAddressU = addressingU;

                FORCE_SAMPLERSTATE(stage, D3DSAMP_ADDRESSU, _RwD3D9AddressConvTable[addressingU]);
            }

            addressingV = RwTextureGetAddressingV(texture);
            if (addressingV != _RwD3D9StateCache.stage[stage].texAddressV)
            {
                _RwD3D9StateCache.stage[stage].texAddressV = addressingV;

                FORCE_SAMPLERSTATE(stage, D3DSAMP_ADDRESSV, _RwD3D9AddressConvTable[addressingV]);
            }

            /* Filter mode */
            filtering = RwTextureGetFilterMode(texture);

            if (D3D9AnisotTextureOffset > 0)
            {
                const RwInt32 anisotropy = CONSTTEXTUREGETANISOT(texture);

                if (_RwD3D9StateCache.stage[stage].maxAnisotropy != anisotropy)
                {
                    _RwD3D9StateCache.stage[stage].maxAnisotropy = anisotropy;

                    FORCE_SAMPLERSTATE(stage, D3DSAMP_MAXANISOTROPY, anisotropy);
                }

                if (anisotropy > 1)
                {
                    filtering = (RwTextureFilterMode)NUMFILTERMODES;
                }
            }

            if (_RwD3D9StateCache.stage[stage].filterMode != filtering)
            {
                D3DTEXTUREFILTERTYPE    minmag, mip;

                _RwD3D9StateCache.stage[stage].filterMode = filtering;

                minmag = _RwD3D9FilterModeConvTable[filtering].minmag;
                mip = _RwD3D9FilterModeConvTable[filtering].mip;

                if (minmag == D3DTEXF_ANISOTROPIC)
                {
                    FORCE_SAMPLERSTATE(stage, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
                    if (_RwD3D9DeviceCaps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC)
                    {
                        FORCE_SAMPLERSTATE(stage, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
                    }
                    else
                    {
                        FORCE_SAMPLERSTATE(stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                    }
                }
                else
                {
                    FORCE_SAMPLERSTATE(stage, D3DSAMP_MINFILTER, minmag);
                    FORCE_SAMPLERSTATE(stage, D3DSAMP_MAGFILTER, minmag);
                }

                FORCE_SAMPLERSTATE(stage, D3DSAMP_MIPFILTER, mip);
            }

            /* Raster */
            _rwD3D9RWSetRasterStage(texture->raster, stage);

        }
        else
        {
            if (stage == 0)
            {
                if (_RwD3D9StateCache.textureAlphaBlendEnable)
                {
                    _RwD3D9StateCache.textureAlphaBlendEnable = FALSE;

                    /*
                    * Disable alpha blending if it has not been explicity set
                    */
                    if (!_RwD3D9StateCache.vertexAlphaBlendEnable)
                    {
                        FORCE_RENDERSTATE(D3DRS_ALPHABLENDENABLE, FALSE);

                        SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
                    }
                }
            }

            if (NULL != _RwD3D9StateCache.stage[stage].curTexRaster)
            {
                _RwD3D9StateCache.stage[stage].curTexRaster = NULL;

                IDirect3DDevice9_SetTexture(_RwD3DDevice, stage, NULL);
            }
        }

        RWRETURN(TRUE);
    }
     RwBool RenderingEngine::rwD3D9RenderStateZWriteEnable(RwBool enable)
    {

        if (enable)
        {
            if (!_RwD3D9StateCache.zWriteEnable)
            {
                if (!_RwD3D9StateCache.zTestEnable)
                {
                    FORCE_RENDERSTATE(D3DRS_ZENABLE, TRUE);
                }

                FORCE_RENDERSTATE(D3DRS_ZWRITEENABLE, TRUE);

                _RwD3D9StateCache.zWriteEnable = TRUE;
            }
        }
        else
        {
            if (_RwD3D9StateCache.zWriteEnable)
            {
                if (!_RwD3D9StateCache.zTestEnable)
                {
                    FORCE_RENDERSTATE(D3DRS_ZENABLE, FALSE);
                }

                FORCE_RENDERSTATE(D3DRS_ZWRITEENABLE, FALSE);

                _RwD3D9StateCache.zWriteEnable = FALSE;
            }
        }
        RWRETURN(TRUE);
    }

     RwBool RenderingEngine::rwD3D9RenderStateZTestEnable(RwBool enable)
    {

        if (enable)
        {
            if (!_RwD3D9StateCache.zTestEnable)
            {
                if (!_RwD3D9StateCache.zWriteEnable)
                {
                    FORCE_RENDERSTATE(D3DRS_ZENABLE, TRUE);
                }

                FORCE_RENDERSTATE(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

                _RwD3D9StateCache.zTestEnable = TRUE;
            }
        }
        else
        {
            if (_RwD3D9StateCache.zTestEnable)
            {
                if (!_RwD3D9StateCache.zWriteEnable)
                {
                    FORCE_RENDERSTATE(D3DRS_ZENABLE, FALSE);
                }

                FORCE_RENDERSTATE(D3DRS_ZFUNC, D3DCMP_ALWAYS);

                _RwD3D9StateCache.zTestEnable = FALSE;
            }
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::rwD3D9RenderStateShadeMode(RwShadeMode shadeMode)
    {
        if (shadeMode != _RwD3D9StateCache.shadeMode)
        {
            FORCE_RENDERSTATE(D3DRS_SHADEMODE,  _RwD3D9ShadeModeConvTable[shadeMode]);
            _RwD3D9StateCache.shadeMode = shadeMode;
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::_rwD3D9RenderStateVertexAlphaEnable(RwBool enable)
    {
        if (enable)
        {
            if (!_RwD3D9StateCache.vertexAlphaBlendEnable)
            {
                _RwD3D9StateCache.vertexAlphaBlendEnable = TRUE;

                if (!_RwD3D9StateCache.textureAlphaBlendEnable)
                {
                    FORCE_RENDERSTATE(D3DRS_ALPHABLENDENABLE, TRUE);

                    RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, _RwD3D9StateCache.alphaTestEnable);
                }
            }
        }
        else
        {
            if (_RwD3D9StateCache.vertexAlphaBlendEnable)
            {
                _RwD3D9StateCache.vertexAlphaBlendEnable = FALSE;

                if (!_RwD3D9StateCache.textureAlphaBlendEnable)
                {
                    FORCE_RENDERSTATE(D3DRS_ALPHABLENDENABLE, FALSE);

                    RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
                }
            }
        }

        RWRETURN(TRUE);
    }

    RwBool  RenderingEngine::_rwD3D9RenderStateIsVertexAlphaEnable(void)
    {
        RWRETURN(_RwD3D9StateCache.vertexAlphaBlendEnable);
    }

    RwBool RenderingEngine::rwD3D9RenderStateBorderColor(RwUInt32 borderColor)
    {

        if (borderColor != _RwD3D9StateCache.stage[0].borderColor)
        {
            _RwD3D9StateCache.stage[0].borderColor = borderColor;

            FORCE_SAMPLERSTATE(0, D3DSAMP_BORDERCOLOR, borderColor);
        }

        RWRETURN(TRUE);
    }


     RwBool rwD3D9RenderStateTexturePerspective(RwBool persp)
    {
        /* Only can set on */
        if (persp)
        {
            RWRETURN(TRUE);
        }

        /* If trying to set off, fail */
        RWRETURN(FALSE);
    }

    RwBool RenderingEngine::rwD3D9RenderStateCullMode(RwCullMode cullMode)
    {
        if (cullMode != _RwD3D9StateCache.cullMode)
        {
            FORCE_RENDERSTATE(D3DRS_CULLMODE, _RwD3D9CullModeConvTable[cullMode]);

            _RwD3D9StateCache.cullMode = cullMode;
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::_rwD3D9RenderStateStencilEnable(RwBool enable)
    {

        if (enable != _RwD3D9StateCache.stencilEnable)
        {
            FORCE_RENDERSTATE(D3DRS_STENCILENABLE, enable);

            _RwD3D9StateCache.stencilEnable = enable;
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::_rwD3D9RenderStateStencilFail(RwStencilOperation operation)
    {
        if (operation != _RwD3D9StateCache.stencilFailOp)
        {
            FORCE_RENDERSTATE(D3DRS_STENCILFAIL, _RwD3D9StencilOpConvTable[operation]);

            _RwD3D9StateCache.stencilFailOp = operation;
        }

        RWRETURN(TRUE);
    }

    RwBool  RenderingEngine::_rwD3D9RenderStateStencilZFail(RwStencilOperation operation)
    {
        if (operation != _RwD3D9StateCache.stencilZFailOp)
        {
            FORCE_RENDERSTATE(D3DRS_STENCILZFAIL, _RwD3D9StencilOpConvTable[operation]);

            _RwD3D9StateCache.stencilZFailOp = operation;
        }

        RWRETURN(TRUE);
    }

    RwBool  RenderingEngine::_rwD3D9RenderStateStencilPass(RwStencilOperation operation)
    {
        if (operation != _RwD3D9StateCache.stencilPassOp)
        {
            FORCE_RENDERSTATE(D3DRS_STENCILPASS, _RwD3D9StencilOpConvTable[operation]);

            _RwD3D9StateCache.stencilPassOp = operation;
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::_rwD3D9RenderStateStencilFunction(RwStencilFunction function)
    {
        if (function != _RwD3D9StateCache.stencilFunction)
        {
            FORCE_RENDERSTATE(D3DRS_STENCILFUNC, _RwD3D9CmpFuncConvTable[function]);

            _RwD3D9StateCache.stencilFunction = function;
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::_rwD3D9RenderStateStencilFunctionRef(RwInt32 ref)
    {
        if (ref != _RwD3D9StateCache.stencilRef)
        {
            FORCE_RENDERSTATE(D3DRS_STENCILREF, ref);

            _RwD3D9StateCache.stencilRef = ref;
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::_rwD3D9RenderStateStencilFunctionMask(RwInt32 mask)
    {
        if (mask != _RwD3D9StateCache.stencilMask)
        {
            FORCE_RENDERSTATE(D3DRS_STENCILMASK, mask);

            _RwD3D9StateCache.stencilMask = mask;
        }

        RWRETURN(TRUE);
    }

    RwBool  RenderingEngine::_rwD3D9RenderStateStencilFunctionWriteMask(RwInt32 writeMask)
    {
        if (writeMask != _RwD3D9StateCache.stencilWriteMask)
        {
            FORCE_RENDERSTATE(D3DRS_STENCILWRITEMASK, writeMask);

            _RwD3D9StateCache.stencilWriteMask = writeMask;
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::_rwD3D9RWSetRenderState(RwRenderState state, void* param)
    {
        RwBool  result = FALSE;
        D3D9UnionParam uparam;
        /*
         * Need to be in a camera update cliche for this to work
         */

        uparam.VoidPtr = param;

        switch (state)
        {
            /***************************** FOG ******************************/

        case rwRENDERSTATEFOGENABLE:
            result = rwD3D9RenderStateFogEnable(uparam.Bool);
            break;

        case rwRENDERSTATEFOGCOLOR:
            result = rwD3D9RenderStateFogColor(uparam.UInt32);
            break;

        case rwRENDERSTATEFOGTYPE:
            result = rwD3D9RenderStateFogType(uparam.FogType);
            break;

        case rwRENDERSTATEFOGDENSITY:
            result = rwD3D9RenderStateFogDensity(*uparam.RealPtr);
            break;

            /********************** TEXTURE ADDRESSING **********************/

        case rwRENDERSTATETEXTUREADDRESS:
            result = _rwD3D9RenderStateTextureAddress(uparam.TextureAddressMode);
            break;

        case rwRENDERSTATETEXTUREADDRESSU:
            result = _rwD3D9RenderStateTextureAddressU(uparam.TextureAddressMode);
            break;

        case rwRENDERSTATETEXTUREADDRESSV:
            result = _rwD3D9RenderStateTextureAddressV(uparam.TextureAddressMode);
            break;

            /********************* TEXTURE FILTER MODE **********************/

        case rwRENDERSTATETEXTUREFILTER:
            result = _rwD3D9RenderStateTextureFilter(uparam.TextureFilterMode);
            break;

            /************************ TEXTURE RASTER ************************/

        case rwRENDERSTATETEXTURERASTER:
            result = _rwD3D9RWSetRasterStage(uparam.RasterPtr, 0);
            break;

            /************************ Z BUFFER WRITE ************************/

        case rwRENDERSTATEZWRITEENABLE:
            result = rwD3D9RenderStateZWriteEnable(uparam.Bool);
            break;

            /************************ Z BUFFER TEST *************************/

        case rwRENDERSTATEZTESTENABLE:
            result = rwD3D9RenderStateZTestEnable(uparam.Bool);
            break;

            /************************ BLENDING MODES ************************/

        case rwRENDERSTATESRCBLEND:
            result = _rwD3D9RenderStateSrcBlend(uparam.BlendFunction);
            break;

        case rwRENDERSTATEDESTBLEND:
            result = _rwD3D9RenderStateDestBlend(uparam.BlendFunction);
            break;

            /************************* SHADING MODE *************************/

        case rwRENDERSTATESHADEMODE:
            result = rwD3D9RenderStateShadeMode(uparam.ShadeMode);
            break;

            /************************* VERTEX ALPHA *************************/

        case rwRENDERSTATEVERTEXALPHAENABLE:
            result = _rwD3D9RenderStateVertexAlphaEnable(uparam.Bool);
            break;

            /************************ BORDER COLOR **************************/

        case rwRENDERSTATEBORDERCOLOR:
            result = rwD3D9RenderStateBorderColor(uparam.UInt32);
            break;

            /*************** PERSPECTIVELY CORRECT TEXTUREING ***************/

        case rwRENDERSTATETEXTUREPERSPECTIVE:
            result = rwD3D9RenderStateTexturePerspective(uparam.Bool);
            break;

            /************************* CULLING MODE *************************/

        case rwRENDERSTATECULLMODE:
            result = rwD3D9RenderStateCullMode(uparam.CullMode);
            break;

            /************************** STENCIL BUFFER **************************/

        case rwRENDERSTATESTENCILENABLE:
            result = _rwD3D9RenderStateStencilEnable(uparam.Bool);
            break;

        case rwRENDERSTATESTENCILFAIL:
            result = _rwD3D9RenderStateStencilFail(uparam.StencilOperation);
            break;

        case rwRENDERSTATESTENCILZFAIL:
            result = _rwD3D9RenderStateStencilZFail(uparam.StencilOperation);
            break;

        case rwRENDERSTATESTENCILPASS:
            result = _rwD3D9RenderStateStencilPass(uparam.StencilOperation);
            break;

        case rwRENDERSTATESTENCILFUNCTION:
            result = _rwD3D9RenderStateStencilFunction(uparam.StencilFunction);
            break;

        case rwRENDERSTATESTENCILFUNCTIONREF:
            result = _rwD3D9RenderStateStencilFunctionRef(uparam.Int32);
            break;

        case rwRENDERSTATESTENCILFUNCTIONMASK:
            result = _rwD3D9RenderStateStencilFunctionMask(uparam.Int32);
            break;

        case rwRENDERSTATESTENCILFUNCTIONWRITEMASK:
            result = _rwD3D9RenderStateStencilFunctionWriteMask(uparam.Int32);
            break;

        case rwRENDERSTATEALPHATESTFUNCTION:
            if (uparam.AlphaTestFunction != _RwD3D9StateCache.alphaTestFunction)
            {
                _RwD3D9StateCache.alphaTestFunction = uparam.AlphaTestFunction;

                _RwD3D9StateCache.alphaTestEnable = (uparam.AlphaTestFunction != rwALPHATESTFUNCTIONALWAYS);

                if (RenderStateCache[D3DRS_ALPHABLENDENABLE].value)
                {
                    RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, _RwD3D9StateCache.alphaTestEnable);
                }

                FORCE_RENDERSTATE(D3DRS_ALPHAFUNC,
                    _RwD3D9CmpFuncConvTable[uparam.AlphaTestFunction]);
            }
            result = TRUE;
            break;

        case rwRENDERSTATEALPHATESTFUNCTIONREF:
            RwD3D9SetRenderState(D3DRS_ALPHAREF, uparam.UInt32);
            result = TRUE;
            break;

            /************************ NOT SUPPORTED *************************/

        case rwRENDERSTATENARENDERSTATE:
            break;

        case rwRENDERSTATEFORCEENUMSIZEINT:
            break;

#if (0)
        default:
            result = FALSE;
            break;
#endif /* (0) */
        }

        RWRETURN(result);
    }


    void RenderingEngine::D3D9RenderStateCacheClear()
    {
        RwUInt32 state;

        memset(RenderStateD3D, 0xffffffff, 210 * sizeof(RwUInt32));

        /* Change special values */
        RenderStateD3D[D3DRS_FOGCOLOR] = 0;
        RenderStateD3D[D3DRS_AMBIENT] = 0;
        RenderStateD3D[D3DRS_STENCILMASK] = 0;
        RenderStateD3D[D3DRS_STENCILWRITEMASK] = 0;

        /* Initialize the cache */
        for (state = 0; state < 210; state++)
        {
            RenderStateCache[state].value = RenderStateD3D[state];
            RenderStateCache[state].changed = FALSE;
        }

        NumRenderStateChanges = 0;

        RWRETURNVOID();
    }

    void RenderingEngine::D3D9TextureStageStateCacheClear()
    {
        RwUInt32 stage, type;

        memset(TextureStageStateD3D, 0xffffffff, RWD3D9_MAX_TEXTURE_STAGES * 33 * sizeof(RwUInt32));

        /* Initialize the cache */
        for (stage = 0; stage < RWD3D9_MAX_TEXTURE_STAGES; stage++)
        {
            for (type = 0; type < 33; type++)
            {
                TextureStageStateCache[stage][type].value = 0xffffffff;
                TextureStageStateCache[stage][type].changed = FALSE;
            }
        }

        NumTextureStageStateChanges = 0;

        RWRETURNVOID();
    }

    void RenderingEngine::D3D9SamplerStageStateCacheClear()
    {
        memset(SamplerStageStateD3D, 0xffffffff, RWD3D9_MAX_TEXTURE_STAGES * 14 * sizeof(RwUInt32));

        RWRETURNVOID();
    }

    void RenderingEngine::_rwD3D9RenderStateOpen()
    {
        RwInt32 i, dither;

        D3D9RenderStateCacheClear();
        D3D9TextureStageStateCacheClear();
        D3D9SamplerStageStateCacheClear();
        //D3D9ClearCacheMaterials();

        /* Fog */
        _RwD3D9StateCache.fogEnable = FALSE;
        _RwD3D9StateCache.fogType = rwFOGTYPELINEAR;
        _RwD3D9StateCache.fogDensity = 1.0f;
        _RwD3D9StateCache.fogColor = 0;

        SetRenderState(D3DRS_FOGENABLE, FALSE);

        if ((_RwD3D9DeviceCaps.RasterCaps & D3DPRASTERCAPS_FOGTABLE) != 0 &&
            (_RwD3D9DeviceCaps.RasterCaps & D3DPRASTERCAPS_WFOG) != 0)
        {
            SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
            SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
        }
        else
        {
            SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
            SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
        }

        SetRenderState(D3DRS_FOGCOLOR, 0);

        /* Shading mode */
        SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
        _RwD3D9StateCache.shadeMode = rwSHADEMODEGOURAUD;

        /* Set Z Compare mode */
        SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
        SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
        SetRenderState(D3DRS_ZENABLE, TRUE);

        _RwD3D9StateCache.zWriteEnable = TRUE;
        _RwD3D9StateCache.zTestEnable = TRUE;

        /* Stencil */
        SetRenderState(D3DRS_STENCILENABLE, FALSE);
        SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
        SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
        SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
        SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
        SetRenderState(D3DRS_STENCILREF, 0);
        SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
        SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);

        _RwD3D9StateCache.stencilEnable = FALSE;
        _RwD3D9StateCache.stencilFailOp = rwSTENCILOPERATIONKEEP;
        _RwD3D9StateCache.stencilZFailOp = rwSTENCILOPERATIONKEEP;
        _RwD3D9StateCache.stencilPassOp = rwSTENCILOPERATIONKEEP;
        _RwD3D9StateCache.stencilFunction = rwSTENCILFUNCTIONALWAYS;
        _RwD3D9StateCache.stencilRef = 0;
        _RwD3D9StateCache.stencilMask = 0xFFFFFFFF;
        _RwD3D9StateCache.stencilWriteMask = 0xFFFFFFFF;

        /* Texture stages */
        for (i = 0; i < RWD3D9_MAX_TEXTURE_STAGES; i++)
        {
            /* Texture */
            IDirect3DDevice9_SetTexture(_RwD3DDevice, i, NULL);
            _RwD3D9StateCache.stage[i].curTexRaster = NULL;

            /* Texture filter mode - Liner/Trilinear */
            if (_RwD3D9DeviceCaps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR)
            {
                SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
                SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                _RwD3D9StateCache.stage[i].filterMode = rwFILTERLINEAR;
            }
            else
            {
                SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_POINT);
                SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
                _RwD3D9StateCache.stage[i].filterMode = rwFILTERNEAREST;
            }

            SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

            /* Texture addressing mode */
            SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
            SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

            _RwD3D9StateCache.stage[i].texAddressU = rwTEXTUREADDRESSWRAP;
            _RwD3D9StateCache.stage[i].texAddressV = rwTEXTUREADDRESSWRAP;

            /* Border color */
            SetSamplerState(i, D3DSAMP_BORDERCOLOR, 0xFF000000);
            _RwD3D9StateCache.stage[i].borderColor = 0xFF000000;

            /* Max Anisotropy Level: 1 to disable it */
            _RwD3D9StateCache.stage[i].maxAnisotropy = 1;
            SetSamplerState(i, D3DSAMP_MAXANISOTROPY, 1);

            /* Disable stage if not the first one */
            if (i)
            {
                SetTextureStageState(i, D3DTSS_COLOROP, D3DTOP_DISABLE);
                SetTextureStageState(i, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
            }
        }

        /* Set default blending modes */
        SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

        SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

        _RwD3D9StateCache.srcBlend = rwBLENDSRCALPHA;
        _RwD3D9StateCache.dstBlend = rwBLENDINVSRCALPHA;

        /* Set up the alpha compare function for future use */
        SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
        _RwD3D9StateCache.alphaTestFunction = rwALPHATESTFUNCTIONGREATER;
        _RwD3D9StateCache.alphaTestEnable = TRUE;

        SetRenderState(D3DRS_ALPHAREF, 0x00000000);

        /* Disable alpha blending */
        SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

        _RwD3D9StateCache.vertexAlphaBlendEnable = FALSE;
        _RwD3D9StateCache.textureAlphaBlendEnable = FALSE;

        /* Turn lighting off */
        SetRenderState(D3DRS_LIGHTING, FALSE);

        /* Cull clockwise facing triangles */
        SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
        _RwD3D9StateCache.cullMode = rwCULLMODECULLBACK;

        /* Vertex color only until a texture has been set */
        SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

        SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
        SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

        /* Diffuse, Specular & Ambient colors come from the material */
        SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);

        SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);

        SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);

        /* Use dithering if possible */
        dither = (_RwD3D9DeviceCaps.RasterCaps & D3DPRASTERCAPS_DITHER) ? TRUE : FALSE;
        SetRenderState(D3DRS_DITHERENABLE, dither);

        SetRenderState(D3DRS_SPECULARENABLE, FALSE);

        SetRenderState(D3DRS_LOCALVIEWER, FALSE);

        SetRenderState(D3DRS_AMBIENT, 0xffffffff);

        SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);

        RenderStateFlushCache();
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
        SetRenderState(D3DRS_FOGSTART, FLOATASDWORD(camera->fogPlane));
        SetRenderState(D3DRS_FOGEND, FLOATASDWORD(camera->farPlane));


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
        rv = _rxD3D9VideoMemoryRasterListRestore();

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

    const void* RenderingEngine::D3D9GetCaps()
    {
        return &_RwD3D9DeviceCaps;
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

    RwBool RenderingEngine::D3D9RasterLock(void* pixelsIn, void* rasterIn, RwInt32 accessMode)
    {
        RwUInt8** pixels = (RwUInt8**)pixelsIn;
        RwRaster* raster = (RwRaster*)rasterIn;
        RwRaster* topRaster;
        _rwD3D9RasterExt* rasExt;
        _rwD3D9RasterExt* topRasExt;
        RwUInt8         mipLevel;
        RwUInt32        flags;
        HRESULT         hr = D3D_OK;


        /* Can't rasters that have already been locked */
        if (raster->cpPixels != NULL)
        {
            RWRETURN(FALSE);
        }

        rasExt = RASTEREXTFROMRASTER(raster);

        /* Get the top level raster as this is the only one with a valid texture */
        topRaster = raster;
        while ((topRaster = topRaster->parent) != topRaster->parent) {};

        topRasExt = RASTEREXTFROMRASTER(topRaster);

        /* Prepare lock info */
        mipLevel = (RwUInt8)((accessMode & (RwInt32)0xFF00) >> 8);

        flags = D3DLOCK_NOSYSLOCK;

        if (!(accessMode & rwRASTERLOCKWRITE))
        {
            flags |= D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_READONLY;
        }

        switch (raster->cType & rwRASTERTYPEMASK)
        {
        case rwRASTERTYPENORMAL:
        case rwRASTERTYPETEXTURE:
        {
            if (topRasExt->cube)
            {
                hr = IDirect3DCubeTexture9_GetCubeMapSurface((LPDIRECT3DCUBETEXTURE9)topRasExt->texture,
                    (D3DCUBEMAP_FACES)topRasExt->face,
                    mipLevel,
                    &(rasExt->lockedSurface));
            }
            else
            {
                hr = IDirect3DTexture9_GetSurfaceLevel(topRasExt->texture, mipLevel, &(rasExt->lockedSurface));
            }

            if (SUCCEEDED(hr))
            {
                if (topRaster == raster)
                {
                    hr = IDirect3DSurface9_LockRect(rasExt->lockedSurface,
                        &rasExt->lockedRect,
                        NULL,
                        flags);
                }
                else
                {
                    RECT rect;

                    rect.left = raster->nOffsetX;
                    rect.top = raster->nOffsetY;
                    rect.right = raster->nOffsetX + raster->width;
                    rect.bottom = raster->nOffsetY + raster->height;

                    hr = IDirect3DSurface9_LockRect(rasExt->lockedSurface,
                        &rasExt->lockedRect,
                        &rect,
                        flags);
                }
            }

            if (SUCCEEDED(hr))
            {
                if (accessMode & rwRASTERLOCKREAD)
                {
                    raster->privateFlags |= rwRASTERPIXELLOCKEDREAD;
                }

                if (accessMode & rwRASTERLOCKWRITE)
                {
                    raster->privateFlags |= rwRASTERPIXELLOCKEDWRITE;
                }
            }
            else
            {
                RWRETURN(FALSE);
            }
        }
        break;

        case rwRASTERTYPECAMERATEXTURE:
        case rwRASTERTYPECAMERA:
        {
            LPSURFACE       surface;
            D3DSURFACE_DESC d3dDesc;

            if ((raster->cType & rwRASTERTYPEMASK) == rwRASTERTYPECAMERATEXTURE)
            {
                if (topRasExt->cube)
                {
                    hr = IDirect3DCubeTexture9_GetCubeMapSurface((LPDIRECT3DCUBETEXTURE9)topRasExt->texture,
                        (D3DCUBEMAP_FACES)topRasExt->face,
                        mipLevel,
                        &surface);
                }
                else
                {
                    hr = IDirect3DTexture9_GetSurfaceLevel(topRasExt->texture, mipLevel, &surface);
                }
            }
            else
            {
                if (topRasExt->swapChain)
                {
                    surface = (LPSURFACE)topRasExt->texture;
                }
                else
                {
                    surface = RwD3D9RenderSurface;
                }

                hr = D3D_OK;
            }

            if (SUCCEEDED(hr))
            {
                IDirect3DSurface9_GetDesc(surface, &d3dDesc);

                /* Create a system memory surface */
                hr = IDirect3DDevice9_CreateOffscreenPlainSurface(_RwD3DDevice,
                    d3dDesc.Width,
                    d3dDesc.Height,
                    d3dDesc.Format,
                    D3DPOOL_SYSTEMMEM,
                    &(rasExt->lockedSurface),
                    NULL);

                if (SUCCEEDED(hr))
                {
                    /* Copy the contents if needed */
                    if (accessMode & rwRASTERLOCKREAD)
                    {
                        IDirect3DDevice9_GetRenderTargetData(_RwD3DDevice,
                            surface,
                            rasExt->lockedSurface);
                    }

                    if (SUCCEEDED(hr))
                    {
                        if (topRaster == raster)
                        {
                            hr = IDirect3DSurface9_LockRect(rasExt->lockedSurface,
                                &rasExt->lockedRect,
                                NULL,
                                flags);
                        }
                        else
                        {
                            RECT rect;

                            rect.left = raster->nOffsetX;
                            rect.top = raster->nOffsetY;
                            rect.right = raster->nOffsetX + raster->width;
                            rect.bottom = raster->nOffsetY + raster->height;

                            hr = IDirect3DSurface9_LockRect(rasExt->lockedSurface,
                                &rasExt->lockedRect,
                                &rect,
                                flags);
                        }

                        if (SUCCEEDED(hr))
                        {
                            if (accessMode & rwRASTERLOCKREAD)
                            {
                                raster->privateFlags |= rwRASTERPIXELLOCKEDREAD;
                            }

                            if (accessMode & rwRASTERLOCKWRITE)
                            {
                                raster->privateFlags |= rwRASTERPIXELLOCKEDWRITE;
                            }
                        }
                    }

                    if (FAILED(hr))
                    {
                        IDirect3DSurface9_Release(rasExt->lockedSurface);
                    }
                }

                if ((raster->cType & rwRASTERTYPEMASK) == rwRASTERTYPECAMERATEXTURE)
                {
                    IDirect3DSurface9_Release(surface);
                }
            }

            if (FAILED(hr))
            {
                RWRETURN(FALSE);
            }
        }
        break;

        case rwRASTERTYPEZBUFFER:
        default:
            RWRETURN(FALSE);
            break;
        }

        /* Pixels */
        raster->cpPixels = (RwUInt8*)rasExt->lockedRect.pBits;

        /* Cache original width, height & stride */
        raster->originalWidth = raster->width;
        raster->originalHeight = raster->height;

        /* Mip level width, height & stride */
        raster->width = raster->width >> mipLevel;
        raster->height = raster->height >> mipLevel;

        /* Clamp width and height to 1 */
        if (raster->width == 0)
        {
            raster->width = 1;
        }

        if (raster->height == 0)
        {
            raster->height = 1;
        }

        /* Set the stride */
        raster->stride = rasExt->lockedRect.Pitch;

        /* Cache the mip level locked */
        rasExt->lockedMipLevel = mipLevel;

        /* Save off the pixel pointer */
        (*pixels) = raster->cpPixels;

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::D3D9RasterUnlock(void* unused1 __RWUNUSED__, void* rasterIn, RwInt32 unused3 __RWUNUSED__)
    {
        RwRaster* raster = (RwRaster*)rasterIn;
        _rwD3D9RasterExt* rasExt;

        /* Can't unlock rasters that have not been locked */
        if (raster->cpPixels == NULL)
        {
            RWRETURN(FALSE);
        }

        rasExt = RASTEREXTFROMRASTER(raster);

        switch (raster->cType & rwRASTERTYPEMASK)
        {
        case rwRASTERTYPENORMAL:
        case rwRASTERTYPETEXTURE:
        {
            IDirect3DSurface9_UnlockRect(rasExt->lockedSurface);

            IDirect3DSurface9_Release(rasExt->lockedSurface);

            /* Restore the original width, height & stride */
            raster->width = raster->originalWidth;
            raster->height = raster->originalHeight;

            raster->stride = 0;
            raster->cpPixels = NULL;

            if ((raster->privateFlags & rwRASTERPIXELLOCKEDWRITE) &&
                (raster->cFormat & (rwRASTERFORMATAUTOMIPMAP >> 8)) &&
                (rasExt->lockedMipLevel == 0) &&
                (rasExt->automipmapgen == 0))
            {
                RwRaster* topRaster;

                raster->privateFlags = (RwUInt8)(raster->privateFlags & ~rwRASTERPIXELLOCKED);

                rasExt->lockedMipLevel = (RwUInt8)-1;

                /* Get the top level raster as this is the only one with a valid texture */
                topRaster = raster;
                while ((topRaster = topRaster->parent) != topRaster->parent) {};

                RwTextureRasterGenerateMipmaps(topRaster, NULL);
            }
            else
            {
                raster->privateFlags = (RwUInt8)(raster->privateFlags & ~rwRASTERPIXELLOCKED);
            }
        }
        break;

        case rwRASTERTYPECAMERATEXTURE:
        case rwRASTERTYPECAMERA:
        {
            /* Restore the original width, height & stride */
            raster->width = raster->originalWidth;
            raster->height = raster->originalHeight;

            raster->stride = 0;
            raster->cpPixels = NULL;

            /* Unlock system memory surface */
            IDirect3DSurface9_UnlockRect(rasExt->lockedSurface);

            /* Update video memory surface if needed */
            if (raster->privateFlags & rwRASTERPIXELLOCKEDWRITE)
            {
                LPSURFACE           surface;
                HRESULT             hr;
                RwRaster* topRaster;
                _rwD3D9RasterExt* topRasExt;


                /* Get the top level raster as this is the only one with a valid texture */
                topRaster = raster;
                while ((topRaster = topRaster->parent) != topRaster->parent) {};

                topRasExt = RASTEREXTFROMRASTER(topRaster);

                if ((raster->cType & rwRASTERTYPEMASK) == rwRASTERTYPECAMERATEXTURE)
                {
                    if (topRasExt->cube)
                    {
                        hr = IDirect3DCubeTexture9_GetCubeMapSurface((LPDIRECT3DCUBETEXTURE9)topRasExt->texture,
                            (D3DCUBEMAP_FACES)topRasExt->face,
                            rasExt->lockedMipLevel,
                            &surface);
                    }
                    else
                    {
                        hr = IDirect3DTexture9_GetSurfaceLevel(topRasExt->texture,
                            rasExt->lockedMipLevel,
                            &surface);
                    }
                }
                else
                {
                    if (topRasExt->swapChain)
                    {
                        surface = (LPSURFACE)topRasExt->texture;
                    }
                    else
                    {
                        surface = RwD3D9RenderSurface;
                    }

                    hr = D3D_OK;
                }

                if (SUCCEEDED(hr))
                {
                    if (topRaster == raster)
                    {
                        hr = IDirect3DDevice9_UpdateSurface(_RwD3DDevice, rasExt->lockedSurface, NULL, surface, NULL);
                    }
                    else
                    {
                        RECT rect;
                        POINT pt;

                        rect.left = raster->nOffsetX;
                        rect.top = raster->nOffsetY;
                        rect.right = rect.left + raster->width;
                        rect.bottom = rect.top + raster->height;

                        pt.x = raster->nOffsetX;
                        pt.y = raster->nOffsetY;

                        hr = IDirect3DDevice9_UpdateSurface(_RwD3DDevice, rasExt->lockedSurface, &rect, surface, &pt);
                    }

                    if ((raster->cType & rwRASTERTYPEMASK) == rwRASTERTYPECAMERATEXTURE)
                    {
                        IDirect3DSurface9_Release(surface);
                    }
                }

                IDirect3DSurface9_Release(rasExt->lockedSurface);

                raster->privateFlags = (RwUInt8)(raster->privateFlags & ~rwRASTERPIXELLOCKED);

                if ((raster->cType & rwRASTERTYPEMASK) == rwRASTERTYPECAMERATEXTURE)
                {
                    if ((raster->cFormat & (rwRASTERFORMATAUTOMIPMAP >> 8)) &&
                        (rasExt->lockedMipLevel == 0))
                    {
                        rasExt->lockedMipLevel = (RwUInt8)-1;

                        RwTextureRasterGenerateMipmaps(topRaster, NULL);
                    }
                }
            }
            else
            {
                IDirect3DSurface9_Release(rasExt->lockedSurface);

                raster->privateFlags = (RwUInt8)(raster->privateFlags & ~rwRASTERPIXELLOCKED);
            }
        }
        break;

        case rwRASTERTYPEZBUFFER:
        default:
            RWRETURN(FALSE);
            break;
        }

        RWRETURN(TRUE);
    }

    RwInt32 RenderingEngine::D3D9FindCorrectRasterFormat(RwRasterType type, RwInt32 flags)
    {
        RwUInt32    format = flags & rwRASTERFORMATMASK;

        switch (type)
        {
        case rwRASTERTYPENORMAL:
        case rwRASTERTYPETEXTURE:
            if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATDEFAULT)
            {
                /* Check if we are requesting a default pixel format palette texture */
                if (format & rwRASTERFORMATPAL8)
                {
                    if (_rwD3D9CheckValidTextureFormat(D3DFMT_P8))
                    {
                        if (_RwD3D9DeviceCaps.TextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE)
                        {
                            format |= rwRASTERFORMAT8888;
                        }
                        else
                        {
                            format |= rwRASTERFORMAT888;
                        }
                    }
                    else
                    {
                        format &= (~rwRASTERFORMATPAL8);
                    }
                }

                if ((format & rwRASTERFORMATPAL8) == 0)
                {
                    /*
                     * By default textures are 8888, but make mipmap generation flag and
                     * palletized flags persist
                     */
                    if (_rwD3D9CheckValidTextureFormat(D3DFMT_A8R8G8B8))
                    {
                        format |= rwRASTERFORMAT8888;
                    }
                    else
                    {
                        if (_rwD3D9CheckValidTextureFormat(D3DFMT_A4R4G4B4))
                        {
                            format |= rwRASTERFORMAT4444;
                        }
                        else
                        {
                            format |= rwRASTERFORMAT1555;
                        }
                    }
                }
            }
            else
            {
                /* No support for 4 bits palettes */
                if (format & rwRASTERFORMATPAL4)
                {
                    /* Change it to a 8 bits palette */
                    format &= (~rwRASTERFORMATPAL4);

                    format |= rwRASTERFORMATPAL8;
                }

                if (format & rwRASTERFORMATPAL8)
                {
                    if (!_rwD3D9CheckValidTextureFormat(D3DFMT_P8) ||
                        (((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT8888) &&
                        ((_RwD3D9DeviceCaps.TextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE) == 0)))
                    {
                        /* Change it to a default format */
                        format &= (~rwRASTERFORMATPAL8);

                        if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT888)
                        {
                            if (_RwD3D9AdapterInformation.displayDepth > 16)
                            {
                                if (_rwD3D9CheckValidTextureFormat(D3DFMT_X8R8G8B8))
                                {
                                    format |= rwRASTERFORMAT888;
                                }
                                else
                                {
                                    format &= (~rwRASTERFORMAT888);

                                    format |= rwRASTERFORMAT565;
                                }
                            }
                            else
                            {
                                format &= (~rwRASTERFORMAT888);

                                format |= rwRASTERFORMAT565;
                            }
                        }
                        else
                        {
                            if (_RwD3D9AdapterInformation.displayDepth > 16)
                            {
                                if (_rwD3D9CheckValidTextureFormat(D3DFMT_A8R8G8B8))
                                {
                                    format |= rwRASTERFORMAT8888;
                                }
                                else
                                {
                                    format &= (~rwRASTERFORMAT8888);

                                    if (_rwD3D9CheckValidTextureFormat(D3DFMT_A4R4G4B4))
                                    {
                                        format |= rwRASTERFORMAT4444;
                                    }
                                    else
                                    {
                                        format |= rwRASTERFORMAT1555;
                                    }
                                }
                            }
                            else
                            {
                                format &= (~rwRASTERFORMAT8888);

                                if (_rwD3D9CheckValidTextureFormat(D3DFMT_A4R4G4B4))
                                {
                                    format |= rwRASTERFORMAT4444;
                                }
                                else
                                {
                                    format |= rwRASTERFORMAT1555;
                                }
                            }
                        }
                    }
                }
                else
                {
                    if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT8888)
                    {
                        if (!_rwD3D9CheckValidTextureFormat(D3DFMT_A8R8G8B8))
                        {
                            format &= (~rwRASTERFORMAT8888);

                            if (_rwD3D9CheckValidTextureFormat(D3DFMT_A4R4G4B4))
                            {
                                format |= rwRASTERFORMAT4444;
                            }
                            else
                            {
                                format |= rwRASTERFORMAT1555;
                            }
                        }
                    }
                    else if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT888)
                    {
                        if (!_rwD3D9CheckValidTextureFormat(D3DFMT_X8R8G8B8))
                        {
                            format &= (~rwRASTERFORMAT888);

                            format |= rwRASTERFORMAT565;
                        }
                    }
                    else if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT4444)
                    {
                        if (!_rwD3D9CheckValidTextureFormat(D3DFMT_A4R4G4B4))
                        {
                            format &= (~rwRASTERFORMAT4444);

                            format |= rwRASTERFORMAT1555;
                        }
                    }
                    else if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATLUM8)
                    {
                        if (!_rwD3D9CheckValidTextureFormat(D3DFMT_L8))
                        {
                            format &= (~rwRASTERFORMATLUM8);

                            if (_rwD3D9CheckValidTextureFormat(D3DFMT_X8R8G8B8))
                            {
                                format |= D3DFMT_X8R8G8B8;
                            }
                            else
                            {
                                format |= rwRASTERFORMAT565;
                            }
                        }
                    }
                }
            }
            break;

        case rwRASTERTYPECAMERATEXTURE:
            if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATDEFAULT)
            {
                if (16 == _RwD3D9ZBufferDepth)
                {
                    format |= rwRASTERFORMAT565;
                }
                else
                {
                    if (_rwD3D9CheckValidCameraTextureFormat(D3DFMT_X8R8G8B8))
                    {
                        format |= rwRASTERFORMAT888;
                    }
                    else
                    {
                        format |= rwRASTERFORMAT565;
                    }
                }
            }
            else
            {
                const RwInt32 depth = _rwRasterConvTable[FMT2TBL(format)].depth;

                if (16 == _RwD3D9ZBufferDepth)
                {
                    if (depth == 32)
                    {
                        if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT8888)
                        {
                            format &= (~rwRASTERFORMAT8888);

                            format |= rwRASTERFORMAT4444;
                        }
                        else if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT888)
                        {
                            format &= (~rwRASTERFORMAT888);

                            format |= rwRASTERFORMAT565;
                        }
                        else
                        {
                            format = rwRASTERFORMAT565;
                        }
                    }

                    if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT4444)
                    {
                        if (!_rwD3D9CheckValidCameraTextureFormat(D3DFMT_A4R4G4B4))
                        {
                            format &= (~rwRASTERFORMAT4444);

                            format |= rwRASTERFORMAT1555;
                        }
                    }

                    if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT1555)
                    {
                        if (!_rwD3D9CheckValidCameraTextureFormat(D3DFMT_A1R5G5B5))
                        {
                            format &= (~rwRASTERFORMAT1555);

                            format |= rwRASTERFORMAT565;
                        }
                    }
                }
                else
                {
                    if (depth == 16)
                    {
                        if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT4444)
                        {
                            format &= (~rwRASTERFORMAT4444);

                            format |= rwRASTERFORMAT8888;
                        }
                        else if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT1555)
                        {
                            format &= (~rwRASTERFORMAT1555);

                            format |= rwRASTERFORMAT8888;
                        }
                        else if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT565)
                        {
                            format &= (~rwRASTERFORMAT565);

                            format |= rwRASTERFORMAT888;
                        }
                        else
                        {
                            format = rwRASTERFORMAT888;
                        }
                    }

                    if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT8888)
                    {
                        if (!_rwD3D9CheckValidCameraTextureFormat(D3DFMT_A8R8G8B8))
                        {
                            format &= (~rwRASTERFORMAT8888);

                            format |= rwRASTERFORMAT888;
                        }
                    }
                }
            }
            break;

        case rwRASTERTYPECAMERA:
            /* Always force default */
            if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATDEFAULT)
            {
                format = rwD3D9FindRwFormat(Present.BackBufferFormat);
            }
            break;

        case rwRASTERTYPEZBUFFER:
            /* Always force default */
            if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATDEFAULT)
            {
                if (16 == _RwD3D9ZBufferDepth)
                {
                    format = rwRASTERFORMAT16;
                }
                else if (32 == _RwD3D9ZBufferDepth)
                {
                    format = rwRASTERFORMAT32;
                }
            }
            break;

        default:
            break;
        }

        RWRETURN(format);
    }

    RwBool RenderingEngine::D3D9CheckRasterFormat(void* rasterIn, RwInt32 flags)
    {
        RwRaster* raster = (RwRaster*)rasterIn;
        RwUInt32    format = flags & rwRASTERFORMATMASK;

        /* Copy over types */
        raster->cType = (RwUInt8)(flags & rwRASTERTYPEMASK);
        raster->cFlags = (RwUInt8)(flags & ~rwRASTERTYPEMASK);

        format = (RwRasterType) D3D9FindCorrectRasterFormat((RwRasterType)raster->cType, flags);

        raster->cFormat = (RwUInt8)(format >> 8);

        raster->depth = _rwRasterConvTable[FMT2TBL(format)].depth;

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::rwD3D9SetRasterFormat(void* rasterIn, RwInt32 flags)
    {
        RwRaster* raster = (RwRaster*)rasterIn;
        RwUInt32    format = flags & rwRASTERFORMATMASK;

        /* Copy over types */
        raster->cType = (RwUInt8)(flags & rwRASTERTYPEMASK);
        raster->cFlags = (RwUInt8)(flags & ~rwRASTERTYPEMASK);

        switch (raster->cType & rwRASTERTYPEMASK)
        {
        case rwRASTERTYPENORMAL:
        case rwRASTERTYPETEXTURE:
            if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATDEFAULT)
            {
                /* Check if we are requesting a default pixel format palette texture */
                if (format & rwRASTERFORMATPAL8)
                {
                    if (_rwD3D9CheckValidTextureFormat(D3DFMT_P8))
                    {
                        if (_RwD3D9DeviceCaps.TextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE)
                        {
                            format |= rwRASTERFORMAT8888;
                        }
                        else
                        {
                            format |= rwRASTERFORMAT888;
                        }

                        raster->depth = 8;
                    }
                    else
                    {
                        RWRETURN(FALSE);
                    }
                }
                else
                {
                    /*
                     * By default textures are 8888, but make mipmap generation flag and
                     * palletized flags persist
                     */
                    if (_RwD3D9AdapterInformation.displayDepth > 16)
                    {
                        if (_rwD3D9CheckValidTextureFormat(D3DFMT_A8R8G8B8))
                        {
                            format |= rwRASTERFORMAT8888;

                            raster->depth = 32;
                        }
                        else
                        {
                            if (_rwD3D9CheckValidTextureFormat(D3DFMT_A4R4G4B4))
                            {
                                format |= rwRASTERFORMAT4444;
                            }
                            else
                            {
                                format |= rwRASTERFORMAT1555;
                            }

                            raster->depth = 16;
                        }
                    }
                    else
                    {
                        if (_rwD3D9CheckValidTextureFormat(D3DFMT_A4R4G4B4))
                        {
                            format |= rwRASTERFORMAT4444;
                        }
                        else
                        {
                            format |= rwRASTERFORMAT1555;
                        }

                        raster->depth = 16;
                    }
                }
            }
            else
            {
                /* No support for 4 bits palettes */
                if (format & rwRASTERFORMATPAL4)
                {
                    RWRETURN(FALSE);
                }
                else
                {
                    if (format & rwRASTERFORMATPAL8)
                    {
                        if (_rwD3D9CheckValidTextureFormat(D3DFMT_P8) &&
                            (((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT888) ||
                            (((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT8888) &&
                                (_RwD3D9DeviceCaps.TextureCaps & D3DPTEXTURECAPS_ALPHAPALETTE))))
                        {
                            raster->depth = 8;
                        }
                        else
                        {
                            RWRETURN(FALSE);
                        }
                    }
                    else
                    {
                        D3DFORMAT d3dFormat;

                        d3dFormat = _rwRasterConvTable[FMT2TBL(format)].format;

                        if (IS_D3DFORMAT_ZBUFFER(d3dFormat))
                        {
                            if (_rwD3D9CheckValidZBufferTextureFormat(d3dFormat))
                            {
                                raster->depth = _rwRasterConvTable[FMT2TBL(format)].depth;
                            }
                            else
                            {
                                RWRETURN(FALSE);
                            }
                        }
                        else
                        {
                            if (_rwD3D9CheckValidTextureFormat(d3dFormat))
                            {
                                raster->depth = _rwRasterConvTable[FMT2TBL(format)].depth;
                            }
                            else
                            {
                                RWRETURN(FALSE);
                            }
                        }
                    }
                }
            }

            /* check automipmapgen */
            if ((format & rwRASTERFORMATPAL8) == 0 &&
                (flags & (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP)) == (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP))
            {
                if (_rwD3D9CheckAutoMipmapGenTextureFormat(_rwRasterConvTable[FMT2TBL(format)].format))
                {
                    _rwD3D9RasterExt* rasExt = RASTEREXTFROMRASTER(raster);

                    rasExt->automipmapgen = 1;
                }
            }
            break;

        case rwRASTERTYPECAMERATEXTURE:
            if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATDEFAULT)
            {
                /*
                 * Raster should match back buffer bit depth
                 */
                if (16 == _RwD3D9AdapterInformation.displayDepth)
                {
                    format |= rwRASTERFORMAT565;

                    raster->depth = 16;
                }
                else
                {
                    if (_rwD3D9CheckValidCameraTextureFormat(D3DFMT_X8R8G8B8))
                    {
                        format |= rwRASTERFORMAT888;

                        raster->depth = 32;
                    }
                    else
                    {
                        format |= rwRASTERFORMAT565;

                        raster->depth = 16;
                    }
                }
            }
            else
            {
                RwInt32 depth;

                depth = _rwRasterConvTable[FMT2TBL(format)].depth;

                if (_rwD3D9CheckValidCameraTextureFormat(_rwRasterConvTable[FMT2TBL(format)].format))
                {
                    raster->depth = depth;
                }
                else
                {
                    RWRETURN(FALSE);
                }
            }

            /* check automipmapgen */
            if ((flags & (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP)) == (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP))
            {
                if (_rwD3D9CheckAutoMipmapGenCameraTextureFormat(_rwRasterConvTable[FMT2TBL(format)].format))
                {
                    _rwD3D9RasterExt* rasExt = RASTEREXTFROMRASTER(raster);

                    rasExt->automipmapgen = 1;
                }
            }
            break;

        case rwRASTERTYPECAMERA:
            /* Always force default */
            if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATDEFAULT)
            {
                format = rwD3D9FindRwFormat(Present.BackBufferFormat);
            }
            else
            {
                const RwUInt32 mainCameraFormat = rwD3D9FindRwFormat(Present.BackBufferFormat);

                if (mainCameraFormat != format)
                {
                    RWRETURN(FALSE);
                }
            }

            raster->depth = _RwD3D9AdapterInformation.displayDepth;
            break;

        case rwRASTERTYPEZBUFFER:
            /* Always force default */
            if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMATDEFAULT)
            {
                if (16 == _RwD3D9ZBufferDepth)
                {
                    raster->depth = 16;
                    format = rwRASTERFORMAT16;
                }
                else if (32 == _RwD3D9ZBufferDepth)
                {
                    raster->depth = 32;
                    format = rwRASTERFORMAT32;
                }
            }
            else
            {
                if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT16 && _RwD3D9ZBufferDepth == 16)
                {
                    raster->depth = 16;
                }
                else if ((format & rwRASTERFORMATPIXELFORMATMASK) == rwRASTERFORMAT32 && _RwD3D9ZBufferDepth == 32)
                {
                    raster->depth = 32;
                }
                else
                {
                    RWRETURN(FALSE);
                }
            }
            break;

        default:
            RWRETURN(FALSE);
            break;
        }

        raster->cFormat = (RwUInt8)(format >> 8);

        RWRETURN(TRUE);
    }

     RwUInt32 RenderingEngine::rwD3D9FindRwFormat(D3DFORMAT d3dFormat)
    {
        RWRETURN(_rwD3D9PixelFormatInfo[d3dFormat].rwFormat);
    }

    RwBool RenderingEngine::_rwD3D9CheckValidTextureFormat(RwInt32 format)
    {
        const D3DFORMAT  adapterFormat = _RwD3D9AdapterInformation.mode.Format;
        HRESULT             hr;

        hr = IDirect3D9_CheckDeviceFormat(_RwDirect3DObject, _RwD3DAdapterIndex, _RwD3DAdapterType,
            adapterFormat, 0,D3DRTYPE_TEXTURE,(D3DFORMAT)format);

        RWRETURN(SUCCEEDED(hr));
    }

    RwBool RenderingEngine::_rwD3D9CheckValidCameraTextureFormat(RwInt32 format)
    {
        HRESULT             hr;
        const D3DFORMAT  adapterFormat = _RwD3D9AdapterInformation.mode.Format;

        hr = IDirect3D9_CheckDeviceFormat(_RwDirect3DObject, _RwD3DAdapterIndex, _RwD3DAdapterType,
            adapterFormat, D3DUSAGE_RENDERTARGET,   D3DRTYPE_TEXTURE,  (D3DFORMAT)format);

        if (SUCCEEDED(hr))
        {
            hr = IDirect3D9_CheckDepthStencilMatch(_RwDirect3DObject,   _RwD3DAdapterIndex, _RwD3DAdapterType,
                adapterFormat,  (D3DFORMAT)format, Present.AutoDepthStencilFormat);
        }

        RWRETURN(SUCCEEDED(hr));
    }

    RwBool RenderingEngine::_rwD3D9CheckValidZBufferTextureFormat(RwInt32 format)
    {
        HRESULT             hr;
        const D3DFORMAT  adapterFormat = _RwD3D9AdapterInformation.mode.Format;


        hr = IDirect3D9_CheckDeviceFormat(_RwDirect3DObject, _RwD3DAdapterIndex,  _RwD3DAdapterType,
            adapterFormat,  D3DUSAGE_DEPTHSTENCIL,  D3DRTYPE_TEXTURE,  (D3DFORMAT)format);
      
        if (SUCCEEDED(hr))
        {

            hr = IDirect3D9_CheckDepthStencilMatch(_RwDirect3DObject, _RwD3DAdapterIndex,_RwD3DAdapterType,
                adapterFormat,   adapterFormat,  (D3DFORMAT)format);
        }

        RWRETURN(SUCCEEDED(hr));
    }

    RwBool RenderingEngine::_rwD3D9CheckValidCubeTextureFormat(RwInt32 format)
    {
        const D3DFORMAT adapterFormat = _RwD3D9AdapterInformation.mode.Format;
        HRESULT         hr;

        hr = IDirect3D9_CheckDeviceFormat(_RwDirect3DObject, _RwD3DAdapterIndex, _RwD3DAdapterType,
            adapterFormat, 0,  D3DRTYPE_CUBETEXTURE, (D3DFORMAT)format);

        RWRETURN(SUCCEEDED(hr));
    }

    RwBool RenderingEngine::_rwD3D9CheckValidCameraCubeTextureFormat(RwInt32 format)
    {
        HRESULT             hr;
        const D3DFORMAT  adapterFormat = _RwD3D9AdapterInformation.mode.Format;

        hr = IDirect3D9_CheckDeviceFormat(_RwDirect3DObject,  _RwD3DAdapterIndex,_RwD3DAdapterType,
            adapterFormat,  D3DUSAGE_RENDERTARGET,    D3DRTYPE_CUBETEXTURE, (D3DFORMAT)format);

        if (SUCCEEDED(hr))
        {
            hr = IDirect3D9_CheckDepthStencilMatch(_RwDirect3DObject, _RwD3DAdapterIndex, _RwD3DAdapterType,
                adapterFormat, (D3DFORMAT)format, Present.AutoDepthStencilFormat);
        }

        RWRETURN(SUCCEEDED(hr));
    }

    RwBool RenderingEngine::_rwD3D9CheckAutoMipmapGenCubeTextureFormat(RwInt32 format)
    {
        if (_RwD3D9DeviceCaps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP)
        {
            const D3DFORMAT adapterFormat = _RwD3D9AdapterInformation.mode.Format;
            HRESULT         hr;

            hr = IDirect3D9_CheckDeviceFormat(_RwDirect3DObject, _RwD3DAdapterIndex, _RwD3DAdapterType,
                adapterFormat,  D3DUSAGE_AUTOGENMIPMAP,D3DRTYPE_CUBETEXTURE, (D3DFORMAT)format);

            RWRETURN(hr == D3D_OK);
        }
        else
        {
            RWRETURN(FALSE);
        }
    }

    RwBool RenderingEngine::_rwD3D9CheckAutoMipmapGenCameraTextureFormat(RwInt32 format)
    {
        if (_RwD3D9DeviceCaps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP)
        {
            const D3DFORMAT  adapterFormat = _RwD3D9AdapterInformation.mode.Format;
            HRESULT             hr;

            hr = IDirect3D9_CheckDeviceFormat(_RwDirect3DObject, _RwD3DAdapterIndex, _RwD3DAdapterType,
                adapterFormat,
                D3DUSAGE_AUTOGENMIPMAP | D3DUSAGE_RENDERTARGET,  D3DRTYPE_TEXTURE,  (D3DFORMAT)format);

            RWRETURN(hr == D3D_OK);
        }
        else
        {
            RWRETURN(FALSE);
        }
    }

    RwBool RenderingEngine::_rwD3D9CheckValidZBufferFormat(RwInt32 format)
    {
        HRESULT             hr;
        const D3DFORMAT  adapterFormat = _RwD3D9AdapterInformation.mode.Format;


        hr = IDirect3D9_CheckDeviceFormat(_RwDirect3DObject, _RwD3DAdapterIndex,_RwD3DAdapterType,
            adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, (D3DFORMAT)format);
        
        if (SUCCEEDED(hr))
        {

            hr = IDirect3D9_CheckDepthStencilMatch(_RwDirect3DObject,_RwD3DAdapterIndex, _RwD3DAdapterType,
                adapterFormat, adapterFormat, (D3DFORMAT)format);
        }

        RWRETURN(SUCCEEDED(hr));
    }


    RwBool  RenderingEngine::_rwD3D9CheckAutoMipmapGenTextureFormat(RwInt32 format)
    {

        if (_RwD3D9DeviceCaps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP)
        {
            const D3DFORMAT  adapterFormat = _RwD3D9AdapterInformation.mode.Format;
            HRESULT             hr;

            hr = IDirect3D9_CheckDeviceFormat(_RwDirect3DObject,_RwD3DAdapterIndex,  _RwD3DAdapterType,
                adapterFormat, D3DUSAGE_AUTOGENMIPMAP,  D3DRTYPE_TEXTURE, (D3DFORMAT)format);
            RWRETURN(hr == D3D_OK);
        }
        else
        {
            RWRETURN(FALSE);
        }
    }

    RwBool RenderingEngine::_rwD3D9RasterHasAlpha(const RwRaster* raster)
    {
        const _rwD3D9RasterExt* rasExt;

        rasExt = RASTEREXTFROMCONSTRASTER(raster);

        RWRETURN(rasExt->alpha);
    }

    RwBool RenderingEngine::_rwD3D9TextureHasAlpha(const RwTexture* texture)
    {
        if (texture->raster != NULL)
        {
            const _rwD3D9RasterExt* rasExt;

            rasExt = RASTEREXTFROMRASTER(texture->raster);

            RWRETURN(rasExt->alpha);
        }
        else
        {
            RWRETURN(FALSE);
        }
    }
    RwBool RenderingEngine::_rwD3D9NativeTextureRead(void* streamIn, void* textureIn, RwInt32 unused3 __RWUNUSED__)
    {
        RwStream* stream = (RwStream*)streamIn;
        RwRaster* raster = (RwRaster*)NULL;
        RwTexture* texture;
        RwUInt32            length, version;
        _rwD3D9NativeRaster   nativeRaster;
        _rwD3D9NativeTexture  nativeTexture;
        _rwD3D9RasterExt* rasExt;

        if (!RwStreamFindChunk(stream, rwID_STRUCT, &length, &version))
        {
            RWRETURN(FALSE);
        }

        if (!((version >= rwLIBRARYBASEVERSION) &&
            (version <= rwLIBRARYCURRENTVERSION)))
        {
            RWRETURN(FALSE);
        }

        /*
         * Don't have to worry about endianness as this is platform specific
         */

         /* ID, filter & addressing modes */
        if (RwStreamRead(stream, (void*)&nativeTexture,
            sizeof(_rwD3D9NativeTexture)) != sizeof(_rwD3D9NativeTexture))
        {
            RWRETURN(FALSE);
        }

        if (rwID_PCD3D9 != nativeTexture.id)
        {
            RWRETURN(FALSE);
        }

        /* Native raster data structure */
        if (RwStreamRead(stream, (void*)&nativeRaster,
            sizeof(_rwD3D9NativeRaster)) != sizeof(_rwD3D9NativeRaster))
        {
            RWRETURN(FALSE);
        }

        if (nativeRaster.flags & IS_COMPRESSED)
        {
            RwUInt32 dxtFormat;

            /* Get compressed format */
            dxtFormat = nativeRaster.d3dFormat;

            if (_rwD3D9CheckValidTextureFormat(dxtFormat))
            {
                RwUInt32 levels;

                /* Create a raster */
                raster = RwRasterCreate(nativeRaster.width, nativeRaster.height,
                    (RwInt32)nativeRaster.depth, nativeRaster.type | nativeRaster.format | rwRASTERDONTALLOCATE);

                if (NULL == raster)
                {
                    RWRETURN(FALSE);
                }

                /* Get the raster extension */
                rasExt = RASTEREXTFROMRASTER(raster);

                levels = ((RwRasterGetFormat(raster) & rwRASTERFORMATMIPMAP) ?
                    nativeRaster.numMipLevels : 1);

                if ((nativeRaster.flags & IS_CUBE) != 0)
                {
                    if ((_RwD3D9DeviceCaps.CubeTextureFilterCaps & (D3DPTFILTERCAPS_MIPFPOINT | D3DPTFILTERCAPS_MIPFLINEAR)) == 0)
                    {
                        if (levels > 1)
                        {
                            RwRasterDestroy(raster);

                            RWRETURN(FALSE);
                        }
                    }

                    rasExt->cube = TRUE;

                    /* check automipmapgen */
                    if ((nativeRaster.format & (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP)) == (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP))
                    {
                        if (_rwD3D9CheckAutoMipmapGenCubeTextureFormat(dxtFormat))
                        {
                            rasExt->automipmapgen = 1;
                        }
                    }

                    if (FAILED(IDirect3DDevice9_CreateCubeTexture(_RwD3DDevice, raster->width,
                        levels,
                        (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0),
                        (D3DFORMAT)dxtFormat, D3DPOOL_MANAGED, (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture),
                        NULL)))
                    {
                        RwRasterDestroy(raster);

                        RWRETURN(FALSE);
                    }
                    else
                    {
                        /* Remove the rwRASTERDONTALLOCATE flag so it will get destroyed */
                        raster->cFlags &= ~rwRASTERDONTALLOCATE;

                        /* Copy alpha flag */
                        rasExt->alpha = (nativeRaster.flags & HAS_ALPHA) != 0;

                        /* Mark as cube texture */
                        rasExt->cube = TRUE;

                        /* Mark as compressed texture */
                        rasExt->compressed = TRUE;

                        rasExt->d3dFormat = (D3DFORMAT)dxtFormat;
                    }
                }
                else
                {
                    /* check automipmapgen */
                    if ((nativeRaster.format & (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP)) == (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP))
                    {
                        if (_rwD3D9CheckAutoMipmapGenTextureFormat(dxtFormat))
                        {
                            rasExt->automipmapgen = 1;
                        }
                    }

                    /* Create the texture, if mipped 0 gets all the mip levels */
                    if (FAILED(IDirect3DDevice9_CreateTexture(_RwD3DDevice, raster->width, raster->height,
                        levels, (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0),
                        (D3DFORMAT)dxtFormat, D3DPOOL_MANAGED, &(rasExt->texture), NULL)))
                    {
                        RwRasterDestroy(raster);

                        RWRETURN(FALSE);
                    }
                    else
                    {
                        /* Remove the rwRASTERDONTALLOCATE flag so it will get destroyed */
                        raster->cFlags &= ~rwRASTERDONTALLOCATE;

                        /* Copy alpha flag */
                        rasExt->alpha = (nativeRaster.flags & HAS_ALPHA) != 0;

                        /* Mark as compressed texture */
                        rasExt->compressed = TRUE;

                        rasExt->d3dFormat = (D3DFORMAT)dxtFormat;
                    }
                }
            }
            else
            {
                RWRETURN(FALSE);
            }
        }
        else if ((nativeRaster.flags & IS_CUBE) != 0)
        {
            RwUInt32 levels;

            /* Create a raster */
            raster = RwRasterCreate(nativeRaster.width, nativeRaster.height,
                (RwInt32)nativeRaster.depth, nativeRaster.type | nativeRaster.format | rwRASTERDONTALLOCATE);

            if (NULL == raster)
            {
                RWRETURN(FALSE);
            }

            /* Get the raster extension */
            rasExt = RASTEREXTFROMRASTER(raster);

            levels = ((RwRasterGetFormat(raster) & rwRASTERFORMATMIPMAP) ?
                nativeRaster.numMipLevels : 1);

            if ((_RwD3D9DeviceCaps.CubeTextureFilterCaps & (D3DPTFILTERCAPS_MIPFPOINT | D3DPTFILTERCAPS_MIPFLINEAR)) == 0)
            {
                if (levels > 1)
                {
                    RwRasterDestroy(raster);

                    RWRETURN(FALSE);
                }
            }

            /* Mark as cube texture */
            rasExt->cube = TRUE;

            /* check automipmapgen */
            if ((nativeRaster.format & (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP)) == (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP))
            {
                if (_rwD3D9CheckAutoMipmapGenCubeTextureFormat(nativeRaster.d3dFormat))
                {
                    rasExt->automipmapgen = 1;
                }
            }

            if (FAILED(IDirect3DDevice9_CreateCubeTexture(_RwD3DDevice, raster->width, levels,
                (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0),
                nativeRaster.d3dFormat, D3DPOOL_MANAGED, (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture),
                NULL)))
            {
                RwRasterDestroy(raster);

                RWRETURN(FALSE);
            }
            else
            {
                /* Remove the rwRASTERDONTALLOCATE flag so it will get destroyed */
                raster->cFlags &= ~rwRASTERDONTALLOCATE;

                /* Copy alpha flag */
                rasExt->alpha = (nativeRaster.flags & HAS_ALPHA) != 0;
            }
        }
        else if (nativeRaster.format & ~(rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP)) /* Is an standard RW format? */
        {
            /* Create a raster */
            raster = RwRasterCreate(nativeRaster.width, nativeRaster.height,
                (RwInt32)nativeRaster.depth, nativeRaster.type | nativeRaster.format);
            if (!raster)
            {
                RWRETURN(FALSE);
            }

            /* Get the raster extension */
            rasExt = RASTEREXTFROMRASTER(raster);

            rasExt->compressed = 0;
        }
        else
        {
            /* Create a raster */
            raster = RwD3D9RasterCreate(nativeRaster.width, nativeRaster.height,
                nativeRaster.d3dFormat,
                nativeRaster.type |  (nativeRaster.format & (rwRASTERFORMATAUTOMIPMAP | rwRASTERFORMATMIPMAP)));
            if (!raster)
            {
                RWRETURN(FALSE);
            }

            /* Get the raster extension */
            rasExt = RASTEREXTFROMRASTER(raster);
        }

        /* check automipmap support */
        if (nativeRaster.flags & USE_AUTOMIPMAPGEN)
        {
            if (rasExt->automipmapgen == 0)
            {
                RwRasterDestroy(raster);

                RWRETURN(FALSE);
            }
        }

        /* Check format and size */
        if (nativeRaster.format == (RwUInt32)RwRasterGetFormat(raster) &&
            nativeRaster.d3dFormat == rasExt->d3dFormat &&
            nativeRaster.width == raster->width &&
            nativeRaster.height == raster->height)
        {
            RwUInt32    autoMipmap, face, numMipLevels, i;

            /* Load the palette if palletized */
            if (nativeRaster.format & rwRASTERFORMATPAL4)
            {
                RwUInt8* palette;
                RwUInt32    size;

                palette = RwRasterLockPalette(raster, rwRASTERLOCKWRITE);

                size = sizeof(PALETTEENTRY) * 32;
                if (RwStreamRead(stream, (void*)palette, size) != size)
                {
                    RwRasterUnlockPalette(raster);

                    RwRasterDestroy(raster);

                    RWRETURN(FALSE);
                }

                RwRasterUnlockPalette(raster);
            }
            else if (nativeRaster.format & rwRASTERFORMATPAL8)
            {
                RwUInt8* palette;
                RwUInt32    size;

                palette = RwRasterLockPalette(raster, rwRASTERLOCKWRITE);

                size = sizeof(PALETTEENTRY) * 256;
                if (RwStreamRead(stream, (void*)palette, size) != size)
                {
                    RwRasterUnlockPalette(raster);

                    RwRasterDestroy(raster);

                    RWRETURN(FALSE);
                }

                RwRasterUnlockPalette(raster);
            }

            /* Remove AUTOMIPMAP flag to avoid unlock invoking mipmap create */
            autoMipmap = raster->cFormat & (rwRASTERFORMATAUTOMIPMAP >> 8);
            raster->cFormat &= ~autoMipmap;

            /* Prepare support for cube textures */
            if (rasExt->cube)
            {
                face = 6;
            }
            else
            {
                face = 1;
            }

            if (rasExt->automipmapgen)
            {
                numMipLevels = 1; /* Read only first level */
            }
            else
            {
                numMipLevels = nativeRaster.numMipLevels;
            }

            rasExt->face = 0;

            /* Load mips into raster */
            do
            {
                for (i = 0; i < numMipLevels; i++)
                {
                    RwUInt8* pixels;
                    RwUInt32    size;

                    pixels = RwRasterLock(raster, (RwUInt8)i, rwRASTERLOCKWRITE);

                    /* Size in bytes of mip to load */
                    if (RwStreamRead(stream, (void*)&size, sizeof(RwUInt32)) != sizeof(RwUInt32))
                    {
                        RwRasterUnlock(raster);
                        RwRasterDestroy(raster);

                        RWRETURN(FALSE);
                    }

                    if (size == 0 && raster != 0)
                    {
                        auto rasterExt = RASTEREXTFROMRASTER(raster);

                        // simply copy the data from previous level
                        if (raster->cType == rwRASTERTYPETEXTURE && rasterExt->compressed && rasterExt->texture)
                        {
                            D3DLOCKED_RECT rect;
                            rasterExt->texture->LockRect(i - 1, &rect, NULL, D3DLOCK_READONLY);

                            RwUInt32 sz = 16;
                            if (rasterExt->d3dFormat == D3DFMT_DXT1)
                                sz = 8;

                            memcpy(pixels, rect.pBits, sz);

                            rasterExt->texture->UnlockRect(i - 1);
                        }
                    }

                    /* Read the mip level */
                    if (RwStreamRead(stream, (void*)pixels, size) != size)
                    {
                        RwRasterUnlock(raster);
                        RwRasterDestroy(raster);

                        RWRETURN(FALSE);
                    }

                    RwRasterUnlock(raster);
                }

                if (rasExt->cube)
                {
                    rasExt->face += 1;
                }
            } while (--face);

            /* Restore auto mip flag */
            raster->cFormat |= autoMipmap;
        }
        else
        {
            RwRasterDestroy(raster);

            RWRETURN(FALSE);
        }

        texture = RwTextureCreate(raster);
        if (!texture)
        {
            RwRasterDestroy(raster);

            RWRETURN(FALSE);
        }

        RwTextureSetFilterMode(texture, nativeTexture.filterAndAddress & 0xFF);
        RwTextureSetAddressingU(texture, (nativeTexture.filterAndAddress >> 8) & 0x0F);
        RwTextureSetAddressingV(texture, (nativeTexture.filterAndAddress >> 12) & 0x0F);
        RwTextureSetName(texture, nativeTexture.name);
        RwTextureSetMaskName(texture, nativeTexture.mask);

        *((RwTexture**)textureIn) = texture;

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::_rwD3D9CubeRasterCreate(RwRaster* raster, RwUInt32 d3dformat, RwInt32 mipmapLevels)
    {
        _rwD3D9RasterExt* rasExt;
        RwUInt32 levels;
        HRESULT hr;

        rasExt = RASTEREXTFROMRASTER(raster);

        rasExt->cube = TRUE;

        rasExt->d3dFormat = (D3DFORMAT)d3dformat;

        if (_RwD3D9DeviceCaps.CubeTextureFilterCaps & (D3DPTFILTERCAPS_MIPFPOINT | D3DPTFILTERCAPS_MIPFLINEAR))
        {
            levels = mipmapLevels;
        }
        else
        {
            levels = 1;
        }

        if ((raster->cType & rwRASTERTYPEMASK) != rwRASTERTYPECAMERATEXTURE)
        {
            hr = 
                IDirect3DDevice9_CreateCubeTexture(_RwD3DDevice,
                    raster->width,
                    levels,
                    (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0),
                    rasExt->d3dFormat,
                    D3DPOOL_MANAGED,
                    (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture),
                    NULL);
        }
        else
        {
            hr = 
                IDirect3DDevice9_CreateCubeTexture(_RwD3DDevice,
                    raster->width,
                    levels,
                    (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0) |
                    D3DUSAGE_RENDERTARGET,
                    rasExt->d3dFormat,
                    D3DPOOL_DEFAULT,
                    (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture),
                    NULL);

            if (SUCCEEDED(hr))
            {
                rxD3D9VideoMemoryRasterListAdd(raster);
            }
        }

        if (FAILED(hr))
        {
            RWRETURN(FALSE);
        }

        /* Remove the rwRASTERDONTALLOCATE flag so it will get destroyed */
        raster->cFlags &= ~rwRASTERDONTALLOCATE;

        RWRETURN(TRUE);
    }

    void  RenderingEngine::_rwD3D9CubeRasterSelectFace(RwRaster* raster, RwUInt32 face)
    {
        _rwD3D9RasterExt* rasExt;

        rasExt = RASTEREXTFROMRASTER(raster);
        rasExt->face = face;

        RWRETURNVOID();
    }

    RwBool RenderingEngine::_rwD3D9RasterIsCubeRaster(const RwRaster* raster)
    {
        const _rwD3D9RasterExt* rasExt;

        rasExt = RASTEREXTFROMCONSTRASTER(raster);

        RWRETURN(rasExt->cube);
    }

    void  RenderingEngine::_rwD3D9CheckRasterSize(RwInt32* width, RwInt32* height, RwUInt32 rasterFlags)
    {

        /* Clamp the raster to the max texture size */
        if (*width > (RwInt32)_RwD3D9DeviceCaps.MaxTextureWidth)
        {
            *width = _RwD3D9DeviceCaps.MaxTextureWidth;
        }

        if (*height > (RwInt32)_RwD3D9DeviceCaps.MaxTextureHeight)
        {
            *height = _RwD3D9DeviceCaps.MaxTextureHeight;
        }

        if ((rasterFlags & rwRASTERTYPEMASK) == rwRASTERTYPENORMAL ||
            (rasterFlags & rwRASTERFORMATMIPMAP) == 0)
        {
            if ((_RwD3D9DeviceCaps.TextureCaps & D3DPTEXTURECAPS_POW2) != 0 &&
                (_RwD3D9DeviceCaps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) == 0)
            {
                /* Make it a power of two (round down) */
                *width = 1 << _rwD3D9FindMSB(*width);
                *height = 1 << _rwD3D9FindMSB(*height);
            }
        }
        else
        {
            if (_RwD3D9DeviceCaps.TextureCaps & D3DPTEXTURECAPS_POW2)
            {
                /* Make it a power of two (round down) */
                *width = 1 << _rwD3D9FindMSB(*width);
                *height = 1 << _rwD3D9FindMSB(*height);
            }
        }

        if (_RwD3D9DeviceCaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
        {
            /* Make it square of smallest dimension */
            if (*width < *height)
            {
                *height = *width;
            }
            else
            {
                *width = *height;
            }
        }

        RWRETURNVOID();
    }

     RwBool  RenderingEngine::rwD3D9CreateTextureRaster(RwRaster* raster, _rwD3D9RasterExt* rasExt)
    {
        RwUInt32    rasFormat;
        RwUInt32    pixelFormat;
        HRESULT     hr;


        rasFormat = RwRasterGetFormat(raster);
        pixelFormat = rasFormat & rwRASTERFORMATPIXELFORMATMASK;

        if ((rasFormat & rwRASTERFORMATPAL4) || (rasFormat & rwRASTERFORMATPAL8))
        {
            if (!((rwRASTERFORMAT888 == pixelFormat) ||
                (rwRASTERFORMAT8888 == pixelFormat)))
            {
                RWRETURN(FALSE);
            }

            /* Create the palette */
            if (PaletteFreeList == NULL)
            {
                PaletteFreeList = RwFreeListCreate(sizeof(_rwD3D9Palette), 64, 4,
                    rwID_DRIVERMODULE | rwMEMHINTDUR_EVENT);
            }

            rasExt->palette = (_rwD3D9Palette*)RwFreeListAlloc(PaletteFreeList,
                rwID_DRIVERMODULE | rwMEMHINTDUR_EVENT);

            if (rasExt->palette)
            {
                rasExt->palette->globalindex = FindAvailablePaletteIndex();
            }

            rasExt->d3dFormat = D3DFMT_P8;
            Log::Debug("RenderingEngine::rwD3D9CreateTextureRaster - texture has palette");
        }
        else
        {
            rasExt->d3dFormat = _rwRasterConvTable[FMT2TBL(rasFormat)].format;
        }

        /* Does this raster format have an alpha component */
        rasExt->alpha = _rwRasterConvTable[FMT2TBL(rasFormat)].alpha;

        /* Check the size of the texture */
        _rwD3D9CheckRasterSize(&(raster->width), &(raster->height), raster->cType | rasFormat);

        /* Create the texture, if mipped 0 gets all the mip levels */
        if (rasExt->cube)
        {
            hr = IDirect3DDevice9_CreateCubeTexture(_RwD3DDevice, raster->width,
                (rasFormat & rwRASTERFORMATMIPMAP) ? 0 : 1,
                (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0),
                rasExt->d3dFormat,
                D3DPOOL_MANAGED,
                (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture),
                NULL);
        }
        else
        {
            if (IS_D3DFORMAT_ZBUFFER(rasExt->d3dFormat))
            {
                hr = IDirect3DDevice9_CreateTexture(_RwD3DDevice, raster->width,
                    raster->height,
                    1,
                    D3DUSAGE_DEPTHSTENCIL,
                    rasExt->d3dFormat,
                    D3DPOOL_DEFAULT,
                    &(rasExt->texture),
                    NULL);

                if (SUCCEEDED(hr))
                {
                    rxD3D9VideoMemoryRasterListAdd(raster);
                }
            }
            else
            {
                hr = IDirect3DDevice9_CreateTexture(_RwD3DDevice, raster->width,
                    raster->height,
                    (rasFormat & rwRASTERFORMATMIPMAP) ? 0 : 1,
                    (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0),
                    rasExt->d3dFormat,
                    D3DPOOL_MANAGED,
                    &(rasExt->texture),
                    NULL);
            }
        }

        if (FAILED(hr))
        {
            if (D3DFMT_P8 == rasExt->d3dFormat)
            {
                if (rasExt->palette)
                {
                    AddAvailablePaletteIndex(rasExt->palette->globalindex);

                    RwFreeListFree(PaletteFreeList, rasExt->palette);
                    rasExt->palette = NULL;
                }
            }

            RWRETURN(FALSE);
        }

        if (rasExt->palette != NULL)
        {
            rxD3D9VideoMemoryRasterListAdd(raster);
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::_rwD3D9RasterCreate(void* unused1 __RWUNUSED__, void* rasterIn, RwInt32 flags)
    {
        RwRaster* raster = (RwRaster*)rasterIn;
        _rwD3D9RasterExt* rasExt = RASTEREXTFROMRASTER(raster);

        /* Initialise structure to something sensible */
        raster->cType = (RwUInt8)(flags & rwRASTERTYPEMASK);
        raster->cFlags = (RwUInt8)(flags & ~rwRASTERTYPEMASK);
        raster->cpPixels = NULL;
        raster->palette = NULL;

        rasExt->texture = NULL;
        rasExt->palette = NULL;
        rasExt->alpha = 0;
        rasExt->cube = 0;
        rasExt->automipmapgen = 0;
        rasExt->compressed = 0;
        rasExt->face = 0;
        rasExt->lockedMipLevel = (RwUInt8)-1;
        rasExt->lockedSurface = NULL;
        rasExt->d3dFormat = D3DFMT_UNKNOWN;

        /* Set up the raster format and depth */
        if (!rwD3D9SetRasterFormat(raster, flags))
        {
            RWRETURN(FALSE);
        }

        /*
         * If it is not a camera buffer, then we need to allocate real memory for it
         */
        if ((raster->width) && (raster->height))
        {
            switch (raster->cType & rwRASTERTYPEMASK)
            {
            case rwRASTERTYPENORMAL:
            {
                if (!(raster->cFlags & rwRASTERDONTALLOCATE))
                {
                    if (!rwD3D9CreateTextureRaster(raster, rasExt))
                    {
                        RWRETURN(FALSE);
                    }
                }
            }
            break;

            case rwRASTERTYPETEXTURE:
            {
                if (!(raster->cFlags & rwRASTERDONTALLOCATE))
                {

                    if (!rwD3D9CreateTextureRaster(raster, rasExt))
                    {
                        RWRETURN(FALSE);
                    }
                }
            }
            break;

            case rwRASTERTYPECAMERATEXTURE:
            {
                if (!(raster->cFlags & rwRASTERDONTALLOCATE))
                {


                    if (!rwD3D9CreateCameraTextureRaster(raster, rasExt))
                    {
                        RWRETURN(FALSE);
                    }

                    rxD3D9VideoMemoryRasterListAdd(raster);
                }
            }
            break;

            case rwRASTERTYPEZBUFFER:
            {
                if (!(raster->cFlags & rwRASTERDONTALLOCATE))
                {

                    if (!rwD3D9CreateZBufferRaster(raster, rasExt))
                    {
                        RWRETURN(FALSE);
                    }

                    rxD3D9VideoMemoryRasterListAdd(raster);
                }
            }
            break;

            case rwRASTERTYPECAMERA:
            {
                RECT    rect;

                GetClientRect(*WindowHandle, &rect);

                if ((raster->cFlags & rwRASTERDONTALLOCATE) != 0 ||
                    (rect.right >= raster->width && rect.bottom >= raster->height))
                {

                    /* Find the depth of the backbuffer */
                    if (Present.BackBufferFormat == D3DFMT_A8R8G8B8 ||
                        Present.BackBufferFormat == D3DFMT_X8R8G8B8 ||
                        Present.BackBufferFormat == D3DFMT_A2R10G10B10)
                    {
                        raster->depth = 32;
                    }
                    else if (Present.BackBufferFormat == D3DFMT_R5G6B5 ||
                        Present.BackBufferFormat == D3DFMT_X1R5G5B5 ||
                        Present.BackBufferFormat == D3DFMT_A1R5G5B5)
                    {
                        raster->depth = 16;
                    }
                    else
                    {
                        raster->depth = 0;
                    }

                    /* Very little to do here - the camera is global */
                    raster->stride = 0;
                    raster->cpPixels = NULL;
                    raster->cFlags = (RwUInt8)rwRASTERDONTALLOCATE;

                    raster->originalWidth = raster->width;
                    raster->originalHeight = raster->height;

                    rasExt->d3dFormat = Present.BackBufferFormat;

                    rxD3D9VideoMemoryRasterListAdd(raster);

                    RWRETURN(TRUE);
                }
                else
                {
                    RWRETURN(FALSE);
                }
            }
            break;

            }
        }
        else
        {
            raster->cFlags = rwRASTERDONTALLOCATE;   /* Not allocated */

            raster->stride = 0;
            if (raster->depth == 0)
            {
                raster->depth = 16;
            }

            /* Camera Sub rasters need to be added */
            if (raster->cType == rwRASTERTYPECAMERA)
            {
                rxD3D9VideoMemoryRasterListAdd(raster);
            }

            RWRETURN(TRUE);
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::rwD3D9CreateZBufferRaster(RwRaster* raster, _rwD3D9RasterExt* rasExt)
    {
        RECT        rect;

        /* Find the Z-Buffer depth */
        if (16 == _RwD3D9ZBufferDepth)
        {
            if (_rwD3D9CheckValidZBufferFormat(D3DFMT_D15S1))
            {
                rasExt->d3dFormat = D3DFMT_D15S1;
            }
            else
            {
                rasExt->d3dFormat = D3DFMT_D16;
            }
        }
        else if (32 == _RwD3D9ZBufferDepth)
        {
            if (_rwD3D9CheckValidZBufferFormat(D3DFMT_D24S8))
            {
                rasExt->d3dFormat = D3DFMT_D24S8;
            }
            else if (_rwD3D9CheckValidZBufferFormat(D3DFMT_D24X4S4))
            {
                rasExt->d3dFormat = D3DFMT_D24X4S4;
            }
            else if (_rwD3D9CheckValidZBufferFormat(D3DFMT_D32))
            {
                rasExt->d3dFormat = D3DFMT_D32;
            }
            else
            {
                rasExt->d3dFormat = D3DFMT_D24X8;
            }
        }

        rasExt->alpha = 0;

        GetClientRect(*WindowHandle, &rect);

        if (rect.right != raster->width || rect.bottom != raster->height)
        {
            HRESULT hr;

            /* Remove managed resources from memory when creating a video memory resource */
            IDirect3DDevice9_EvictManagedResources(_RwD3DDevice);

            hr = IDirect3DDevice9_CreateDepthStencilSurface(_RwD3DDevice,
                raster->width,
                raster->height,
                rasExt->d3dFormat,
                Present.MultiSampleType,
                Present.MultiSampleQuality,
                FALSE,
                (LPSURFACE*)&(rasExt->texture),
                NULL);

            if (hr == D3DERR_OUTOFVIDEOMEMORY)
            {
                RWRETURN(FALSE);
            }
            else if (FAILED(hr))
            {
                RWRETURN(FALSE);
            }
        }
        else
        {
            rasExt->texture = (LPDIRECT3DTEXTURE9)RwD3D9DepthStencilSurface;
        }

        RWRETURN(TRUE);
    }

    RwBool RenderingEngine::rwD3D9CreateCameraTextureRaster(RwRaster* raster, _rwD3D9RasterExt* rasExt)
    {
        RwUInt32    rasFormat;
        RwUInt32    pixelFormat;
        HRESULT     hr;

        rasFormat = RwRasterGetFormat(raster);
        pixelFormat = rasFormat & rwRASTERFORMATPIXELFORMATMASK;

        /* rwRASTERTYPECAMERATEXTURE can not be palletized */
        if ((rasFormat & rwRASTERFORMATPAL4) || (rasFormat & rwRASTERFORMATPAL8))
        {
            RWRETURN(FALSE);
        }

        rasExt->d3dFormat = _rwRasterConvTable[FMT2TBL(rasFormat)].format;

        /* Does this raster format have an alpha component */
        rasExt->alpha = _rwRasterConvTable[FMT2TBL(rasFormat)].alpha;

        /* Check the size of the texture */
        _rwD3D9CheckRasterSize(&(raster->width), &(raster->height), raster->cType | rasFormat);

        /* Remove managed resources from memory when creating a video memory resource */
        IDirect3DDevice9_EvictManagedResources(_RwD3DDevice);

        /* Create the texture, if mipped 0 gets all the mip levels */
        if (rasExt->cube)
        {
            hr = IDirect3DDevice9_CreateCubeTexture(_RwD3DDevice, raster->width,
                (rasFormat & rwRASTERFORMATMIPMAP) ? 0 : 1,
                (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0) | D3DUSAGE_RENDERTARGET,
                rasExt->d3dFormat,
                D3DPOOL_DEFAULT,
                (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture),
                NULL);
        }
        else
        {
            hr = IDirect3DDevice9_CreateTexture(_RwD3DDevice, raster->width,
                raster->height,
                (rasFormat & rwRASTERFORMATMIPMAP) ? 0 : 1,
                (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0) | D3DUSAGE_RENDERTARGET,
                rasExt->d3dFormat,
                D3DPOOL_DEFAULT,
                &(rasExt->texture),
                NULL);
        }

        if (hr == D3DERR_OUTOFVIDEOMEMORY)
        {
            RWRETURN(FALSE);
        }
        else if (FAILED(hr))
        {
            if (rasExt->cube)
            {
                hr = IDirect3DDevice9_CreateCubeTexture(_RwD3DDevice, raster->width,
                    (rasFormat & rwRASTERFORMATMIPMAP) ? 0 : 1,
                    (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0),
                    Present.BackBufferFormat,
                    D3DPOOL_DEFAULT,
                    (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture),
                    NULL);
            }
            else
            {
                hr = IDirect3DDevice9_CreateTexture(_RwD3DDevice, raster->width,
                    raster->height,
                    (rasFormat & rwRASTERFORMATMIPMAP) ? 0 : 1,
                    (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0),
                    Present.BackBufferFormat,
                    D3DPOOL_DEFAULT,
                    &(rasExt->texture),
                    NULL);
            }

            if (FAILED(hr))
            {
                RWRETURN(FALSE);
            }
            else
            {
                raster->cFormat = (RwUInt8)(rwD3D9FindRwFormat(Present.BackBufferFormat) >> 8);
                raster->depth = _RwD3D9AdapterInformation.displayDepth;
            }
        }

        RWRETURN(TRUE);
    }

    RwBool  RenderingEngine::_rwD3D9RasterDestroy(void* unused1 __RWUNUSED__, void* raster,  RwInt32 unused3 __RWUNUSED__)
    {
        RwRaster* ras = (RwRaster*)raster;
        _rwD3D9RasterExt* rasExt;
        RwUInt32            n;

        rasExt = RASTEREXTFROMRASTER(ras);

        /* Check if it's the current rendering raster */
        for (n = 0; n < RWD3D9_MAX_TEXTURE_STAGES; n++)
        {
            if (_rwD3D9RWGetRasterStage(n) == ras)
            {
                _rwD3D9RWSetRasterStage(NULL, n);
            }
        }

        /*
         * Only free up the surface if this raster owns it (ie, it's not a sub
         * raster and it's not created with dontallocate flag)
         */
        if ((ras->parent == ras))
        {
            switch (ras->cType)
            {
            case (rwRASTERTYPENORMAL):
            {
                if (!(ras->cFlags & rwRASTERDONTALLOCATE))
                {
                    if (rasExt->palette != NULL)
                    {
                        rxD3D9VideoMemoryRasterListRemove(ras);

                        AddAvailablePaletteIndex(rasExt->palette->globalindex);

                        RwFreeListFree(PaletteFreeList, rasExt->palette);
                        rasExt->palette = NULL;
                    }
                    else if (IS_D3DFORMAT_ZBUFFER(rasExt->d3dFormat))
                    {
                        rxD3D9VideoMemoryRasterListRemove(ras);
                    }

                    /* Destroy the texture */
                    if (rasExt->texture)
                    {
                        IDirect3DTexture9_Release(rasExt->texture);
                    }
                }
            }
            break;

            case (rwRASTERTYPETEXTURE):
            {
                if (!(ras->cFlags & rwRASTERDONTALLOCATE))
                {
                    if (rasExt->palette != NULL)
                    {
                        rxD3D9VideoMemoryRasterListRemove(ras);

                        AddAvailablePaletteIndex(rasExt->palette->globalindex);

                        RwFreeListFree(PaletteFreeList, rasExt->palette);
                        rasExt->palette = NULL;
                    }
                    else if (IS_D3DFORMAT_ZBUFFER(rasExt->d3dFormat))
                    {
                        rxD3D9VideoMemoryRasterListRemove(ras);
                    }

                    /* Destroy the texture */
                    if (rasExt->texture)
                    {
                        IDirect3DTexture9_Release(rasExt->texture);
                    }
                }
            }
            break;

            case (rwRASTERTYPECAMERATEXTURE):
            {
                if (!(ras->cFlags & rwRASTERDONTALLOCATE))
                {
                    rxD3D9VideoMemoryRasterListRemove(ras);

                    /* Destroy the texture */
                    if (rasExt->texture)
                    {
                        IDirect3DTexture9_Release(rasExt->texture);
                    }
                }
            }
            break;

            case (rwRASTERTYPEZBUFFER):
            {
                if (!(ras->cFlags & rwRASTERDONTALLOCATE))
                {
                    rxD3D9VideoMemoryRasterListRemove(ras);

                    /* Destroy the surface */
                    if ((LPSURFACE)rasExt->texture != RwD3D9DepthStencilSurface)
                    {
                        if (rasExt->texture)
                        {
                            IDirect3DSurface9_Release((LPSURFACE)rasExt->texture);
                        }
                    }
                }
            }
            break;

            case (rwRASTERTYPECAMERA):
            {
                rxD3D9VideoMemoryRasterListRemove(ras);
            }
            break;

            default:
                RWRETURN(FALSE);
                break;
            }
        }
        else
        {
            /* Camera Sub rasters need to be removed */
            if (ras->cType == rwRASTERTYPECAMERA)
            {
                rxD3D9VideoMemoryRasterListRemove(ras);
            }
        }

        RWRETURN(TRUE);
    }

     void* RenderingEngine::rwD3D9RasterCtor(void* object,
            RwInt32 offsetInObject __RWUNUSED__,
            RwInt32 sizeInObject __RWUNUSED__)
    {
        _rwD3D9RasterExt* rasExt = RASTEREXTFROMRASTER(object);


        /* These are used to detect when we are using a camera for the first time */
        rasExt->swapChain = NULL;

        RWRETURN(object);
    }

     void* RenderingEngine::rwD3D9RasterDtor(void* object,
            RwInt32 offsetInObject __RWUNUSED__,
            RwInt32 sizeInObject __RWUNUSED__)
    {
        _rwD3D9RasterExt* rasExt = RASTEREXTFROMRASTER(object);

        if (rasExt->swapChain)
        {
            IDirect3DSwapChain9_Release(rasExt->swapChain);
            rasExt->swapChain = NULL;
        }

        /* Phew! */
        RWRETURN(object);
    }

    RwBool RenderingEngine::_rwD3D9RasterPluginAttach()
    {
        _RwD3D9RasterExtOffset = RwRasterRegisterPlugin( sizeof(_rwD3D9RasterExt),
                rwID_DEVICEMODULE, rwD3D9RasterCtor, rwD3D9RasterDtor, NULL);

        if (0 > _RwD3D9RasterExtOffset)
        {
            RWRETURN(FALSE);
        }

        /* Fill format info table */
        memset(_rwD3D9PixelFormatInfo, 0, MAX_PIXEL_FORMATS * sizeof(_rwD3D9FormatInfo));

        SETFORMATINFO(D3DFMT_R8G8B8, FALSE, 24, 0);
        SETFORMATINFO(D3DFMT_A8R8G8B8, TRUE, 32, rwRASTERFORMAT8888);
        SETFORMATINFO(D3DFMT_X8R8G8B8, FALSE, 32, rwRASTERFORMAT888);
        SETFORMATINFO(D3DFMT_R5G6B5, FALSE, 16, rwRASTERFORMAT565);
        SETFORMATINFO(D3DFMT_X1R5G5B5, FALSE, 16, rwRASTERFORMAT555);
        SETFORMATINFO(D3DFMT_A1R5G5B5, TRUE, 16, rwRASTERFORMAT1555);
        SETFORMATINFO(D3DFMT_A4R4G4B4, TRUE, 16, rwRASTERFORMAT4444);
        SETFORMATINFO(D3DFMT_R3G3B2, FALSE, 8, 0);
        SETFORMATINFO(D3DFMT_A8, TRUE, 8, 0);
        SETFORMATINFO(D3DFMT_A8R3G3B2, TRUE, 16, 0);
        SETFORMATINFO(D3DFMT_X4R4G4B4, FALSE, 16, 0);
        SETFORMATINFO(D3DFMT_A2B10G10R10, TRUE, 32, 0);
        SETFORMATINFO(D3DFMT_A8B8G8R8, TRUE, 32, 0);
        SETFORMATINFO(D3DFMT_X8B8G8R8, FALSE, 32, 0);
        SETFORMATINFO(D3DFMT_G16R16, FALSE, 32, 0);
        SETFORMATINFO(D3DFMT_A2R10G10B10, TRUE, 32, 0);
        SETFORMATINFO(D3DFMT_A16B16G16R16, TRUE, 64, 0);
        SETFORMATINFO(D3DFMT_A8P8, TRUE, 16, 0);
        SETFORMATINFO(D3DFMT_P8, FALSE, 8, rwRASTERFORMATPAL8);
        SETFORMATINFO(D3DFMT_L8, FALSE, 8, rwRASTERFORMATLUM8);
        SETFORMATINFO(D3DFMT_A8L8, TRUE, 16, 0);
        SETFORMATINFO(D3DFMT_A4L4, TRUE, 8, 0);
        SETFORMATINFO(D3DFMT_V8U8, FALSE, 16, 0);
        SETFORMATINFO(D3DFMT_L6V5U5, FALSE, 16, 0);
        SETFORMATINFO(D3DFMT_X8L8V8U8, FALSE, 32, 0);
        SETFORMATINFO(D3DFMT_Q8W8V8U8, FALSE, 32, 0);
        SETFORMATINFO(D3DFMT_V16U16, FALSE, 32, 0);
        SETFORMATINFO(D3DFMT_A2W10V10U10, TRUE, 32, 0);
        SETFORMATINFO(D3DFMT_D16_LOCKABLE, FALSE, 16, rwRASTERFORMAT16);
        SETFORMATINFO(D3DFMT_D32, FALSE, 32, rwRASTERFORMAT32);
        SETFORMATINFO(D3DFMT_D15S1, FALSE, 16, rwRASTERFORMAT16);
        SETFORMATINFO(D3DFMT_D24S8, FALSE, 32, rwRASTERFORMAT32);
        SETFORMATINFO(D3DFMT_D24X8, FALSE, 32, rwRASTERFORMAT32);
        SETFORMATINFO(D3DFMT_D24X4S4, FALSE, 32, rwRASTERFORMAT32);
        SETFORMATINFO(D3DFMT_D16, FALSE, 16, rwRASTERFORMAT16);
        SETFORMATINFO(D3DFMT_D32F_LOCKABLE, FALSE, 32, rwRASTERFORMAT32);
        SETFORMATINFO(D3DFMT_D24FS8, FALSE, 32, rwRASTERFORMAT32);
        SETFORMATINFO(D3DFMT_L16, FALSE, 16, 0);
        SETFORMATINFO(D3DFMT_Q16W16V16U16, FALSE, 64, 0);
        SETFORMATINFO(D3DFMT_R16F, FALSE, 16, 0);
        SETFORMATINFO(D3DFMT_G16R16F, FALSE, 32, 0);
        SETFORMATINFO(D3DFMT_A16B16G16R16F, TRUE, 64, 0);
        SETFORMATINFO(D3DFMT_R32F, FALSE, 32, 0);
        SETFORMATINFO(D3DFMT_G32R32F, FALSE, 64, 0);
        SETFORMATINFO(D3DFMT_A32B32G32R32F, TRUE, 128, 0);
        SETFORMATINFO(D3DFMT_CxV8U8, FALSE, 16, 0);

        RWRETURN(TRUE);
    }

    void RenderingEngine::_rwD3D9RasterOpen(void)
    {
        rxD3D9VideoMemoryRasterListCreate();
    }

    void RenderingEngine::_rwD3D9RasterClose()
    {
        rxD3D9VideoMemoryRasterListDestroy();

        NumPaletteIndexFree = 0;
        MaxPaletteIndexFree = 0;
        MaxPaletteIndex = 0;

        if (PaletteIndexFree != NULL)
        {
            RwFree(PaletteIndexFree);
            PaletteIndexFree = NULL;
        }

        if (PaletteFreeList != NULL)
        {
            RwFreeListDestroy(PaletteFreeList);
            PaletteFreeList = NULL;
        }
    }

    void RenderingEngine::rxD3D9VideoMemoryRasterListDestroy()
    {
        while (VideoMemoryRasters)
        {
            RxD3D9VideoMemoryRaster* next = VideoMemoryRasters->next;

            RwFreeListFree(VideoMemoryRastersFreeList, VideoMemoryRasters);

            VideoMemoryRasters = next;
        }

        if (VideoMemoryRastersFreeList != NULL)
        {
            RwFreeListDestroy(VideoMemoryRastersFreeList);
            VideoMemoryRastersFreeList = NULL;
        }

        RWRETURNVOID();
    }

    void RenderingEngine::_rxD3D9VideoMemoryRasterListRelease()
    {
        RxD3D9VideoMemoryRaster* currentVideoRaster;

        currentVideoRaster = VideoMemoryRasters;
        while (currentVideoRaster)
        {
            RwRaster* raster = currentVideoRaster->raster;
            _rwD3D9RasterExt* rasExt = RASTEREXTFROMRASTER(raster);
            RxD3D9VideoMemoryRaster* next = currentVideoRaster->next;

            if (raster->parent == raster)
            {
                switch (raster->cType)
                {
                case rwRASTERTYPENORMAL:
                case rwRASTERTYPETEXTURE:
                {
                    if (rasExt->texture)
                    {
                        if (IS_D3DFORMAT_ZBUFFER(rasExt->d3dFormat))
                        {
                            IDirect3DTexture9_Release(rasExt->texture);
                            rasExt->texture = NULL;
                        }
                    }
                }
                break;

                case rwRASTERTYPECAMERATEXTURE:
                {
                    if (rasExt->texture)
                    {
                        IDirect3DTexture9_Release(rasExt->texture);
                        rasExt->texture = NULL;
                    }
                }
                break;

                case rwRASTERTYPEZBUFFER:
                {
                    if (rasExt->texture)
                    {
                        if ((LPSURFACE)rasExt->texture != RwD3D9DepthStencilSurface)
                        {
                            IDirect3DSurface9_Release((LPSURFACE)rasExt->texture);
                            rasExt->texture = NULL;
                        }
                    }
                }
                break;

                case rwRASTERTYPECAMERA:
                {
                    if (rasExt->swapChain)
                    {
                        IDirect3DSwapChain9_Release(rasExt->swapChain);
                    }
                }
                break;

                default:
                    break;
                }
            }

            currentVideoRaster = next;
        }

        RWRETURNVOID();
    }

    RwBool RenderingEngine::_rxD3D9VideoMemoryRasterListRestore()
    {
        RwBool                  result;
        HRESULT                 hr;
        RxD3D9VideoMemoryRaster* currentVideoRaster;

        result = TRUE;
        hr = D3D_OK;
        currentVideoRaster = VideoMemoryRasters;
        while (currentVideoRaster)
        {
            RwRaster* raster = currentVideoRaster->raster;
            _rwD3D9RasterExt* rasExt = RASTEREXTFROMRASTER(raster);
            RxD3D9VideoMemoryRaster* next = currentVideoRaster->next;

            if (raster->parent == raster)
            {
                switch (raster->cType)
                {
                case rwRASTERTYPENORMAL:
                case rwRASTERTYPETEXTURE:
                {
                    RwUInt32    rasFormat = RwRasterGetFormat(raster);

                    if ((rasFormat & rwRASTERFORMATPAL4) || (rasFormat & rwRASTERFORMATPAL8))
                    {
                        if (rasExt->palette)
                        {
                            hr = IDirect3DDevice9_SetPaletteEntries(_RwD3DDevice,
                                rasExt->palette->globalindex,
                                (PALETTEENTRY*)(rasExt->palette->entries));
                        }
                    }
                    else if (IS_D3DFORMAT_ZBUFFER(rasExt->d3dFormat))
                    {
                        hr = IDirect3DDevice9_CreateTexture(_RwD3DDevice,
                            raster->width,
                            raster->height,
                            1,
                            D3DUSAGE_DEPTHSTENCIL,
                            rasExt->d3dFormat,
                            D3DPOOL_DEFAULT,
                            &(rasExt->texture),
                            NULL);
                    }
                }
                break;

                case rwRASTERTYPECAMERATEXTURE:
                {
                    if (rasExt->texture == NULL)
                    {
                        RwUInt32    rasFormat = RwRasterGetFormat(raster);
                        RwUInt32    levels = ((rasFormat & rwRASTERFORMATMIPMAP) ? 0 : 1);

                        if (rasExt->cube)
                        {
                            hr = IDirect3DDevice9_CreateCubeTexture(_RwD3DDevice,
                                raster->width,
                                levels,
                                (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0) | D3DUSAGE_RENDERTARGET,
                                rasExt->d3dFormat,
                                D3DPOOL_DEFAULT,
                                (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture),
                                NULL);
                        }
                        else
                        {
                            hr = IDirect3DDevice9_CreateTexture(_RwD3DDevice,
                                raster->width,
                                raster->height,
                                levels,
                                (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0) | D3DUSAGE_RENDERTARGET,
                                rasExt->d3dFormat,
                                D3DPOOL_DEFAULT,
                                &(rasExt->texture),
                                NULL);
                        }

                        if (FAILED(hr))
                        {
                            if (hr != D3DERR_OUTOFVIDEOMEMORY)
                            {
                                if (rasExt->cube)
                                {
                                    hr = IDirect3DDevice9_CreateCubeTexture(_RwD3DDevice, raster->width,
                                        (rasFormat & rwRASTERFORMATMIPMAP) ? 0 : 1,
                                        (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0),
                                        Present.BackBufferFormat,
                                        D3DPOOL_DEFAULT,
                                        (LPDIRECT3DCUBETEXTURE9*)&(rasExt->texture),
                                        NULL);
                                }
                                else
                                {
                                    hr = IDirect3DDevice9_CreateTexture(_RwD3DDevice, raster->width,
                                        raster->height,
                                        (rasFormat & rwRASTERFORMATMIPMAP) ? 0 : 1,
                                        (rasExt->automipmapgen ? D3DUSAGE_AUTOGENMIPMAP : 0),
                                        Present.BackBufferFormat,
                                        D3DPOOL_DEFAULT,
                                        &(rasExt->texture),
                                        NULL);
                                }

                                raster->cFormat = (RwUInt8)(_rwD3D9PixelFormatInfo[Present.BackBufferFormat].rwFormat >> 8);
                                raster->depth = _rwD3D9PixelFormatInfo[Present.BackBufferFormat].depth;

                                rasExt->d3dFormat = Present.BackBufferFormat;
                                rasExt->alpha = _rwD3D9PixelFormatInfo[Present.BackBufferFormat].alpha;
                            }
                        }
                    }
                }
                break;

                case rwRASTERTYPEZBUFFER:
                {
                    if (rasExt->texture == NULL)
                    {
                        /* Find the Z-Buffer depth */
                        if (16 == _RwD3D9ZBufferDepth)
                        {
                            if (_rwD3D9CheckValidZBufferFormat(D3DFMT_D15S1))
                            {
                                rasExt->d3dFormat = D3DFMT_D15S1;
                            }
                            else
                            {
                                rasExt->d3dFormat = D3DFMT_D16;
                            }
                        }
                        else if (32 == _RwD3D9ZBufferDepth)
                        {
                            if (_rwD3D9CheckValidZBufferFormat(D3DFMT_D24S8))
                            {
                                rasExt->d3dFormat = D3DFMT_D24S8;
                            }
                            else if (_rwD3D9CheckValidZBufferFormat(D3DFMT_D24X4S4))
                            {
                                rasExt->d3dFormat = D3DFMT_D24X4S4;
                            }
                            else if (_rwD3D9CheckValidZBufferFormat(D3DFMT_D32))
                            {
                                rasExt->d3dFormat = D3DFMT_D32;
                            }
                            else
                            {
                                rasExt->d3dFormat = D3DFMT_D24X8;
                            }
                        }

                        hr = IDirect3DDevice9_CreateDepthStencilSurface(_RwD3DDevice,
                            max(1, raster->width),
                            max(1, raster->height),
                            rasExt->d3dFormat,
                            Present.MultiSampleType,
                            Present.MultiSampleQuality,
                            FALSE,
                            (LPSURFACE*)&(rasExt->texture),
                            NULL);
                    }
                    else
                    {
                        RECT rect;

                        GetClientRect(*WindowHandle, &rect);

                        raster->width = rect.right;
                        raster->height = rect.bottom;

                        rasExt->texture = (LPDIRECT3DTEXTURE9)RwD3D9DepthStencilSurface;
                        rasExt->d3dFormat = Present.AutoDepthStencilFormat;
                    }
                }
                break;

                case rwRASTERTYPECAMERA:
                {
                    if (rasExt->swapChain)
                    {
                        D3DPRESENT_PARAMETERS   present;
                        RECT rect;

                        present = Present;

                        present.hDeviceWindow = (HWND)rasExt->window;

                        GetClientRect(present.hDeviceWindow, &rect);

                        if (rect.right)
                        {
                            present.BackBufferWidth = 0;
                        }
                        else
                        {
                            present.BackBufferWidth = 1;
                        }

                        if (rect.bottom)
                        {
                            present.BackBufferHeight = 0;
                        }
                        else
                        {
                            present.BackBufferHeight = 1;
                        }

                        present.EnableAutoDepthStencil = FALSE;

                        rasExt->swapChain = NULL;

                        if (D3D_OK == IDirect3DDevice9_CreateAdditionalSwapChain(_RwD3DDevice, &present, &(rasExt->swapChain)))
                        {
                            IDirect3DSurface9** ppBackBuffer =
                                (IDirect3DSurface9**)&(rasExt->texture);

                            hr = IDirect3DSwapChain9_GetBackBuffer(rasExt->swapChain, 0, D3DBACKBUFFER_TYPE_MONO, ppBackBuffer);

                            IDirect3DSurface9_Release((LPSURFACE)rasExt->texture);

                            rasExt->d3dFormat = present.BackBufferFormat;
                        }
                    }
                    else
                    {
                        if (_RwD3D9AdapterInformation.mode.Width &&
                            (RwUInt32)raster->width != Present.BackBufferWidth)
                        {
                            raster->width = (raster->width * Present.BackBufferWidth) / _RwD3D9AdapterInformation.mode.Width;
                        }

                        if (_RwD3D9AdapterInformation.mode.Height &&
                            (RwUInt32)raster->height != Present.BackBufferHeight)
                        {
                            raster->height = (raster->height * Present.BackBufferHeight) / _RwD3D9AdapterInformation.mode.Height;
                        }

                        raster->depth = _rwD3D9PixelFormatInfo[Present.BackBufferFormat].depth;
                        raster->cFormat = (RwUInt8)(_rwD3D9PixelFormatInfo[Present.BackBufferFormat].rwFormat >> 8);

                        rasExt->d3dFormat = Present.BackBufferFormat;
                        rasExt->alpha = _rwD3D9PixelFormatInfo[Present.BackBufferFormat].alpha;
                    }
                }
                break;

                default:
                    break;
                }
            }

            if (FAILED(hr))
            {
                result = FALSE;
            }

            currentVideoRaster = next;
        }

        RWRETURN(result);
    }

    void RenderingEngine::rxD3D9VideoMemoryRasterListRemove(RwRaster* raster)
    {
        RxD3D9VideoMemoryRaster* currentVideoRaster;
        RxD3D9VideoMemoryRaster* prevVideoRaster;


        currentVideoRaster = VideoMemoryRasters;
        prevVideoRaster = NULL;

        while (currentVideoRaster)
        {
            if (currentVideoRaster->raster == raster)
            {
                break;
            }

            prevVideoRaster = currentVideoRaster;
            currentVideoRaster = currentVideoRaster->next;
        }

        if (currentVideoRaster)
        {
            if (prevVideoRaster)
            {
                prevVideoRaster->next = currentVideoRaster->next;
            }
            else
            {
                if (VideoMemoryRasters == currentVideoRaster)
                {
                    VideoMemoryRasters = currentVideoRaster->next;
                }
            }

            RwFreeListFree(VideoMemoryRastersFreeList, currentVideoRaster);

        }

        RWRETURNVOID();
    }

    void RenderingEngine::rxD3D9VideoMemoryRasterListAdd(RwRaster* raster)
    {
        RxD3D9VideoMemoryRaster* videoRaster;

        videoRaster = (RxD3D9VideoMemoryRaster*) RwFreeListAlloc(VideoMemoryRastersFreeList, rwID_DRIVERMODULE | rwMEMHINTDUR_EVENT);

        videoRaster->raster = raster;
        videoRaster->next = VideoMemoryRasters;

        VideoMemoryRasters = videoRaster;
        RWRETURNVOID();
    }

     RwBool RenderingEngine::rxD3D9VideoMemoryRasterListCreate()
    {
        VideoMemoryRastersFreeList = RwFreeListCreate( sizeof(RxD3D9VideoMemoryRaster),
            127,    /* It results in ~1024 Bytes */
            4, rwID_DRIVERMODULE | rwMEMHINTDUR_EVENT);

        RWRETURN(VideoMemoryRastersFreeList != NULL);
    }

}