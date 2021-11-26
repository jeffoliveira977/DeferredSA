#include "Water.h"
#include "EnvironmentMapping.h"
#include "GTADef.h"
#include "CScene.h"
#include "CTimeCycle.h"
#include "DeferredRenderer.h"
#include "plugin.h"
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

void* pWaterVSCode;
void* pWaterPSCode;
void* pWaterPS;
void* pWaterVS;
 WaterReflection ObjectType;
RwTexture* Waves = 0;
RwTexture* Foam = 0;
RwTexture* Water_NormalW = 0;
RwTexture* Water_NormalN = 0;
RwTexture* CloudyHillsCubemap2 = 0;
RwRaster* CWaterLevel::m_zRaster;
void CWaterLevel::InitShaders()
{
    pWaterVSCode = CreateVertexShader("DeferredSA/shaders/WaterEffects.hlsl", "WaterVS");
   
  //  pWaterVSCode = CreateVertexShader("DeferredSA/water.hlsl", "VertexShaderFunction");
    //pWaterVSCode = CreateVertexShader("DeferredSA/water_ref.hlsl", "VertexShaderSB");
   // pWaterVSCode = CreateVertexShader("DeferredSA/Ocean.hlsl", "OceanVS");
   // pWaterPSCode = CreatePixelShader("DeferredSA/water.hlsl", "PixelShaderFunction");
  //  pWaterPSCode = CreatePixelShader("DeferredSA/water_ref.hlsl", "PixelShaderSB");
 //  pWaterPSCode = CreatePixelShader("DeferredSA/Ocean.hlsl", "OceanPS");
     pWaterPSCode = CreatePixelShader("DeferredSA/shaders/WaterEffects.hlsl", "WaterPS");

     

  //  g_pWaveCubeDDS = RwD3D9DDSTextureRead("DeferredSA/PuddlesRelief");
  //   Wavemap = LoadBMPTextureFromFile("DeferredSA/Wave.bmp");
   Wavemap = LoadTextureFromFile("DeferredSA/textures/Water_N_1.png");
    //Wavemap = LoadTextureFromFile("DeferredSA/wavemap.png");
    Foam = LoadTextureFromFile("DeferredSA/textures/voronoise_3.png");
    Water_NormalW = LoadTextureFromFile("DeferredSA/textures/WaterFoam.png");
   // Wavemap = RwD3D9DDSTextureRead("DeferredSA/wave1");
   // Wavemap = RwD3D9DDSTextureRead("DeferredSA/waterbump");
    //  Wavemap = RwD3D9DDSTextureRead("DeferredSA/waves2");

    m_reflection = RwRasterCreate(1920, 1080, 32, rwRASTERTYPECAMERATEXTURE);
    m_refraction = RwRasterCreate(1920, 1080, 32, rwRASTERTYPECAMERATEXTURE);
    m_zRaster = RwRasterCreate(1920, 1080, 32, rwRASTERTYPEZBUFFER);

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

void CWaterLevel::imguiParameters()
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

void CWaterLevel::SetupWaterShader()
{
    int& waterdis = *(int*)0x08D37D0;
    waterdis = 100;

   


    DirectX::XMMATRIX transformTransposedMatrix;
    DirectX::XMMATRIX worldMatrix;

    D3DXMATRIX world;
    D3DXMatrixIdentity(&world);

    D3DXMATRIX worldViewProj, proj, worldInverse, view, viewInverse;
    RwD3D9GetTransform(D3DTS_PROJECTION, &proj);
    RwD3D9GetTransform(D3DTS_VIEW, &view);

    _rwD3D9SetVertexShaderConstant(0, &view, 4);
    _rwD3D9SetVertexShaderConstant(4, &proj, 4);
  
    D3DXMatrixInverse(&viewInverse, 0, &view);
    _rwD3D9SetPixelShaderConstant(0, &viewInverse, 4);
    _rwD3D9SetPixelShaderConstant(4, &proj, 4);

    auto ti = (float)CTimer__m_snTimeInMilliseconds;
    ti = ti / 1000.0;
    _rwD3D9SetVertexShaderConstant(8, &ti, 1);
    _rwD3D9SetPixelShaderConstant(8, &ti, 1);

    CVector* sunDirs = (CVector*)0xB7CA50;
    int sunDirIndex = *(int*)0xB79FD0;
    CVector curSunDir = sunDirs[sunDirIndex];
    _rwD3D9SetPixelShaderConstant(9, &curSunDir, 1);

    float fog[4];
    fog[0] = CTimeCycle::m_CurrentColours.m_fFogStart;
    fog[1] = CTimeCycle::m_CurrentColours.m_fFarClip;
    _rwD3D9SetPixelShaderConstant(10, &fog, 1);

    RwUInt32 color;
    RwRenderStateGet(rwRENDERSTATEFOGCOLOR, &color);

    RwRGBAReal fogColor;
    RwLongToRGBAReal(&fogColor, color);
    _rwD3D9SetPixelShaderConstant(11, &fogColor, 1);

    RwRGBAReal waterColor = {CTimeCycle::m_CurrentColours.m_fWaterRed / 255.0f,
                               CTimeCycle::m_CurrentColours.m_fWaterGreen / 255.0f,
                                CTimeCycle::m_CurrentColours.m_fWaterBlue / 255.0f,
                                CTimeCycle::m_CurrentColours.m_fWaterAlpha / 255.0f};

    _rwD3D9SetPixelShaderConstant(12, &waterColor, 1);
    _rwD3D9SetPixelShaderConstant(13, &GetSunColor(), 1);
    _rwD3D9SetPixelShaderConstant(14, &GetSkyTopColor(), 1);
    _rwD3D9SetPixelShaderConstant(15, &GetSkyBottomColor(), 1);

    CWaterLevel__RenderWater();
}


//void CWaterLevel::SetupWaterShader()
//{
//    int& waterdis = *(int*)0x08D37D0;
//    waterdis = 500;
//
//    DirectX::XMMATRIX transformTransposedMatrix;
//    DirectX::XMMATRIX worldMatrix;
//
//    D3DXMATRIX world;
//    D3DXMatrixIdentity(&world);
//
//    D3DXMATRIX worldViewProj, proj, worldInverse, view, viewInverse;
//    RwD3D9GetTransform(D3DTS_PROJECTION, &proj);
//    RwD3D9GetTransform(D3DTS_VIEW, &view);
//
//    RwD3D9SetVertexShaderConstant(0, &world, 4);
//    RwD3D9SetVertexShaderConstant(4, &view, 4);
//    RwD3D9SetVertexShaderConstant(8, &proj, 4);
//
//    D3DXMatrixInverse(&viewInverse, 0, &view);
//    RwD3D9SetVertexShaderConstant(12, &viewInverse, 4);
//
//    auto time = GetTickCount64_() * 0.001f;
//    RwD3D9SetVertexShaderConstant(16, &time, 1);
//
//    CVector* sunDirs = (CVector*)0xB7CA50;
//    int sunDirIndex = *(int*)0xB79FD0;
//    CVector curSunDir = sunDirs[sunDirIndex];
//    RwD3D9SetVertexShaderConstant(17, &curSunDir, 1);
//
//    CWaterLevel__RenderWater();
//}
#include "ShaderManager.h"

void CWaterLevel::RenderReflection(RenderCallback renderCallback)
{
    if(renderCallback == NULL)
        return;

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
    renderCallback(); 
    RwCameraEndUpdate(m_envCamera);

    // Render reflection
    gRenderState = stageReflection;
    RwCameraSetRaster(m_envCamera, m_reflection);
    RwCameraClear(m_envCamera, &ambient, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
    RwCameraBeginUpdate(m_envCamera);
    ShaderContext->SetViewProjectionMatrix(4, true);
    renderCallback();
    RwCameraEndUpdate(m_envCamera);
}

void DrawWater()
{
    RwInt32 numIndices = TempBufferIndicesStored;
    RwInt32 numVerts = TempBufferVerticesStored;

    RwIm3DVertex* verts = TempVertexBuffer;
    RwImVertexIndex* indices = TempBufferRenderIndexList;

    numIndices -= (numIndices % 2);

    void* bufferMem = NULL;

    HRESULT hr;

    _rwD3D9SetVertexDeclaration(VertexDeclIm3D);

    RwInt32 stride = sizeof(RxD3D9Im3DVertex);

    if(RwD3D9DynamicVertexBufferLock(stride, numVerts, (void**)&CurrentVertexBuffer3D, &bufferMem, &CurrentBaseIndex3D))
    {
        RxD3D9Im3DVertex* desVerts = (RxD3D9Im3DVertex*)bufferMem;
        RwUInt32 i = numVerts;
        do
        {
            desVerts->x = verts->objVertex.x;
            desVerts->y = verts->objVertex.y;
            desVerts->z = verts->objVertex.z;
            desVerts->color = verts->color;
            desVerts->u = verts->u;
            desVerts->v = verts->v;

            desVerts++;
            verts++;
            i--;
        }
        while(i);

        RwD3D9DynamicVertexBufferUnlock(CurrentVertexBuffer3D);

        /* Fill the Index Buffer */
        if((IB3DOffset + numIndices) > BUFFER_MAX_INDEX)
        {
            IB3DOffset = 0;

            hr = IDirect3DIndexBuffer9_Lock(IndexBuffer3D, 0,
                                            numIndices * sizeof(RwImVertexIndex),
                                            &bufferMem,
                                            D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
        }
        else
        {
            hr = IDirect3DIndexBuffer9_Lock(IndexBuffer3D,
                                            IB3DOffset * sizeof(RwImVertexIndex),
                                            numIndices * sizeof(RwImVertexIndex),
                                            &bufferMem,
                                            D3DLOCK_NOOVERWRITE | D3DLOCK_NOSYSLOCK);
        }

        if(SUCCEEDED(hr))
        {
            memcpy(bufferMem, indices, numIndices * sizeof(RwImVertexIndex));
            IDirect3DIndexBuffer9_Unlock(IndexBuffer3D);

            RwD3D9SetStreamSource(0, CurrentVertexBuffer3D, 0, stride);
            _rwD3D9SetIndices(IndexBuffer3D);

            _rwD3D9SetVertexShader(pWaterVSCode);
            _rwD3D9SetPixelShader(pWaterPSCode);

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
            RwD3D9SetSamplerState(4, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
            RwD3D9SetSamplerState(4, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
            RwD3D9SetSamplerState(4, D3DSAMP_MAXANISOTROPY, 16);
            RwD3D9SetTexture(Foam, 4);

            RwD3D9SetSamplerState(5, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            RwD3D9SetSamplerState(5, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            RwD3D9SetSamplerState(5, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
            RwD3D9SetSamplerState(5, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
            RwD3D9SetSamplerState(5, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
            RwD3D9SetSamplerState(5, D3DSAMP_MAXANISOTROPY, 16);
            _rwD3D9RWSetRasterStage(DeferredContext->m_graphicsBuffer[1], 5);

            RwD3D9SetSamplerState(6, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            RwD3D9SetSamplerState(6, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            RwD3D9SetSamplerState(6, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
            RwD3D9SetSamplerState(6, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
            RwD3D9SetSamplerState(6, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
            RwD3D9SetSamplerState(6, D3DSAMP_MAXANISOTROPY, 16);
            RwD3D9SetTexture(Water_NormalW, 6);

            RwInt32 numPrimitives = numIndices / 3;
            _rwD3D9DrawIndexedPrimitive(D3DPT_TRIANGLELIST, CurrentBaseIndex3D, 0, numVerts, IB3DOffset, numPrimitives);

            IB3DOffset += numIndices;

            RwD3D9SetTexture(NULL, 1);
            RwD3D9SetTexture(NULL, 2);
        }
    }
}

void CWaterLevel::Hook()
{
    patch::Nop(0x006F0071, 0x30);
    patch::RedirectCall(0x006F0099, DrawWater);
    patch::Nop(0x006EA2BE, 0x32);
    patch::RedirectCall(0x006EA2E8, DrawWater);
 /*   CPatch::Nop(0x006EF884, 0x30);
    CPatch::RedirectCall(0x006EF8AC, DrawWater);*/
    patch::Nop(0x006E7691, 0x30);
    patch::RedirectCall(0x006E76B9, DrawWater);

    //Nop(0x006EDD53, 0x31);
    //InjectHook(0x006EDD7C, DrawWater, PATCH_CALL);
}