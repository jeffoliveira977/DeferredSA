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
#include "imgui.h"

#define MINI_CASE_SENSITIVE
#include "mini/ini.h"

CascadedShadowRendering* CascadedShadowManagement;

CascadedShadowRendering::CascadedShadowRendering()
{
    ShadowSize = 1024 * 2;
    CascadeCount = 3;
    FilterSize = 40.0f;
    MinDistance = 2.0;
    MaxDistance = 100.0;
    PSSMLambda = 0.05f;

    for(size_t i = 0; i < 4; i++)
        mColorRaster[i] = nullptr;

    mDepthRaster = nullptr;
}

CascadedShadowRendering::~CascadedShadowRendering()
{
    for(int i = 0; i < CascadeCount; i++)
        RwRasterDestroy(mColorRaster[i]);

    RwRasterDestroy(mDepthRaster);
}

void CascadedShadowRendering::Initialize()
{
    for(int i = 0; i < CascadeCount; i++)
        mColorRaster[i] = RwD3D9RasterCreate(ShadowSize, ShadowSize, D3DFMT_G32R32F, rwRASTERTYPECAMERATEXTURE);

    mDepthRaster = RwRasterCreate(ShadowSize, ShadowSize, 32, rwRASTERTYPEZBUFFER);

    mINI::INIFile file("DeferredConfig.ini");
    mINI::INIStructure ini;

    file.read(ini);

    int n = 0, o = 0;
    for(auto const& it : ini)
    {
        auto const& section = it.first;
        auto const& collection = it.second;
        for(auto const& it2 : collection)
        {
            auto const& key = it2.first;
            auto const& value = it2.second;
            if(section.compare("SHADOW") == 0)
            {
                if(key.find("DistanceCoefficients") != std::string::npos)
                {
                    mDistanceCoefficients[n] = (float)atof(value.c_str());
                    n++;
                }
                else if(key.find("BiasCoefficients") != std::string::npos)
                {
                    BiasCoefficients[o] = (float)atof(value.c_str());
                    o++;
                }

            }
        }
    }
}

void CascadedShadowRendering::UpdateBuffer()
{
    mConstantBuffer.mDistanceCoefficients = {m_fShadowDistances[0], m_fShadowDistances[1], m_fShadowDistances[2], m_fShadowDistances[3]};
    mConstantBuffer.mShadowBias = {BiasCoefficients[0], BiasCoefficients[1], BiasCoefficients[2], BiasCoefficients[3]};
    mConstantBuffer.mShadowParam = {(float)ShadowSize, m_fShadowDistances[4], (float)CascadeCount,
                                  1.0f - (CGame::currArea == 0 ? CGameIdle::m_fShadowDNBalance : 1.0f)};

    for(size_t i = 0; i < CascadeCount; i++)
    {
        mConstantBuffer.mShadowMatrix[i] = Desc[i].mLightViewMatrix * Desc[i].mLightOrthoMatrix;
    }
}

void CascadedShadowRendering::UpdateImgui()
{
    if(ImGui::BeginTabItem("Shadow"))
    {
        ImGuiSliderFlags flags = ImGuiSliderFlags_NoRoundToFormat;

        ImGui::EndTabItem();
        ImGui::InputFloat("Bias 0", &BiasCoefficients[0], 0.000001, 0.00001, "%.6f");
        ImGui::InputFloat("Bias 1", &BiasCoefficients[1], 0.000001, 0.00001, "%.6f");
        ImGui::InputFloat("Bias 2", &BiasCoefficients[2], 0.000001, 0.00001, "%.6f");
        ImGui::InputFloat("Bias 3", &BiasCoefficients[3], 0.000001, 0.00001, "%.6f");

        ImGui::InputFloat("DistanceCoefficients 0", &mDistanceCoefficients[0], 0.0001, 0.001, "%.6f");
        ImGui::InputFloat("DistanceCoefficients 1", &mDistanceCoefficients[1], 0.0001, 0.001, "%.6f");
        ImGui::InputFloat("DistanceCoefficients 2", &mDistanceCoefficients[2], 0.0001, 0.001, "%.6f");
        ImGui::InputFloat("DistanceCoefficients 3", &mDistanceCoefficients[3], 0.0001, 0.001, "%.6f");

        mINI::INIFile file("DeferredConfig.ini");
        mINI::INIStructure ini;

        file.read(ini);

        int n = 0, o = 0;
        for(auto const& it : ini)
        {
            auto const& section = it.first;
            auto const& collection = it.second;
            for(auto const& it2 : collection)
            {
                auto const& key = it2.first;
                if(section.compare("SHADOW") == 0)
                {
                    if(key.find("DistanceCoefficients") != std::string::npos)
                    {
                        ini[section][key] = std::to_string(mDistanceCoefficients[n]);
                        n++;
                    }
                    else if(key.find("BiasCoefficients") != std::string::npos)
                    {
                        ini[section][key] = std::to_string(BiasCoefficients[o]);
                        o++;
                    }

                }
            }
        }
        file.write(ini);
    }
}

void CascadedShadowRendering::Update()
{
    if(CGame::currArea != 0 || CGameIdle::m_fShadowDNBalance >= 1.0)
        return;

    gRenderState = stageCascadeShadow;

    // Render in red channel
    RwD3D9SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED);

    RWSRCGLOBAL(curCamera) = Scene.m_pRwCamera;
   
    // Render cascades
    for(size_t i = 0; i < CascadeCount; i++)
    {
        RwD3D9SetRenderTarget(0, mColorRaster[i]);
        rwD3D9SetDepthStencil(mDepthRaster);

        D3DVIEWPORT9 viewport;
        viewport.X = 0;
        viewport.Y = 0;
        viewport.Width = mColorRaster[i]->width;
        viewport.Height = mColorRaster[i]->height;
        viewport.MinZ = 0;
        viewport.MaxZ = 1;
        RwD3DDevice->SetViewport(&viewport);

        rwD3D9Clear(nullptr, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);

        if(rwD3D9TestState())
        {
            RwD3D9SetTransform(D3DTS_VIEW, &Desc[i].mLightViewMatrix);
            RwD3D9SetTransform(D3DTS_PROJECTION, &Desc[i].mLightOrthoMatrix);

            _rwD3D9SetVertexShaderConstant(4, &Desc[i].mLightViewMatrix, 4);
            _rwD3D9SetVertexShaderConstant(8, &Desc[i].mLightOrthoMatrix, 4);

            ShadowCasterEntity->Render(i);
        }

    }

    RWSRCGLOBAL(curCamera) = NULL;

    // Restore all channels colors
    RwD3D9SetRenderState(D3DRS_COLORWRITEENABLE,
                         D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED |
                         D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
}