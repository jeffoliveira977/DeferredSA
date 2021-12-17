#include "Water.h"
#include "EnvironmentMapping.h"
#include "GTADef.h"
#include "CScene.h"
#include "CTimeCycle.h"
#include "DeferredRenderer.h"
#include "plugin.h"
#include "VertexShader.h"
#include "PixelShader.h"

using namespace plugin;

RwRaster* CWaterLevel::m_reflection = NULL;
RwRaster* CWaterLevel::m_refraction = NULL;
 RwFrame* CWaterLevel::m_envFrame = NULL;
 RwCamera* CWaterLevel::m_envCamera = NULL;

RwTexture* g_pWaveCubeDDS = 0;
RwTexture* Wavemap = 0;

RwTexture* WaveMapS0 = 0;
RwTexture* WaveMapS1 = 0;


RwTexture* Waterbump = 0;
RwTexture* Waves2 = 0;

VertexShader* pWaterVSCode;
PixelShader* pWaterPSCode;
void* pWaterPS;
void* pWaterVS;
 WaterReflection ObjectType;
RwTexture* Waves = 0;
RwTexture* Foam = 0;
RwTexture* Water_NormalW = 0;
RwTexture* Water_NormalN = 0;
RwTexture* CloudyHillsCubemap2 = 0;
RwRaster* CWaterLevel::m_zRaster;


#define CWaterManager__BuildWaterIndex()((void (__cdecl *)())0x006E7B30)()
#include "CTxdStore.h"

#define RwReadTexture(name, mask) ( (RwTexture * (__cdecl *)(const RwChar*,const RwChar*))0x007F3AC0 )(name, mask)

UINT& m_nNumOfWaterTriangles = *(UINT*)0xC22884;
UINT& m_nNumOfWaterQuads = *(UINT*)0xC22888;

RwRaster* waterclear256Raster = (RwRaster*)0xC228A8;
RwTexture* texWaterclear256 = (RwTexture*)0xC228AC;
RwRaster* seabd32Raster = (RwRaster*)0xC228B0;
RwTexture* texSeabd32 = (RwTexture*)0xC228B4;
RwRaster* waterwakeRaster = (RwRaster*)0xC228B8;
RwTexture* texWaterwake = (RwTexture*)0xC228BC;

uint32_t& NumWaterVertices = *(uint32_t*)0xC2288C;
uint32_t& NumWaterZonePolys = *(uint32_t*)0xC215F0;
uint32_t *&WaterZones = *(uint32_t**)0x00C21B7;

uint32_t& m_nWaterConfiguration = *(uint32_t*)0xC228A0;

#include "CFileLoader.h"

VertexBuffer* CWaterLevel::mVertexBuffer = nullptr;
RwIndexBuffer* CWaterLevel::mIndexBuffer = nullptr;

void CWaterLevel::InitShaders()
{
    pWaterVSCode = new VertexShader();
    pWaterVSCode->CreateFromFile("WaterEffects", "WaterVS");

    pWaterPSCode = new PixelShader();
    pWaterPSCode->CreateFromFile("WaterEffects", "WaterPS");

    mVertexBuffer = new VertexBuffer();
    mVertexBuffer->Initialize(TOTAL_TEMP_BUFFER_VERTICES, sizeof(RwIm3DVertex));

    mIndexBuffer = new RwIndexBuffer();
    mIndexBuffer->Initialize(TOTAL_TEMP_BUFFER_INDICES);

    // g_pWaveCubeDDS = RwD3D9DDSTextureRead("DeferredSA/PuddlesRelief");
    //  Wavemap = LoadBMPTextureFromFile("DeferredSA/Wave.bmp");
    Wavemap = LoadTextureFromFile("DeferredSA/textures/Water_N_1.png");
    //Wavemap = LoadTextureFromFile("DeferredSA/wavemap.png");
    Foam = LoadTextureFromFile("DeferredSA/textures/voronoise_3.png");
    Water_NormalW = LoadTextureFromFile("DeferredSA/textures/WaterFoam.png");
    //Wavemap = RwD3D9DDSTextureRead("DeferredSA/wave1");
    //Wavemap = RwD3D9DDSTextureRead("DeferredSA/waterbump");
     //Wavemap = RwD3D9DDSTextureRead("DeferredSA/waves2");

    int width, height;
    width = 1024;
    height = 1024;

    m_reflection = RwRasterCreate(1024, 1024, 32, rwRASTERTYPECAMERATEXTURE);
    m_refraction = RwRasterCreate(1024, 1024, 32, rwRASTERTYPECAMERATEXTURE);
    m_zRaster = RwRasterCreate(1024, 1024, 32, rwRASTERTYPEZBUFFER);

    m_envCamera = RwCameraCreate();
    RwCameraSetProjection(m_envCamera, RwCameraProjection::rwPERSPECTIVE);

    RwCameraSetNearClipPlane(m_envCamera, 0.1f);
    RwCameraSetFarClipPlane(m_envCamera, 400);
    RwCameraSetRaster(m_envCamera, m_reflection);
    RwCameraSetZRaster(m_envCamera, m_zRaster);

    m_envFrame = RwFrameCreate();
    RwCameraSetFrame(m_envCamera, m_envFrame);
    RpWorldAddCamera(Scene.m_pRpWorld, m_envCamera);
}

void CWaterLevel::UpdateTextures()
{
 //   int width, height;
 //   width = 1024;
 //   height = 1024;

 //   m_reflection->width = width;
 //   m_reflection->height = height;

 //   m_refraction->width = width;
 //   m_refraction->height = height;

 //   m_zRaster->width = width;
 //   m_zRaster->height = height;

}

void CWaterLevel::UpdateImgui()
{
    if(ImGui::BeginTabItem("Water"))
    {
        ImGui::EndTabItem();
        ImGui::Columns(2, nullptr, false); // You set 2 columns

        ImGui::SetColumnOffset(1, 200);
        static bool Reflcetion;
        static bool Refraction;
      
        ImGui::Checkbox("Render reflection", &Reflcetion);
        ImGui::NextColumn();
     
        ImGui::Checkbox("Render refraction", &Refraction); ImGui::NextColumn();
        ImGui::Separator();
        ImGui::Text("Reflection/refraction objects");
        ImGui::Checkbox("Render Buldings", &ObjectType.buldings);
        ImGui::Checkbox("Render Vehicles", &ObjectType.vehicles);
        ImGui::Checkbox("Render Peds", &ObjectType.peds);
        ImGui::Checkbox("Render Objects", &ObjectType.objects);

        static float f = 0.02;
        static int Specular;
        static float color[3];
        static bool enableWaterCaustics;
        static bool enableFoam;
        static float WaterCausticsSize;
        static int TextureSize[] = {1920, 1080};
        ImGui::SliderFloat("Bump normal", &f, 0.001f, 3.0f);
        ImGui::SliderInt("Specular intensity", &Specular, 0, 500);
        ImGui::ColorEdit3("Water Color", (float*)&color);
        ImGui::ColorEdit3("Deepth Water Color", (float*)&color);
        ImGui::Checkbox("Render caustics", &enableWaterCaustics);
        if(enableWaterCaustics)
            ImGui::SliderFloat("Caustics size", &WaterCausticsSize, 1.0, 20.0);

        ImGui::Checkbox("Render Foam", &enableFoam);

        int a = TextureSize[0];
        int b = TextureSize[1];
        ImGui::SliderInt2("Texture size", TextureSize, 128, 4096);
    }
}

#include <DirectXMath.h>
#include "ShaderManager.h"
#include "PixelShaderConstant.h"
#include "CascadedShadowRendering.h"
void CWaterLevel::SetupWaterShader()
{
   // int& waterdis = *(int*)0x08D37D0;
    //waterdis = 100;

    ShaderContext->SetViewProjectionMatrix(4, true);
    ShaderContext->SetInverseViewMatrix(4, false);
    ShaderContext->SetProjectionMatrix(8, false);



    float ti = (float)CTimer__m_snTimeInMilliseconds;
    ti = ti / 1000.0;
    _rwD3D9SetVertexShaderConstant(12, &ti, 1);
    _rwD3D9SetPixelShaderConstant(12, &ti, 1);

    float mSettings[4];
    mSettings[0] = CTimeCycle::m_CurrentColours.m_fFogStart;
    mSettings[1] = CTimeCycle::m_CurrentColours.m_fFarClip;
    mSettings[2] = RsGlobal.maximumWidth;
    mSettings[3] = RsGlobal.maximumHeight;
    _rwD3D9SetPixelShaderConstant(13, &mSettings, 1);

    RwUInt32 color;
    RwRenderStateGet(rwRENDERSTATEFOGCOLOR, &color);

    RwRGBAReal fogColor;
    RwLongToRGBAReal(&fogColor, color);
    _rwD3D9SetPixelShaderConstant(14, &fogColor, 1);

    RwRGBAReal waterColor = {CTimeCycle::m_CurrentColours.m_fWaterRed / 255.0f,
                               CTimeCycle::m_CurrentColours.m_fWaterGreen / 255.0f,
                                CTimeCycle::m_CurrentColours.m_fWaterBlue / 255.0f,
                                CTimeCycle::m_CurrentColours.m_fWaterAlpha / 255.0f};


    PixelShaderConstant::SetData(15, &waterColor, 1);
    ShaderContext->SetSkyColor(16);
    ShaderContext->SetSunColor(18);
    ShaderContext->SetSunDirection(19);

   // PrintMessage("%f %f", screenSize[0], screenSize[1]);
    for(size_t i = 0; i < CascadedShadowManagement->CascadeCount; i++)
    {
        rwD3D9SetSamplerState(i + 6, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        rwD3D9SetSamplerState(i + 6, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        rwD3D9SetSamplerState(i + 6, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
        rwD3D9SetSamplerState(i + 6, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        rwD3D9SetSamplerState(i + 6, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
        rwD3D9SetSamplerState(i + 6, D3DSAMP_BORDERCOLOR, 0xFFFFFFFF);

        rwD3D9RWSetRasterStage(CascadedShadowManagement->mColorRaster[i], i + 6);
    }

    _rwD3D9SetPixelShaderConstant(20, &CascadedShadowManagement->mConstantBuffer,
                                  sizeof(CascadedShadowManagement->mConstantBuffer) / sizeof(XMVECTOR));

    CWaterLevel__RenderWater();
}

#include "CRenderer.h"
#include "CWorld.h"
#include "Renderer.h"
#include "ShaderManager.h"

#include "CVisibilityPlugins.h"
#include "CObjectInfo.h"

using tAlphaRenderOrderedListCB = void(__cdecl*)(CEntity * entity, float distance);
CLinkList<CVisibilityPlugins::AlphaObjectInfo>& m_alphaList = *(CLinkList<CVisibilityPlugins::AlphaObjectInfo>*)0xC88070;
CLinkList<CVisibilityPlugins::AlphaObjectInfo>& m_alphaBoatAtomicList = *(CLinkList<CVisibilityPlugins::AlphaObjectInfo>*)0xC880C8;
CLinkList<CVisibilityPlugins::AlphaObjectInfo>& m_alphaEntityList = *(CLinkList<CVisibilityPlugins::AlphaObjectInfo>*)0xC88120;
CLinkList<CVisibilityPlugins::AlphaObjectInfo>& m_alphaUnderwaterEntityList = *(CLinkList<CVisibilityPlugins::AlphaObjectInfo>*)0xC88178;
CLinkList<CVisibilityPlugins::AlphaObjectInfo>& m_alphaReallyDrawLastList = *(CLinkList<CVisibilityPlugins::AlphaObjectInfo>*)0xC881D0;

void CVisibilityPlugins__RenderFadingUnderwaterEntities()
{
    for(auto i = m_alphaUnderwaterEntityList.usedListTail.prev; i != &m_alphaUnderwaterEntityList.usedListHead; i = i->prev)
    {
        auto type = ((CEntity*)i->data.m_atomic)->m_nType;
        if(type == ENTITY_TYPE_PED && ObjectType.peds ||
           type == ENTITY_TYPE_VEHICLE && ObjectType.vehicles||
           type == ENTITY_TYPE_BUILDING && ObjectType.buldings ||
           type == ENTITY_TYPE_OBJECT && ObjectType.objects)
        {
            auto callBack = reinterpret_cast<tAlphaRenderOrderedListCB>(i->data.m_pCallback);
            callBack((CEntity*)i->data.m_atomic, i->data.m_distance);
        }
    }
}

void CVisibilityPlugins__RenderFadingInBuildings(void)
{
    for(auto i = m_alphaEntityList.usedListTail.prev; i != &m_alphaEntityList.usedListHead; i = i->prev)
    {
        auto type = ((CEntity*)i->data.m_atomic)->m_nType;
        if(type == ENTITY_TYPE_PED && ObjectType.peds ||
           type == ENTITY_TYPE_VEHICLE && ObjectType.vehicles ||
           type == ENTITY_TYPE_BUILDING && ObjectType.buldings ||
           type == ENTITY_TYPE_OBJECT && ObjectType.objects)
        {
            auto callBack = reinterpret_cast<tAlphaRenderOrderedListCB>(i->data.m_pCallback);
            callBack((CEntity*)i->data.m_atomic, i->data.m_distance);
        }
    }
}

std::vector<CEntity*>CWaterLevel::m_renderableList;
void CRenderer_RenderRoadsAndBuildings(void)
{
    ObjectType.buldings = true;
    ObjectType.vehicles = true;
    ObjectType.objects = true;
    ObjectType.peds = true;
    
    DefinedState();

    CClouds__Render();
    CCoronas__RenderReflections();
    CCoronas__Render();
    CClouds__RenderBottomFromHeight();
    CWeather__RenderRainStreaks();
    CCoronas__RenderSunReflection();

    RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)2);

    for(int i = 0; i < CRenderer::ms_nNoOfVisibleEntities; i++)
    {
        CEntity* e = CRenderer::ms_aVisibleEntityPtrs[i];
        int type = e->m_nType;
        if((type == ENTITY_TYPE_PED && ObjectType.peds) ||
            (type == ENTITY_TYPE_VEHICLE && ObjectType.vehicles) ||
           (type == ENTITY_TYPE_BUILDING && ObjectType.buldings) ||
           (type == ENTITY_TYPE_OBJECT && ObjectType.objects))
        {
            //if(reinterpret_cast<CBaseModelInfo*>(CModelInfo::ms_modelInfoPtrs[e->m_nModelIndex])->nSpecialType)
            // CRenderer__RenderOneRoad(e);
            //else
            CRenderer::RenderOneNonRoad(e);
        }
    }

    for(int i = 0; i < CRenderer::ms_nNoOfVisibleLods; i++)
    {
        CEntity* e = CRenderer::ms_aVisibleLodPtrs[i];
        int type = e->m_nType;
        if((type == ENTITY_TYPE_PED && ObjectType.peds) ||
            (type == ENTITY_TYPE_VEHICLE && ObjectType.vehicles) ||
           (type == ENTITY_TYPE_BUILDING && ObjectType.buldings) ||
           (type == ENTITY_TYPE_OBJECT && ObjectType.objects))
        {
            CRenderer::RenderOneNonRoad(CRenderer::ms_aVisibleLodPtrs[i]);
        }
    }

    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)3);

    CVisibilityPlugins__RenderFadingInBuildings();
    CVisibilityPlugins__RenderFadingUnderwaterEntities();

    CVisibilityPlugins__RenderWeaponPedsForPC();
}

void CWaterLevel::RenderReflection(RenderCallback renderCallback)
{

    RwRGBA ambient = {CTimeCycle::m_CurrentColours.m_nSkyTopRed, CTimeCycle::m_CurrentColours.m_nSkyTopGreen, CTimeCycle::m_CurrentColours.m_nSkyTopBlue, 255};

    RwCamera *camera = Scene.m_pRwCamera;
    RwCameraSetViewWindow(m_envCamera, &camera->viewWindow);
    RwFrameTransform(m_envFrame, &RwCameraGetFrame(camera)->ltm, rwCOMBINEREPLACE);
    RwCameraSetZRaster(m_envCamera, m_zRaster);

    // Render refraction
    gRenderState = stageRefraction;    
    RwCameraSetRaster(m_envCamera, m_refraction);
    RwCameraClear(m_envCamera, &ambient, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
    RwCameraBeginUpdate(m_envCamera);
    ShaderContext->SetViewProjectionMatrix(4, true);
    CRenderer_RenderRoadsAndBuildings();
    RwCameraEndUpdate(m_envCamera);

    // Render reflection
    gRenderState = stageReflection;
    RwCameraSetRaster(m_envCamera, m_reflection);
    RwCameraClear(m_envCamera, &ambient, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
    RwCameraBeginUpdate(m_envCamera);
    ShaderContext->SetViewProjectionMatrix(4, true);
    CRenderer_RenderRoadsAndBuildings();
    RwCameraEndUpdate(m_envCamera);
}

void DrawWater()
{
    RwInt32 numIndices = uiTempBufferIndicesStored;
    RwInt32 numVerts = uiTempBufferVerticesStored;

    _rwD3D9SetVertexDeclaration(VertexDeclIm3DOld);

    RwInt32 stride = sizeof(RwIm3DVertex);

    RwIm3DVertex* bufferMem = nullptr;
    CWaterLevel::mVertexBuffer->Map(numVerts * stride, (void**)&bufferMem);
    std::copy(aTempBufferVertices, aTempBufferVertices + numVerts, bufferMem);
    CWaterLevel::mVertexBuffer->Unmap();

    RwImVertexIndex* indexBuffer = nullptr;
    CWaterLevel::mIndexBuffer->Map(numIndices * sizeof(RwImVertexIndex), (void**)&indexBuffer);
    std::copy(aTempBufferIndices, aTempBufferIndices + numIndices, indexBuffer);
    CWaterLevel::mIndexBuffer->Unmap();

    RwD3D9SetStreamSource(0, CWaterLevel::mVertexBuffer->GetBuffer(), 0, stride);
    _rwD3D9SetIndices(CWaterLevel::mIndexBuffer->GetBuffer());

    _rwD3D9SetVertexShader(pWaterVSCode->GetObject());
    _rwD3D9SetPixelShader(pWaterPSCode->GetObject());

    //RwD3D9SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    //RwD3D9SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    //RwD3D9SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    //RwD3D9SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
    //RwD3D9SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
    //RwD3D9SetSamplerState(2, D3DSAMP_MAXANISOTROPY, 16);
    //rwD3D9RWSetRasterStage(CWaterLevel::m_reflection, 2);

    //RwD3D9SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    //RwD3D9SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    //RwD3D9SetSamplerState(3, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    //RwD3D9SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
    //RwD3D9SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
    //RwD3D9SetSamplerState(3, D3DSAMP_MAXANISOTROPY, 16);
    //rwD3D9RWSetRasterStage(CWaterLevel::m_refraction, 3);


    RwD3D9SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    RwD3D9SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    RwD3D9SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    RwD3D9SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    RwD3D9SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    RwD3D9SetSamplerState(1, D3DSAMP_MAXANISOTROPY, 16);
    RwD3D9SetTexture(Wavemap, 1);

    RwD3D9SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    RwD3D9SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    RwD3D9SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    RwD3D9SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
    RwD3D9SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
    RwD3D9SetSamplerState(2, D3DSAMP_MAXANISOTROPY, 16);
    _rwD3D9RWSetRasterStage(CWaterLevel::m_reflection, 2);

    RwD3D9SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    RwD3D9SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    RwD3D9SetSamplerState(3, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    RwD3D9SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
    RwD3D9SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
    RwD3D9SetSamplerState(3, D3DSAMP_MAXANISOTROPY, 16);
    _rwD3D9RWSetRasterStage(CWaterLevel::m_refraction, 3);

    RwD3D9SetSamplerState(4, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    RwD3D9SetSamplerState(4, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    RwD3D9SetSamplerState(4, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    RwD3D9SetSamplerState(4, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    RwD3D9SetSamplerState(4, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    RwD3D9SetSamplerState(4, D3DSAMP_MAXANISOTROPY, 16);
    _rwD3D9RWSetRasterStage(DeferredContext->mGraphicsBuffer[1]->GetRaster(), 4);

    RwD3D9SetSamplerState(5, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    RwD3D9SetSamplerState(5, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    RwD3D9SetSamplerState(5, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    RwD3D9SetSamplerState(5, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    RwD3D9SetSamplerState(5, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    RwD3D9SetSamplerState(5, D3DSAMP_MAXANISOTROPY, 16);
    RwD3D9SetTexture(Water_NormalW, 5);

    RwInt32 numPrimitives = numIndices / 3;
    _rwD3D9DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, numVerts, 0, numPrimitives);


    RwD3D9SetTexture(NULL, 1);
    RwD3D9SetTexture(NULL, 2);
}

void CWaterLevel::Hook()
{
    patch::Nop(0x006F0071, 0x30);
    patch::RedirectCall(0x006F0099, DrawWater);
    patch::Nop(0x006EA2BE, 0x32);
    patch::RedirectCall(0x006EA2E8, DrawWater);
    patch::Nop(0x006E7691, 0x30);
    patch::RedirectCall(0x006E76B9, DrawWater);
}