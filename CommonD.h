#pragma once
#include <string>
#include <algorithm>
#include <vector>
#include <list>
#include <d3d9.h>
#include <d3dx9.h>

#include "plugin.h"
#include "RenderWare.h"

#include <DirectXMath.h>

using namespace DirectX;
using namespace std;

typedef void (*RenderCallback)();

struct _rwD3D9Palette
{
    PALETTEENTRY    entries[256];
    RwInt32			globalindex;
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

#define RwD3D9RasterExtOffset (*(RwInt32*)0xB4E9E0)
#define RwD3D9DeviceCaps ((D3DCAPS9 *)0xC9BF00)
#define RwD3DDevice (*(IDirect3DDevice9 **)0xC97C28)
#define CurrentRenderSurface ((IDirect3DSurface9 **)0xC98090)
#define CurrentDepthStencilSurface (*(IDirect3DSurface9 **)0xC9808C)
#define RwD3D9RenderSurface (*(IDirect3DSurface9 **)0xC97C30)
#define RwD3D9DepthStencilSurface (*(IDirect3DSurface9 **)0xC97C2C)

#define _rxPipelineGlobalsOffset    (*(RwInt32*)0xC9BC60)
#define meshModule                 (*(RwModuleInfo*)0xC9B8C0)
#define resourcesModule            (*(RwModuleInfo*)0xC9BC58)

#undef RXPIPELINEGLOBAL
#define RXPIPELINEGLOBAL(var) (RWPLUGINOFFSET(rwPipeGlobals, RwEngineInstance, _rxPipelineGlobalsOffset)->var)

#define RWRESOURCESGLOBAL(var) (RWPLUGINOFFSET(rwResourcesGlobals,  \
    RwEngineInstance, resourcesModule.globalsOffset)->var)

#define RASTEREXTFROMRASTER(raster) \
    ((_rwD3D9RasterExt *)(((RwUInt8 *)(raster)) + RwD3D9RasterExtOffset))
#define RASTEREXTFROMCONSTRASTER(raster) ((const _rwD3D9RasterExt*) \
     (((const RwUInt8 *)(raster)) + RwD3D9RasterExtOffset))

#define BUFFER_MAX_INDEX  0x10000

extern RwUInt32& IB3DOffset;
extern LPDIRECT3DINDEXBUFFER9& IndexBuffer3D;
extern LPDIRECT3DVERTEXDECLARATION9& VertexDeclIm3D;
extern LPDIRECT3DVERTEXDECLARATION9& VertexDeclIm3DNoTex;
extern PDIRECT3DVERTEXDECLARATION9& VertexDeclIm3DOld;
extern rwIm3DPool*& _rwD3D9ImmPool;
extern RwUInt32&                 CurrentBaseIndex3D;
extern LPDIRECT3DVERTEXBUFFER9&  CurrentVertexBuffer3D;
extern D3DPRIMITIVETYPE* _RwD3D9PrimConv;

struct RxD3D9Im3DVertex
{
    RwReal x;
    RwReal y;
    RwReal z;
    RwUInt32 color;
    RwReal u;
    RwReal v;
};

struct _rxD3D9Im3DVertexNoTex
{
    RwReal x;
    RwReal y;
    RwReal z;
    RwUInt32 color;
};

#define RwLongToRGBAReal(o, i)                  \
(o)->alpha = (RwReal)(i >> 24 & 0xFF) / 255.0f; \
(o)->red   = (RwReal)(i >> 16 & 0xFF) / 255.0f; \
(o)->green = (RwReal)(i >> 8 & 0xFF) / 255.0f;  \
(o)->blue  = (RwReal)(i & 0xFF) / 255.0f;       \

#define RwD3D9D3D9ViewTransform ( *(RwMatrix *)0xC9BC80 )
#define RwD3D9D3D9ProjTransform ( *(RwMatrix *)0x8E2458 )
#define RwCameraSync( camera )                                                 \
    ( (RwCamera * (__cdecl *)(RwCamera *))0x7EE5A0 )( camera )

#define rwD3D9DeleteVertexDeclaration(declaration) ((RwBool (__cdecl*)(PDIRECT3DVERTEXDECLARATION9))0x7FAC10)(declaration)
#define _rwD3D9RWGetRasterStage(stage) ((RwRaster*(__cdecl*)(RwUInt32))0x7FDE50)(stage)
#define RwD3D9DeleteVertexShader(shader) ((RwBool (__cdecl*)(void *))0x7FAC90)(shader)
#define RwD3D9DeletePixelShader(shader) ((RwBool (__cdecl*)(void *))0x7FACF0)(shader)
//#define RwD3D9CreatePixelShader(function, shader) ((RwBool (__cdecl*)(const RwUInt32*, void **))0x7FACC0)(function, shader)
#define RwD3D9CreateVertexShader(function, shader) ((RwBool (__cdecl*)(const RwUInt32*, void **))0x7FAC60)(function, shader)
#define rxD3D9VideoMemoryRasterListAdd(raster) ((void(__cdecl*)(RwRaster*))0x4CB530)(raster)
#define RwD3D9RasterCreate(width, height, d3dFormat, rasterFormat) ((RwRaster *(__cdecl *)(RwInt32, RwInt32, D3DFORMAT, RwUInt32))0x4CD050)(width, height, d3dFormat, rasterFormat)
#define _rwD3D9RWSetRasterStage(raster, stage) ((RwBool (__cdecl *)(RwRaster*, RwInt32))0x7FDCD0)(raster, stage)
#define __rwD3D9SetRenderTarget(index, surface) ((RwBool (__cdecl *)(RwUInt32, IDirect3DSurface9*))0x7F5F20)(index, surface);
#define _rwD3D9CubeRasterCreate(raster, d3dformat, mipmapLevels) ((RwBool (__cdecl *)(RwRaster *,RwUInt32,RwUInt32))0x4C9ED0)(raster, d3dformat, mipmapLevels)
#define rwD3D9CreateCameraTextureRaster(raster, rasExt) ((RwBool (__cdecl *)(RwRaster *,_rwD3D9RasterExt*))0x4CB9C0)(raster, rasExt)
#define _rwD3D9CubeRasterSelectFace(raster, face)((void (__cdecl *)(RwRaster *, RwUInt32))0x4C9F40)(raster, face)
#define RwD3D9DynamicVertexBufferUnlock(buffer) ((RwBool (__cdecl *)(IDirect3DVertexBuffer9*))0x7F5C90)(buffer) 
#define RwD3D9TextureHasAlpha(texture) ((RwBool (__cdecl*) (RwTexture*)) 0x4C9EA0)(texture)
#define RpMatFXMaterialGetEffects(material)((RpMatFXMaterialFlags (__cdecl *)(RpMaterial *)) 0x5D70D0)(material)
#define RpMaterialGetFxEnvShininess(material) ((float (__cdecl *)(RpMaterial *)) 0x5D70D0)(material)
#define RwD3D9DynamicVertexBufferDestroy(vertexBuffer)((void (__cdecl *)(void *)) 0x7F5AE0)(vertexBuffer)
#define _rwD3D9CheckValidZBufferTextureFormat(format) ((RwBool (__cdecl *)(RwInt32)) 0x4CBE80)(format)
#define _rwD3D9AtomicDefaultLightingCallback(object) ((void (__cdecl *)(void *)) 0x757400 )(object)
#define _rpD3D9VertexShaderCacheOpen() ((void (__cdecl *)()) 0x760CF0)()
#define rwD3D9DrawPrimitive(primitiveType, startVertex, primitiveCount) ((void (__cdecl *)(RwUInt32, RwUInt32, RwUInt32)) 0x007FA360)(primitiveType, startVertex, primitiveCount)

typedef LPDIRECT3DSURFACE9 LPSURFACE;
typedef LPDIRECT3DTEXTURE9 LPTEXTURE;

struct RpSkin
{
    RwUInt32        numBones;
    RwUInt32        numBoneIds;
    RwUInt8* boneIds;
    RwMatrix* skinToBoneMatrices;
    RwUInt32        maxNumWeightsForVertex;
    RwUInt32* vertexBoneIndices;
    RwMatrixWeights* vertexBoneWeights;
    RwUInt32        boneCount;
    RwBool          useVS;
    RwUInt32        boneLimit;
    RwUInt32        numMeshes;
    RwUInt32        numRLE;
    RwUInt8* meshBoneRemapIndices;
    RwUInt32        meshBoneRLECount;
    void* meshBoneRLE;
    void* field_3C;
};

std::vector<BYTE> readFile(const char* filename);
void rwD3D9SetRenderTargets(RwRaster* rasters[], int rasterCount, RwUInt32 startIndex);
#define rpD3D9SkinVertexShaderMatrixUpdate(matrices, atomic, skin) ((void( __cdecl*)(RwMatrix*, RpAtomic*, RpSkin*))0x7C78A0)(matrices, atomic, skin)
#define RwD3D9RenderStateReset() ((int (__cdecl*)())0x7FD100)()
#define RpSkinAtomicGetHAnimHierarchy(atomic) ((RpHAnimHierarchy*( __cdecl*)(const RpAtomic*))0x7C7540)(atomic)
#define rpSkinGeometryGetSkin(geometry) ((RpSkin*( __cdecl*)(RpGeometry*))0x7C7550)(geometry)
#define rpSkinGetNumBones(skin) ((int( __cdecl*)(RpSkin*))0x7C77E0)(skin)
#define rpSkinGetVertexBoneWeights(skin) ((const RwMatrixWeights*( __cdecl*)(RpSkin*))0x7C77F0)(skin)
#define rpSkinGetVertexBoneIndices(skin) ((const RwUInt32*( __cdecl*)(RpSkin*))0x7C7800)(skin)
#define rpSkinGetMeshBoneRLECount(skin) ((const RwInt32( __cdecl*)(RpSkin*))0x7C8A50)(skin)
#define rpSkinGetSkinToBoneMatrices(skin) (( RwMatrix* ( __cdecl*)(RpSkin*))0x7C7810)(skin)
#define rpSkinGetAlignedMatrixCache() ((RwMatrix* ( __cdecl*)())0x7C6810)()
#define RxLockedPipeAddFragment(pipeline, firstIndex, nodeDef0) ((RxPipeline* ( __cdecl*)(RxPipeline*, RwUInt32*, RxNodeDefinition*, RxNodeDefinition*))0x806BE0)(pipeline, firstIndex, nodeDef0, NULL) 
#define RsMouseSetPos(pos) ((int (__cdecl *)(RwV2d *)) 0x6194A0)(pos)
#define RsCameraShowRaster(camera) ((int (__cdecl *)(RwCamera *))0x619440)(camera)
#define RsCameraBeginUpdate(camera) ((signed int (__cdecl *)(RwCamera *))0x619450)(camera)
#define _rwD3D9SetDepthStencilSurface(depthStencilSurface) ((void (__cdecl *)(LPSURFACE))0x007F5EF0)(depthStencilSurface)
RwMatrix RwMatrixTranspose(RwMatrix* mtx);
enum eZoneAttributes : uint16_t
{
    NONE = 0x0,
    CAM_CLOSE_IN_FOR_PLAYER = 0x1,
    CAM_STAIRS_FOR_PLAYER = 0x2,
    CAM_1ST_PERSONS_FOR_PLAYER = 0x4,
    CAM_NO_RAIN = 0x8,
    NO_POLICE = 0x10,
    DO_I_NEED_TO_LOAD_COLLISION = 0x40,
    UNKNOWN_1 = 0x80,
    POLICES_ABANDON_CARS = 0x100,
    IN_ROOMS_FOR_AUDIO = 0x200,
    IN_ROOMS_FEWER_PEDS = 0x400,
    UNKNOWN_2 = 0x800,
    MILITARY_ZONE = 0x1000,
    EXTRA_AIR_RESISTANCE = 0x4000,
    FEWER_CARS = 0x8000,
};

#define SAFE_RELEASE(p) {if(p){(p)->Release();(p)=NULL;}}
#define CWorldScan__ScanWorld(a1,a2,a3) ((void (__cdecl *)(CVector2D *, signed int , void (__cdecl *)(int, int) ) )0x72CAE0)(a1,a2,a3)
#define CRenderer__ScanSectorList(a1,a2) ((void (__cdecl *)(int, int))0x554840)(a1,a2)
#define CRenderer__ScanBigBuildingList(a1,a2) ((void (__cdecl *)(int, int))0x554B10)(a1,a2)
#define CRenderer__SetupMapEntityVisibility(entity,mapInfo,renderDistance,a4) ((signed int (__cdecl *)(CEntity *, CBaseModelInfo *,float,char))0x553F60)(entity,mapInfo,renderDistance,a4)
#define CRenderer__AddEntityToRenderList(entity,renderDistance) ((char (__cdecl *)(CEntity *, float))0x5534B0)(entity,renderDistance)
#define CRenderer__AddToLodRenderList(entity,renderDistance) ((char (__cdecl *)(CEntity *, float))0x553710)(entity,renderDistance)
#define CVisibilityPlugins__RenderEntityADDR ((void*)0x732B40)

#define CVisibilityPlugins__InsertEntityIntoUnderwaterEntities(entity,renderDistance) ((bool (__cdecl *)(CEntity *, float))0x733D90)(entity, renderDistance)
#define CTimer__GetTimeMillisecondsFromStart() ((size_t  (__cdecl *)())0x53BAD0)()
#define CGame__TimeMillisecondsFromStart (*(size_t*)0xB72CA8)
#define CTimer__Update() ((void (__cdecl *)())0x561B10)()
#define CPostEffects__NightVisionSetLights() ((void (__cdecl *)())0x7012E0)()
#define CPostEffects__IsVisionFXActive() ((bool (__cdecl *)())0x7034F0)()
#define CPostEffects__FilterFX_StoreAndSetDayNightBalance() ((void (__cdecl *)())0x7034B0)()
#define CPostEffects__FilterFX_RestoreDayNightBalance()  ((void (__cdecl *)())0x7034D0)()
#define CPostEffects__InfraredVisionSetLightsForDefaultObjects()((void(__cdecl*)())0x701430)()
#define CPostEffects__m_bInfraredVision (*(bool*)0xC402B9)
#define CPostEffects__m_bNightVision (*(bool*)0xC402B8)

#define CRenderer__SetupBigBuildingVisibility(entity,renderDistance) ((char (__cdecl *)(CEntity *, float*))0x554650)(entity,renderDistance)
#define COcclusion__ProcessBeforeRendering() ((void ( __cdecl*)())0x007201C0)() 
#define CCullZones__FindTunnelAttributesForCoors(point) ((eZoneAttributes ( __cdecl*)(CVector))0x72D9F0 )(point) 
#define RpHAnimHierarchyGetMatrixArray(hierarchy) ((RwMatrix* ( __cdecl*)(RpHAnimHierarchy*))0x7C5120)(hierarchy) 
#define CWaterLevel__RenderWater() ((void (__cdecl *)())0x6EF650)()
#define CRenderer__ConstructRenderList() ((void (__cdecl *)())0x5556E0)()
#define CWaterLevel__GetWaterDepth(vecPos, pfDepth, pfLevelNoWaves, pfGroundLevel) ((void(__cdecl*)(CVector&, float*, float*, float*))0x6EA960)(vecPos, pfDepth, pfLevelNoWaves, pfGroundLevel)
#define CTimer__m_snTimeInMilliseconds (*(UINT *)0xB7CB84)
#define CTimer__ms_fTimeStep (*(UINT *)0xB7CB5C)
//#define g_fx ((void *)0xA9AE00)
#define CHeli__NumberOfSearchLights (*(unsigned int *)0xC1C96C)
#define CTheScripts__NumberOfScriptSearchLights (*(unsigned short *)0xA90830)
//#define Fx_c__Render(ptfx, camera, unk) ((void (__thiscall *)(void *, RwCamera *, int))0x49E650)(ptfx, camera, unk)
#define CVisibilityPlugins__RenderWeaponPedsForPC() ((void (__cdecl *)())0x732F30)()
#define CClouds__Render() ((void (__cdecl *)())0x713950)()
#define CCoronas__RenderReflections() ((void (__cdecl *)())0x6FB630)()
#define CCoronas__RenderSunReflection() ((void (__cdecl *)())0x6FBAA0)()
#define CClouds__RenderBottomFromHeight() ((void (__cdecl *)())0x7154B0)()
#define CWeather__RenderRainStreaks() ((void (__cdecl *)())0x72AF70)()
#define CWeather__UnderWaterness (*(float *)0xC8132C)
#define CPad__GetPad(number) ((CPad *(__cdecl *)(unsigned int))0x53FB70)(number)
//#define FindPlayerPed(number) ((CPed *(__cdecl *)(unsigned int))0x56E210)(number)
#define CPlayerPed__DrawTriangleForMouseRecruitPed(ped) ((void (__thiscall *)(CPed *))0x60BA80)(ped)
#define CReplay__Mode (*(unsigned char *)0xA43088)
#define g_breakMan ((void *)0xBB4240)
#define CRenderer__RenderRoads() ((void (__cdecl *)())0x553A10)()
#define CCoronas__RenderReflections() ((void (__cdecl *)())0x6FB630)()
#define CRenderer__RenderEverythingBarRoads() ((void (__cdecl *)())0x553AA0)()
#define BreakManager_c__Render(unk1, unk2) ((void (__thiscall *)(void *, int))0x59E6A0)(unk1, unk2)
#define CRenderer__RenderFadingInUnderwaterEntities() ((void (__cdecl *)())0x553220)()
#define CRenderer__RenderFadingInEntities() ((void (__cdecl *)())0x5531E0)()
#define CBirds__Render() ((void (__cdecl *)())0x712810)()
#define CSkidmarks__Render() ((void (__cdecl *)())0x720640)()
#define CRopes__Render() ((void (__cdecl *)())0x556AE0)()
#define CGlass__Render() ((void (__cdecl *)())0x71CE20)()
#define CVisibilityPlugins__RenderReallyDrawLastObjects() ((void (__cdecl *)())0x733800)()
#define CCoronas__Render() ((void (__cdecl *)())0x6FAEC0)()
#define CWaterCannons__Render() ((void (__cdecl *)())0x729B30)()
#define CWaterLevel__RenderWaterFog() ((void (__cdecl *)())0x6E7760)()
#define CClouds__MovingFogRender() ((void (__cdecl *)())0x716C90)()
#define CClouds__VolumetricCloudsRender() ((void (__cdecl *)())0x716380)()
#define CHeli__Pre_SearchLightCone() ((void (__cdecl *)())0x6C4650)()
#define CHeli__RenderAllHeliSearchLights() ((void (__cdecl *)())0x6C7C50)()
#define CTheScripts__RenderAllSearchLights() ((void (__cdecl *)())0x493E30)()
#define CHeli__Post_SearchLightCone() ((void (__cdecl *)())0x6C46E0)()
#define CWeaponEffects__Render() ((void (__cdecl *)())0x742CF0)()
#define CSpecialFX__Render() ((void (__cdecl *)())0x726AD0)()
#define CPointLights__RenderFogEffect() ((void (__cdecl *)())0x7002D0)()
#define CRenderer__RenderFirstPersonVehicle() ((void (__cdecl *)())0x553D00)()
#define CPostEffects__Render() ((void (__cdecl *)())0x7046E0)()
typedef void cDMAudio;
#define DoRWStuffStartOfFrame_Horizon(topR, topG, topB, bottomR, bottomG, bottomB, A) ((RwUInt32 (__cdecl *)(RwUInt16, RwUInt16, \
	RwUInt16, RwUInt16, RwUInt16, RwUInt16, RwUInt16))0x53D7A0)(topR, topG, topB, bottomR, bottomG, bottomB, A)
#define UpdateTimer() ((void (__cdecl *)())0x561B10)()
#define InitSprite2dPerFrame() ((void (__cdecl *)())0x727350)()
#define InitFontPerFrame() ((void (__cdecl *)())0x719800)()
#define ProcessGame() ((void (__cdecl *)())0x53BEE0)()
#define CAudioEngine__Service(audio) ((void (__thiscall *)(cDMAudio *))0x507750)(audio)
#define GetCameraScreenFadeStatus(camera) ((signed int (__thiscall *)(CCamera *))0x50AE20)(camera)
#define CDraw__CalculateAspectRatio() ((double (__cdecl *)())0x6FF420)()
#define CVisibilityPlugins__SetRenderWareCamera(camera) ((void (__cdecl *)(RwCamera *))0x7328C0)(camera)
#define CameraSize(camera, rect, viewWindow, aspectRatio) ((void (__cdecl *)(RwCamera *, CRect *, float, float))0x72FC70)\
	(camera, rect, viewWindow, aspectRatio)
#define GetTimeFromRenderStart() ((DWORD (__cdecl *)())0x561A80)()
#define GetTimerDivider() ((int (__cdecl *)())0x561A40)()
#define Render2dStuff() ((int (__cdecl *)())0x53E230)()
#define AudioEngine ((cDMAudio *)0xB6BC90)
#define Timecycle ((CTimeCycleInfo *)0xB7C4A0)
#define g_realTimeShadowMan ((void* *)0xC40350)
#define NumPointLights (*(unsigned int *)0xC3F0D0)
#define CDraw__ms_fAspectRatio (*(float *)0xC3EFA4)
#define gfFOV (*(float *)0x8D5038)
#define CMenuManager__DrawFrontEnd(menuManager) ((void (__thiscall *)(CMenuManager&))0x57C290)(menuManager)
#define DoFade() ((void (__cdecl *)())0x53E600)()
#define DrawStyledText() ((void (__cdecl *)())0x58D490)()
#define CWeather__WetRoads (*(float*)0xC81308)
#define CCustomBuildingDNPipeline__m_fDNBalanceParam (*(float *)0x8D12C0) 
#define CCustomCarEnvMapPipeline__GetFxSpecSpecularity(material) ((float(__cdecl *)(const RpMaterial *))0x5D8B90)(material)
#define CCustomCarEnvMapPipeline__GetFxEnvShininess(material) ((float( __cdecl *)(const RpMaterial *))0x5D70D0)(material)
#define gColourTop ((RwRGBA *)0xB72CA0)
#define TempBufferVerticesStored (*(UINT *)0xC4B950)
#define TempBufferIndicesStored (*(UINT *)0xC4B954)
#define TempVertexBuffer ((RwIm3DVertex *)0xC4D958)
#define TempBufferRenderIndexList ((USHORT *)0xC4B958)

#define CRenderer__RenderOneRoad(e) ((void(__cdecl*)(CEntity*))0x553230)(e)
#define CRenderer__RenderOneNonRoad(e)	((void(__cdecl*)(CEntity*))0x553260)(e)

#define rwD3D9MeshGetNumVerticesMinIndex(indices, numIndices, numVertices, minv) ((void ( __cdecl*)(const RxVertexIndex*,  RwUInt32, RwUInt32*,  RwUInt32*))0x756710)(indices, numIndices, numVertices, minv) 
#define rwD3D9ResourceEntryInstanceDataDestroy(repEntry) (((void (__cdecl*)(RwResEntry*))0x4C9990)(repEntry)) 
#define rwResourcesAllocateResEntry(owner, ownerRef, size, destroyNotify) ((RwResEntry* (__cdecl*)(void* owner, RwResEntry** ownerRef, RwInt32 size, RwResEntryDestroyNotify destroyNotify))0x807ED0)(owner, ownerRef, size, destroyNotify) 
#define _rwD3D9ConvertToTriList(indexDst, indexSrc, numIndices, minVert) ((RwUInt32 (__cdecl*)(RxVertexIndex*, const RxVertexIndex* , RwUInt32 , RwUInt32 ))0x756770)(indexDst, indexSrc, numIndices, minVert) 
#define _rwD3D9SortTriListIndices(indices, numIndices) ((void (__cdecl*)(RxVertexIndex*, RwUInt32))0x756770)(indices, numIndices) 
#define _rwD3D9IndexBufferCreate(numIndices, indexBuffer) ((RwBool (__cdecl*)(RwUInt32, void*))0x4C9970)(numIndices, indexBuffer) 
#define RwResourcesFreeResEntry(entry) ((RwBool ( __cdecl*)(RwResEntry*))0x807DE0)(entry)
#define RxPipelineDestroy(pipeline) ((void ( __cdecl*)(RxPipeline*))0x805820)(pipeline) 


#define DEFERREDSHADERPATH "DeferredSA/shaders/binary/"

void RwD3D9RestoreRenderTargets(int count);
IDirect3DVertexShader9* CreateVertexShader(string path, string profile);
IDirect3DPixelShader9* CreatePixelShader(string path, string profile);
void* RwCreateCompiledVertexShader(string filename);
void* RwCreateCompiledPixelShader(string filename);

void RwD3D9SetPixelShaderConstantB(RwUInt32 registerAddress, const void* constantData, RwUInt32 constantCount);
RwBool rwD3D9CubeRasterCreate(RwRaster* raster, RwUInt32 d3dformat, RwInt32 mipmapLevels);
XMMATRIX RwMatrixToXMMATRIX(const RwMatrix* mtx);

RwRGBA LerpSkyColor();
CVector GetSkyTopColor();
CVector GetSkyBottomColor();
CVector GetObjectColor();
CVector GetSunColor();
CVector GetSunDirection();
RwMatrix XMMATRIXtoRwMatrix(XMMATRIX mtx);
RwTexture* LoadTextureFromFile(const RwChar* filename);
RwTexture* LoadBMPTextureFromFile(const RwChar* filename);
#include "CMessages.h"
#include <stdio.h>
template<typename... Args>
void PrintMessage(char* fmt, Args... args) // recursive variadic function
{
    static char buf[256];
    sprintf_s(buf, fmt, args...);
    CMessages::AddMessageJumpQ(buf, 100, 0, 0);
}
extern char(&PC_Scratch)[16384];
constexpr float TWO_PI = 6.28318530718f;
constexpr float PI = 3.14159265358979323846f;
constexpr float HALF_PI = PI / 2.0f;
constexpr float LOG10_2 = 0.30102999566398119802f; // log10(2)
constexpr float SQRT_2 = 1.41421356237309504880f;
constexpr float SIN_PI = 0.0f; // std::sin(PI);
constexpr float COS_PI = -1.0f; // std::cos(PI);

template <typename T>
T clamp(T value, T low, T high)
{
    return min(max(value, low), high);
}

constexpr float DegreesToRadians(float angleInDegrees)
{
    return angleInDegrees * PI / 180.0F;
}

constexpr float RadiansToDegrees(float angleInRadians)
{
    return angleInRadians * 180.0F / PI;
}
void rwD3D9RWSetRasterStage(RwRaster* raster, RwUInt32 stage);
void rwD3D9SetSamplerState(RwUInt32 stage, RwUInt32 type, RwUInt32 value);
void rwD3D9SetDepthStencil(RwRaster* depthStencil);
bool GetSunPosn(CVector* dst, float farClip);

void GetGaussianOffsets(bool bHorizontal, float texels, RwV2d* vSampleOffsets, float* fSampleWeights);


void CameraPan(RwCamera* camera, const RwV3d* pos, RwReal angle);
void CameraTilt(RwCamera* camera, const RwV3d* pos, RwReal angle);
void CameraRotate(RwCamera* camera, const RwV3d* pos, RwReal angle);
void CameraPoint(RwCamera* camera, RwReal tilt, RwReal turn);
void TranslateCameraZ(RwCamera* camera, RwReal dist);

RpAtomic* AtomicAddVertices(RpAtomic* atomic, void* data);
void ClumpCalculateWorldBoundingBox(RpClump* clump);
void ClumpRenderBoundingBox(RpClump* clump);

void AtomicCalculateWorldBoundingBox(RpAtomic* atomic);
void AtomicRenderBoundingBox(RpAtomic* atomic);


inline float DotProduct(const CVector& v1, const CVector& v2)
{
    return v1.z * v2.z + v1.y * v2.y + v1.x * v2.x;
}

inline inline CVector operator/(const CVector& vec, float dividend)
{
    return CVector(vec.x / dividend, vec.y / dividend, vec.z / dividend);
}

inline inline float SquaredMagnitude(float x, float y)
{
    return x * x + y * y;
}

inline float SquaredMagnitude(float x, float y, float z)
{
    return x * x + y * y + z * z;
}

inline float SquaredMagnitude(CVector vec)
{
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}
struct Vec3
{
    Vec3()
    {}
    Vec3(float x, float y, float z)
    {
        X = x;
        Y = y;
        Z = Z;
    }
    float X, Y, Z;
    float Length() const
    {
        return sqrt((X * X) + (Y * Y) + (Z * Z));
    }
    float Vec3::LengthSquared() const
    {
        return (X * X) + (Y * Y) + (Z * Z);
    }
    Vec3 operator/(const Vec3& other)
    {
        Vec3 result;
        result.X = X / other.X;
        result.Y = Y / other.Y;
        result.Z = Z / other.Z;
        return result;
    }
    Vec3 Vec3::operator-(const Vec3& other)
    {
        Vec3 result;
        result.X = X - other.X;
        result.Y = Y - other.Y;
        result.Z = Z - other.Z;
        return result;
    }
    Vec3 Vec3::operator-=(const Vec3& other)
    {
        Vec3 result;
        result.X = X - other.X;
        result.Y = Y - other.Y;
        result.Z = Z - other.Z;
        return result;
    }
    Vec3 Vec3::operator+(const Vec3& other)
    {
        Vec3 result;
        result.X = X + other.X;
        result.Y = Y + other.Y;
        result.Z = Z + other.Z;
        return result;
    }
    Vec3 Vec3::operator+=(const Vec3& other)
    {
        Vec3 result;
        result.X = X + other.X;
        result.Y = Y + other.Y;
        result.Z = Z + other.Z;
        return result;
    }
    Vec3 operator/(const float divider)
    {
        Vec3 result;
        result.X = X / divider;
        result.Y = Y / divider;
        result.Z = Z / divider;
        return result;
    }
    Vec3 operator/=(const Vec3& other)
    {
        Vec3 result;
        result.X = X / other.X;
        result.Y = Y / other.Y;
        result.Z = Z / other.Z;
        return result;
    }
    Vec3 operator/=(const float divider)
    {
        Vec3 result;
        result.X = X / divider;
        result.Y = Y / divider;
        result.Z = Z / divider;
        return result;
    }
    Vec3 operator*(const float scaleFactor)
    {
        Vec3 result;
        result.X = X * scaleFactor;
        result.Y = Y * scaleFactor;
        result.Z = Z * scaleFactor;
        return result;
    }
    Vec3 operator*=(const float scaleFactor)
    {
        Vec3 result;
        result.X = X * scaleFactor;
        result.Y = Y * scaleFactor;
        result.Z = Z * scaleFactor;
        return result;
    }
    Vec3 operator*=(const Vec3& other)
    {
        Vec3 result;
        result.X = X * other.X;
        result.Y = Y * other.Y;
        result.Z = Z * other.Z;
        return result;
    }
    Vec3 operator*(const Vec3& other)
    {
        Vec3 result;
        result.X = X * other.X;
        result.Y = Y * other.Y;
        result.Z = Z * other.Z;
        return result;
    }
   static Vec3 Cross(Vec3 vector1, Vec3 vector2)
    {
        Vec3 result;
        result.X = vector1.Y * vector2.Z - vector1.Z * vector2.Y;
        result.Y = vector1.Z * vector2.X - vector1.X * vector2.Z;
        result.Z = vector1.X * vector2.Y - vector1.Y * vector2.X;
        return result;
    }

   static float Vec3::Dot(Vec3 value1, Vec3 value2)
   {
       return (value1.X * value2.X + value1.Y * value2.Y + value1.Z * value2.Z);
   }

};
//struct Ray
//{
//    Vec3 Position;
//    Vec3 Direction;
//};


struct Plane
{
    Vec3 Normal;
    float D;
};

struct Mat4x4
{
    float M11, M12, M13, M14,
        M21, M22, M23, M24,
        M31, M32, M33, M34,
        M41, M42, M43, M44;
};
#include <DirectXCollision.h>

ContainmentType TestAABBPlane(XMFLOAT4* p, CVector min, CVector max);
