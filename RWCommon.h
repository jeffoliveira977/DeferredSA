#pragma once
#include "CommonD.h"

#define SLASH '\\'
#define RwIsAlpha(_c) \
   (  ( ((_c)>='A') && ((_c)<='Z') ) || \
      ( ((_c)>='a') && ((_c)<='z') ) )

#define NUMPIXELFORMATS     11
#define MAX_PIXEL_FORMATS   128
#define IS_D3DFORMAT_ZBUFFER(_d3dformat)    ((_d3dformat) >= D3DFMT_D16_LOCKABLE && (_d3dformat) <= D3DFMT_D16)
#define FMT2TBL(_p) (((_p) & rwRASTERFORMATPIXELFORMATMASK) >> 8)

typedef struct
{
    uint32_t stage;
    D3DTEXTURESTAGESTATETYPE type;
} RxTextureStageChangeType;

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

struct _rwD3D9StageStateCache
{
    RwRaster* curTexRaster;
    RwTextureAddressMode    texAddressU;
    RwTextureAddressMode    texAddressV;
    RwTextureFilterMode     filterMode;
    RwUInt32                borderColor;
    RwInt32                 maxAnisotropy;
};

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
const D3DTEXTUREADDRESS _RwD3D9AddressConvTable[NUMADDRESSMODES] =
{
    (D3DTEXTUREADDRESS)0,
    D3DTADDRESS_WRAP,
    D3DTADDRESS_MIRROR,
    D3DTADDRESS_CLAMP,
    D3DTADDRESS_BORDER
}; 


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


const _rwD3D9RasterConvData _rwRasterConvTable[NUMPIXELFORMATS] =
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

struct _rwFilterParams
{
    D3DTEXTUREFILTERTYPE minmag;
    D3DTEXTUREFILTERTYPE mip;
};

#define NUMFILTERMODES  7
const _rwFilterParams _RwD3D9FilterModeConvTable[NUMFILTERMODES + NUMFILTERMODES] =
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
const D3DCULL _RwD3D9CullModeConvTable[NUMCULLMODES] =
{
    (D3DCULL)0,
    D3DCULL_NONE,
    D3DCULL_CW,
    D3DCULL_CCW
};

#define NUMSTENCILOPERATIONS    9
const D3DSTENCILOP _RwD3D9StencilOpConvTable[NUMSTENCILOPERATIONS] =
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
const D3DCMPFUNC _RwD3D9CmpFuncConvTable[NUMCMPFUNCTIONS] =
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

struct DDS_PIXELFORMAT
{
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwFourCC;
    DWORD dwRGBBitCount;
    DWORD dwRBitMask;
    DWORD dwGBitMask;
    DWORD dwBBitMask;
    DWORD dwABitMask;
};

typedef struct _rxD3D9VideoMemoryRaster RxD3D9VideoMemoryRaster;
struct _rxD3D9VideoMemoryRaster
{
    RwRaster* raster;
    RxD3D9VideoMemoryRaster* next;
};

#define DDS_FOURCC 0x00000004  /* DDPF_FOURCC */
#define DDS_RGB    0x00000040  /* DDPF_RGB */
#define DDS_RGBA   0x00000041  /* DDPF_RGB | DDPF_ALPHAPIXELS */
#define DDS_ALPHAPIXELS 0x00000001  /* DDPF_ALPHAPIXELS */

const DDS_PIXELFORMAT DDSPF_DXT1 =
{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','1'), 0, 0, 0, 0, 0 };

const DDS_PIXELFORMAT DDSPF_DXT2 =
{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','2'), 0, 0, 0, 0, 0 };

const DDS_PIXELFORMAT DDSPF_DXT3 =
{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','3'), 0, 0, 0, 0, 0 };

const DDS_PIXELFORMAT DDSPF_DXT4 =
{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','4'), 0, 0, 0, 0, 0 };

const DDS_PIXELFORMAT DDSPF_DXT5 =
{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D','X','T','5'), 0, 0, 0, 0, 0 };

const DDS_PIXELFORMAT DDSPF_A8R8G8B8 =
{ sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 };

const DDS_PIXELFORMAT DDSPF_A1R5G5B5 =
{ sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000 };

const DDS_PIXELFORMAT DDSPF_A4R4G4B4 =
{ sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x0000f000, 0x000000f0, 0x0000000f, 0x0000f000 };

const DDS_PIXELFORMAT DDSPF_R8G8B8 =
{ sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 };

const DDS_PIXELFORMAT DDSPF_R5G6B5 =
{ sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 16, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000 };

#define DDS_HEADER_FLAGS_TEXTURE    0x00001007  /* DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT */
#define DDS_HEADER_FLAGS_MIPMAP     0x00020000  /* DDSD_MIPMAPCOUNT */
#define DDS_HEADER_FLAGS_VOLUME     0x00800000  /* DDSD_DEPTH */
#define DDS_HEADER_FLAGS_PITCH      0x00000008  /* DDSD_PITCH */
#define DDS_HEADER_FLAGS_LINEARSIZE 0x00080000  /* DDSD_LINEARSIZE */

#define DDS_SURFACE_FLAGS_TEXTURE 0x00001000 /* DDSCAPS_TEXTURE */
#define DDS_SURFACE_FLAGS_MIPMAP  0x00400008 /* DDSCAPS_COMPLEX | DDSCAPS_MIPMAP */
#define DDS_SURFACE_FLAGS_CUBEMAP 0x00000008 /* DDSCAPS_COMPLEX */

#define DDS_CUBEMAP_POSITIVEX 0x00000600 /* DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX */
#define DDS_CUBEMAP_NEGATIVEX 0x00000a00 /* DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX */
#define DDS_CUBEMAP_POSITIVEY 0x00001200 /* DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY */
#define DDS_CUBEMAP_NEGATIVEY 0x00002200 /* DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY */
#define DDS_CUBEMAP_POSITIVEZ 0x00004200 /* DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ */
#define DDS_CUBEMAP_NEGATIVEZ 0x00008200 /* DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ */

#define DDS_CUBEMAP_ALLFACES ( DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX |\
                               DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY |\
                               DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ )

#define DDS_FLAGS_VOLUME 0x00200000 /* DDSCAPS2_VOLUME */

struct DDS_HEADER
{
    DWORD dwSize;
    DWORD dwHeaderFlags;
    DWORD dwHeight;
    DWORD dwWidth;
    DWORD dwPitchOrLinearSize;
    DWORD dwDepth; /* only if DDS_HEADER_FLAGS_VOLUME is set in dwHeaderFlags */
    DWORD dwMipMapCount;
    DWORD dwReserved1[11];
    DDS_PIXELFORMAT ddspf;
    DWORD dwSurfaceFlags;
    DWORD dwCubemapFlags;
    DWORD dwReserved2[3];
};

struct DXTColBlock
{
    RwUInt16 col0;
    RwUInt16 col1;

    /* no bit fields - use bytes*/
    RwUInt8 row[4];
};

struct DXTAlphaBlockExplicit
{
    RwUInt16 row[4];
};

struct DXTAlphaBlock3BitLinear
{
    RwUInt8 alpha0;
    RwUInt8 alpha1;

    RwUInt8 stuff[6];
};

/* use cast to struct instead of RGBA_MAKE as struct is much */
struct Color8888
{
    RwUInt8 r;      /* change the order of names to change the */
    RwUInt8 g;      /*  order of the output ARGB or BGRA, etc...*/
    RwUInt8 b;      /*  Last one is MSB, 1st is LSB.*/
    RwUInt8 a;
};

struct Color565
{
    unsigned nBlue : 5;        /* order of names changes*/
    unsigned nGreen : 6;        /*  byte order of output to 32 bit */
    unsigned nRed : 5;
};

#define HAS_ALPHA           (1<<0)
#define IS_CUBE             (1<<1)
#define USE_AUTOMIPMAPGEN   (1<<2)
#define IS_COMPRESSED       (1<<3)

struct rwD3D9AdapterInformation
{
    RwChar          name[MAX_DEVICE_IDENTIFIER_STRING];
    int32_t         modeCount;
    D3DDISPLAYMODE  mode;
    int32_t         displayDepth;
    int32_t         flags;
};

struct _rwD3D9Palette
{
    PALETTEENTRY    entries[256];
    RwInt32     globalindex;
};

typedef LPDIRECT3DSURFACE9 LPSURFACE;
typedef LPDIRECT3DTEXTURE9 LPTEXTURE;

struct _rwD3D9RasterExt
{
    LPTEXTURE               texture;
    _rwD3D9Palette* palette;
    RwUInt8                 alpha;              /* This texture has alpha */
    RwUInt8                 cube : 4;           /* This texture is a cube texture */
    RwUInt8                 face : 4;           /* The active face of a cube texture */
    RwUInt8                 automipmapgen : 4;  /* This texture uses automipmap generation */
    RwUInt8                 compressed : 4;     /* This texture is compressed */
    RwUInt8                 lockedMipLevel;
    LPSURFACE               lockedSurface;
    D3DLOCKED_RECT          lockedRect;
    D3DFORMAT               d3dFormat;          /* D3D format */
    LPDIRECT3DSWAPCHAIN9    swapChain;
    HWND                    window;
};

#undef RwD3D9RenderSurface
#undef RwD3D9DepthStencilSurface
#undef RwD3D9D3D9ProjTransform
#undef RwD3D9D3D9ViewTransform
extern D3DPRESENT_PARAMETERS& Present;
extern rwD3D9AdapterInformation& _RwD3D9AdapterInformation;
extern uint32_t& _RwD3DAdapterIndex;
extern HWND* WindowHandle;
extern IDirect3D9*& _RwDirect3DObject;
extern LPDIRECT3DDEVICE9& _RwD3DDevice;
extern IDirect3DSurface9*& RwD3D9RenderSurface;
extern IDirect3DSurface9*& RwD3D9DepthStencilSurface;
extern D3DMATRIX& _RwD3D9D3D9ViewTransform;
extern D3DMATRIX& _RwD3D9D3D9ProjTransform;
extern RwInt32& _RwD3D9ZBufferDepth;
extern D3DCAPS9& _RwD3D9DeviceCaps;
extern D3DDEVTYPE& _RwD3DAdapterType;

extern IDirect3DSurface9*& CurrentDepthStencilSurface;
extern  IDirect3DSurface9* (&CurrentRenderSurface)[4];

#define _rwCPUHaveMMX() ((signed int (__cdecl*) ())0x80DBE0)()
#define _rwCPUHaveSSE() ((signed int (__cdecl*) ())0x80DC40)()
#define _rwCPUHaveSSE2() ((signed int (__cdecl*) ())0x80DCA0)()
#define _rwCPUHave3DNow() ((signed int (__cdecl*) ())0x80DD00)()
#define _rwProcessorInitialize() ((void (__cdecl*)())0x00857404)()

#define D3D9CalculateMaxMultisamplingLevels() ((void (__cdecl*)())0x007F6BF0)()

#define _rwD3D9VertexBufferManagerClose() ((void (__cdecl*)())0x007F5D20)()
#define _rwD3D9Im2DRenderClose() ((void (__cdecl*)())0x007FB5F0)()
#define _rwD3D9Im3DRenderClose() ((void (__cdecl*)())0x0080DFB0 )()
#define _rwD3D9DynamicVertexBufferRelease() ((void (__cdecl*)())0x007F5840 )()
#define RwFreeListPurgeAllFreeLists() ((void (__cdecl*)())0x00801F90 )()

#define _rwD3D9VertexBufferManagerOpen() ((signed int (__cdecl*)())0x007F5CB0)()
#define _rwProcessorRelease() ((void (__cdecl*)())0x0085742B)()

#define D3D9DeviceReleaseVideoMemory() ((signed int (__cdecl*) ())0x7F7F70)()
#define InitVideoMemoryRasters() ((signed int (__cdecl*)())0x4CC970)()
#define rwD3D9DynamicVertexBufferRestore() ((signed int (__cdecl*)())0x7F58D0)()
#define rwD3D9RenderStateReset() ((int (__cdecl*)())0x7FD100)()
#define rwD3D9Im2DRenderOpen() ((signed int (__cdecl*) ())0x7FB480)()
#define rwD3D9Im3DRenderOpen() ((signed int (__cdecl*) ())0x80E020)()
#define RenderStatesReset() ((int (__cdecl*)())0x700E00)()
#define RenderStatesSave() ((int (__cdecl*)())0x700CC0)()
#define _rwProcessorForceSinglePrecision() ((void (__cdecl*)())0x857432)()
#define _rwD3D9DynamicVertexBufferManagerForceDiscard() ((void (__cdecl*)())0x7F5800)() 
#define RwImageGetPath() ((RwChar * (__cdecl*)())0x802F60)() 
int _rwpathisabsolute(const RwChar* path);
RwBool RwFexist(const RwChar* name);

RwBool _rwD3D9DecompressDXT1(RwInt32 width, RwInt32 height, RwUInt32* destPixels, const RwUInt16* srcPixels);
void _rwD3D9DecompressDXT3(RwInt32 width, RwInt32 height, RwUInt32* destPixels, const RwUInt16* srcPixels);
void _rwD3D9DecompressDXT5(RwInt32 width, RwInt32 height, RwUInt32* destPixels, const RwUInt16* srcPixels);
RwBool D3D9RasterDXT1HasAlpha(RwRaster* raster);
RwUInt32 D3D9RasterGetSize(RwRaster* raster, RwUInt8 mipLevel);
RwUInt32 D3D9DDSGetSize(RwInt32 ddsWidth, RwInt32 ddsHeight, RwInt32 ddsDepth, RwUInt8 ddsCompressed, D3DFORMAT ddsFormat, RwUInt8 mipLevel);

RwFreeList* RwFreeListCreate(int32_t entrySize, int32_t entriesPerBlock, int32_t alignment, uint32_t hint);

RwBool _rwD3D9Im2DRenderLine(RwIm2DVertex* verts, RwInt32 numVerts, RwInt32 vert1, RwInt32 vert2);
RwBool _rwD3D9Im2DRenderTriangle(RwIm2DVertex* verts, RwInt32 numVerts, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3);
RwBool _rwD3D9Im2DRenderPrimitive(RwPrimitiveType primType, RwIm2DVertex* verts, RwInt32 numVerts);
RwBool _rwD3D9Im2DRenderIndexedPrimitive(RwPrimitiveType primType, RwIm2DVertex* verts, RwInt32 numVerts, RwImVertexIndex* indices, RwInt32 numIndices);

RwBool _rwVectorSetMultFn(rwVectorMultFn multPoint, rwVectorsMultFn multPoints, rwVectorMultFn multVector, rwVectorsMultFn multVectors);
RwV3d* _rwIntelSSEP4V3dTransformPoint(RwV3d* pointOut, const RwV3d* pointIn, const RwMatrix* matrix);
RwV3d* _rwIntelSSEP4V3dTransformPoints(RwV3d* pointsOut, const RwV3d* pointsIn, RwInt32 numPoints, const RwMatrix* matrix);
RwV3d* _rwIntelSSEP4V3dTransformVector(RwV3d* vectorOut, const RwV3d* vectorIn, const RwMatrix* matrix);
RwV3d* _rwIntelSSEP4V3dTransformVectors(RwV3d* vectorsOut, const RwV3d* vectorsIn, RwInt32 numVectors, const RwMatrix* matrix);
RwV3d* _rwIntelSSEV3dTransformPoint(RwV3d* pointOut, const RwV3d* pointIn, const RwMatrix* matrix);
RwV3d* _rwIntelSSEV3dTransformPoints(RwV3d* pointsOut, const RwV3d* pointsIn, RwInt32 numPoints, const RwMatrix* matrix);
RwV3d* _rwIntelSSEV3dTransformVector(RwV3d* vectorOut, const RwV3d* vectorIn, const RwMatrix* matrix);
RwV3d* _rwIntelSSEV3dTransformVectors(RwV3d* vectorsOut, const RwV3d* vectorsIn, RwInt32 numVectors, const RwMatrix* matrix);
