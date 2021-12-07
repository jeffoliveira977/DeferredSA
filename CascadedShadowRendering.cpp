#include "CascadedShadowRendering.h"
#include "CScene.h"
#include "GTADef.h"
#include "RwMath.h"
#include "CTimeCycle.h"
#include "DeferredRenderer.h"
#include "Renderer.h"
#include "CGameIdle.h"
#include "ShaderManager.h"
#include "ShadowCaster.h"
CascadedShadowRendering* CascadedShadowManagement;
char cPath[MAX_PATH];

CascadedShadowRendering::CascadedShadowRendering()
{
    ShadowSize = 1024 * 2;
    CascadeCount = 3;
    FilterSize = 40.0f;
     MinDistance = 2.0;
     MaxDistance = 100.0;
     PSSMLambda = 0.05f;

    for(size_t i = 0; i < 4; i++)
    {
        m_shadowColorRaster[i] = nullptr;

        m_pShadowCamera[i] = nullptr;
    }
    m_shadowDepthRaster = nullptr;

    char cStr[256];
    //char cPath[MAX_PATH];
    GetModuleFileName(NULL, cPath, MAX_PATH);

    if(strrchr(cPath, '\\'))
        *(char*)(strrchr(cPath, '\\') + 1) = '\0';

    strcat_s(cPath, MAX_PATH, "DeferredConfig.ini");

    GetPrivateProfileString("SHADOW", "DistanceCoefficients0", "0.01", cStr, 256, cPath);
    DistanceCoefficients[0] = (float)atof(cStr);
    GetPrivateProfileString("SHADOW", "DistanceCoefficients1", "0.01", cStr, 256, cPath);
    DistanceCoefficients[1] = (float)atof(cStr);
    GetPrivateProfileString("SHADOW", "DistanceCoefficients2", "0.01", cStr, 256, cPath);
    DistanceCoefficients[2] = (float)atof(cStr);
    GetPrivateProfileString("SHADOW", "DistanceCoefficients3", "0.01", cStr, 256, cPath);
    DistanceCoefficients[3] = (float)atof(cStr);

    GetPrivateProfileString("SHADOW", "BiasCoefficients0", "0.01", cStr, 256, cPath);
    BiasCoefficients[0] = (float)atof(cStr);
    GetPrivateProfileString("SHADOW", "BiasCoefficients1", "0.01", cStr, 256, cPath);
    BiasCoefficients[1] = (float)atof(cStr);
    GetPrivateProfileString("SHADOW", "BiasCoefficients2", "0.01", cStr, 256, cPath);
    BiasCoefficients[2] = (float)atof(cStr);
    GetPrivateProfileString("SHADOW", "BiasCoefficients3", "0.01", cStr, 256, cPath);
    BiasCoefficients[3] = (float)atof(cStr);

   /* DistanceCoefficients[0] = 0.052529998f;
    DistanceCoefficients[1] = 0.16563001f;
    DistanceCoefficients[2] = 0.44854999f;
    DistanceCoefficients[3] = 0.64854999f;*/

    //BiasCoefficients[0] = 0.000208f;
    //BiasCoefficients[1] = 0.00013f;
    //BiasCoefficients[2] = 0.00171f;
    //BiasCoefficients[3] = 0.0049000001f;

    //for(int i = 0; i < 5; i++)
    //    m_fShadowDistances[i] = 0.0f;
}

CascadedShadowRendering::~CascadedShadowRendering()
{
    for(int i = 0; i < CascadeCount; i++)
    {
        RwRasterDestroy(m_shadowColorRaster[i]);
        RwCameraDestroy(m_pShadowCamera[i]);
    }
    RwRasterDestroy(m_shadowDepthRaster);
    //RpWorldRemoveCamera(Scene.m_pRpWorld, m_pShadowCamera);
   
}

void CascadedShadowRendering::initGraphics()
{
    m_shadowDepthRaster = RwD3D9RasterCreate(ShadowSize, ShadowSize, D3DFMT_D24S8, rwRASTERTYPEZBUFFER);

    for(int i = 0; i < CascadeCount; i++)
    {
        m_pShadowCamera[i] = RwCameraCreate();
        RwCameraSetProjection(m_pShadowCamera[i], rwPARALLEL);

        m_shadowColorRaster[i] = RwD3D9RasterCreate(ShadowSize, ShadowSize, D3DFMT_G32R32F, rwRASTERTYPECAMERATEXTURE);
        RwCameraSetFrame(m_pShadowCamera[i], RwFrameCreate());
    }

   // m_shadowColorRaster[0] = RwD3D9RasterCreate(ShadowSize*4, ShadowSize, D3DFMT_G32R32F, rwRASTERTYPECAMERATEXTURE);
   //m_shadowDepthRaster = RwD3D9RasterCreate(ShadowSize*4, ShadowSize, D3DFMT_D24S8, rwRASTERTYPEZBUFFER);

   
    //RpWorldAddCamera(Scene.m_pRpWorld, m_pShadowCamera);
}	

void CascadedShadowRendering::SetParamsBuffer()
{
    m_shadowBuffer.DistanceCoefficients = {m_fShadowDistances[0], m_fShadowDistances[1], m_fShadowDistances[2], m_fShadowDistances[3]};
    m_shadowBuffer.ShadowBias = {BiasCoefficients[0], BiasCoefficients[1], BiasCoefficients[2], BiasCoefficients[3]};
    m_shadowBuffer.ShadowParam = {(float)ShadowSize, m_fShadowDistances[4], (float)CascadeCount,
                                  1.0f - (CGame::currArea == 0 ? CGameIdle::m_fShadowDNBalance : 1.0f)};

    m_shadowBuffer.mShadowMatrix[0] = Desc[0].g_mLightViewProj;
    m_shadowBuffer.mShadowMatrix[1] = Desc[1].g_mLightViewProj;
    m_shadowBuffer.mShadowMatrix[2] = Desc[2].g_mLightViewProj;
    m_shadowBuffer.mShadowMatrix[3] = Desc[3].g_mLightViewProj;
}

void WriteIniFileFloat(float value, const char* key, const char* path)
{
    char buf[20];
    sprintf(buf, "%f", value);
    WritePrivateProfileString("SHADOW", key, buf, path);
}

#include "imgui.h"
void CascadedShadowRendering::imguiParameters()
{
    if(ImGui::BeginTabItem("Shadow"))
    {
        ImGuiSliderFlags flags = ImGuiSliderFlags_NoRoundToFormat;

        ImGui::EndTabItem();
        ImGui::InputFloat("Bias 0", &BiasCoefficients[0], 0.000001, 0.00001, "%.6f");
        ImGui::InputFloat("Bias 1", &BiasCoefficients[1], 0.000001, 0.00001, "%.6f");
        ImGui::InputFloat("Bias 2", &BiasCoefficients[2], 0.000001, 0.00001, "%.6f");
        ImGui::InputFloat("Bias 3", &BiasCoefficients[3], 0.000001, 0.00001, "%.6f");

        ImGui::InputFloat("DistanceCoefficients 0", &DistanceCoefficients[0], 0.0001, 0.001, "%.6f");
        ImGui::InputFloat("DistanceCoefficients 1", &DistanceCoefficients[1], 0.0001, 0.001, "%.6f");
        ImGui::InputFloat("DistanceCoefficients 2", &DistanceCoefficients[2], 0.0001, 0.001, "%.6f");
        ImGui::InputFloat("DistanceCoefficients 3", &DistanceCoefficients[3], 0.0001, 0.001, "%.6f");

        WriteIniFileFloat(BiasCoefficients[0], "BiasCoefficients0", cPath);
        WriteIniFileFloat(BiasCoefficients[1], "BiasCoefficients1", cPath);
        WriteIniFileFloat(BiasCoefficients[2], "BiasCoefficients2", cPath);
        WriteIniFileFloat(BiasCoefficients[3], "BiasCoefficients3", cPath);

        WriteIniFileFloat(DistanceCoefficients[0], "DistanceCoefficients0", cPath);
        WriteIniFileFloat(DistanceCoefficients[1], "DistanceCoefficients1", cPath);
        WriteIniFileFloat(DistanceCoefficients[2], "DistanceCoefficients2", cPath);
        WriteIniFileFloat(DistanceCoefficients[3], "DistanceCoefficients3", cPath);
    }
}


#include "CModelInfo.h"
void CascadedShadowRendering::RenderShadowToBuffer(int cascade, void(*render)(int cascade))
{

}

#include "CommonD.h"

void CascadedShadowRendering::Update()
{
    if(CGame::currArea != 0 || CGameIdle::m_fShadowDNBalance >= 1.0)
        return;

    gRenderState = stageCascadeShadow;

    RwRGBA ambient = {CTimeCycle::m_CurrentColours.m_nSkyTopRed, CTimeCycle::m_CurrentColours.m_nSkyTopGreen, CTimeCycle::m_CurrentColours.m_nSkyTopBlue, 255};

    // Render in red channel
    RwD3D9SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED);

    // Render cascades
    for(size_t i = 0; i < CascadeCount; i++)
    {
        RwCameraSetRaster(m_pShadowCamera[i], m_shadowColorRaster[i]);
        RwCameraSetZRaster(m_pShadowCamera[i], m_shadowDepthRaster);

        RwFrame* shadowCamFrame = RwCameraGetFrame(m_pShadowCamera[i]);
        RwMatrix* shadowCamMatrix = RwFrameGetMatrix(shadowCamFrame);
        
        m_LightSpaceMatrix.r[3] = XMLoadFloat3(&Desc[i].FrustumCenter);
        RwMatrixTransform(shadowCamMatrix, &XMMATRIXtoRwMatrix(m_LightSpaceMatrix), rwCOMBINEREPLACE);


        XMFLOAT3 extents;
       // XMStoreFloat3(&extents, (m_lightAABBMax[i] - m_lightAABBMin[i]));


        extents.x = Desc[i].m_AABB.Max.x - Desc[i].m_AABB.Min.x;
        extents.y = Desc[i].m_AABB.Max.y - Desc[i].m_AABB.Min.y;
        extents.z = Desc[i].m_AABB.Max.z - Desc[i].m_AABB.Min.z;

        RwV2d viewWindow;
        viewWindow.x = viewWindow.y = std::max(extents.x, extents.y);
        RwV2dScaleMacro(&viewWindow, &viewWindow, 0.5);
        RwCameraSetViewWindow(m_pShadowCamera[i], &viewWindow);

        Desc[i].NearClip = -(extents.z < 500.0 ? 500.0 : extents.z);
        Desc[i].FarClip = 50.0f + -Desc[i].NearClip;

        RwCameraSetNearClipPlane(m_pShadowCamera[i], Desc[i].NearClip);
        RwCameraSetFarClipPlane(m_pShadowCamera[i], Desc[i].FarClip);

        RwCameraSetProjection(m_pShadowCamera[i], rwPARALLEL);
        RwCameraClear(m_pShadowCamera[i], gColourTop, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
        RwCameraBeginUpdate(m_pShadowCamera[i]);

          RwD3D9SetTransform(D3DTS_VIEW, &Desc[i].lightViewMatrix);
        RwD3D9SetTransform(D3DTS_PROJECTION, &Desc[i].lightOrthoMatrix);

        // Render entities
        {
            XMMATRIX view, projection;
            RwD3D9GetTransform(D3DTS_VIEW, &view);
            RwD3D9GetTransform(D3DTS_PROJECTION, &projection);

            Desc[i].g_mLightViewProj = view * projection;
            ShaderContext->SetViewProjectionMatrix(4, true);

            ShadowCasterEntity->Render(i);
        }

        RwCameraEndUpdate(m_pShadowCamera[i]);
    }

    // Restore all channels colors
    RwD3D9SetRenderState(D3DRS_COLORWRITEENABLE,
                         D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED |
                         D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
}