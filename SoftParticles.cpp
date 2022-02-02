#include "SoftParticles.h"
#include "CTimeCycle.h"
#include "CCamera.h"
#include "DeferredRenderer.h"
#include "Fx_c.h"
#include "ShaderManager.h"
#include "VertexBuffer.h"
#include "DynamicVertexBuffer.h"

VertexShader* SoftParticles::mVertexShader = nullptr;
PixelShader* SoftParticles::mPixelShader = nullptr;
VertexBuffer* SoftParticles::mVertexBuffer = nullptr;
void* SoftParticles::mVertexDeclColor = nullptr;

void SoftParticles::Hook()
{
    plugin::patch::RedirectCall(0x004A19A6, RenderBegin);
    plugin::patch::RedirectCall(0x004A1EAC, RenderBegin);
    plugin::patch::RedirectCall(0x004A2D7F, RenderBegin);
    plugin::patch::RedirectCall(0x004A3B4B, RenderBegin);
    plugin::patch::RedirectCall(0x004A20AE, RenderAddTri);
    plugin::patch::RedirectCall(0x004A21AA, RenderAddTri);
    plugin::patch::RedirectCall(0x004A3D74, RenderAddTri);
    plugin::patch::RedirectCall(0x004A3E73, RenderAddTri);
    plugin::patch::RedirectCall(0x004A1E9F, RenderEnd);
    plugin::patch::RedirectCall(0x004A21BD, RenderEnd);
    plugin::patch::RedirectCall(0x004A3B3A, RenderEnd);
    plugin::patch::RedirectCall(0x004A3E87, RenderEnd);
}

void SoftParticles::Initialize()
{
    mVertexShader = new VertexShader();
    mVertexShader->CreateFromBinary("SoftParticlesVS");

    mPixelShader = new PixelShader();
    mPixelShader->CreateFromBinary("SoftParticlesPS");

   //mVertexBuffer = new VertexBuffer();
    //mVertexBuffer->Initialize(TOTAL_TEMP_BUFFER_VERTICES, sizeof(RwIm3DVertex));
    mVertexBuffer = DynamicVertexBuffer::CreateDynamicVertexBuffer(TOTAL_TEMP_BUFFER_VERTICES, sizeof(RwIm3DVertex));

    D3DVERTEXELEMENT9 declaration[] =
    {
        {0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,     0},
        {0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,       0},
        {0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,        0},
        {0, 28, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,     0},
        D3DDECL_END()
    };

    RwD3D9CreateVertexDeclaration(declaration, &mVertexDeclColor);
}

void SoftParticles::Release()
{
    delete mVertexShader;
    delete mPixelShader;
    delete mVertexBuffer;

    rwD3D9DeleteVertexDeclaration(mVertexDeclColor);
}

void SoftParticles::Render()
{
    if(mVertexBuffer == nullptr)
        return;

    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSWRAP);

    rwD3D9SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    rwD3D9SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    rwD3D9SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    rwD3D9SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    rwD3D9SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    rwD3D9SetSamplerState(2, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
    _rwD3D9RWSetRasterStage(DeferredContext->mGraphicsBuffer[1]->GetRaster(), 2);

    XMMATRIX worldMatrix = RwMatrixToXMMATRIX(g_fx.m_pTransformLTM);
    _rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);
    ShaderContext->SetViewProjectionMatrix(4, true);
    _rwD3D9SetVertexShaderConstant(12, &TheCamera.GetPosition(), 1);

    ShaderContext->SetFogParams(0);
    _rwD3D9SetVertexShader(mVertexShader->GetObject());
    _rwD3D9SetPixelShader(mPixelShader->GetObject());

    _rwD3D9SetVertexDeclaration(mVertexDeclColor);

    RwUInt32 stride = sizeof(RwIm3DVertex);
    RwIm3DVertex* bufferMem = nullptr;
    mVertexBuffer->Map(3 * stride * g_fx.m_nVerticesCount, (void**)&bufferMem);
    std::copy(aTempBufferVertices, aTempBufferVertices + g_fx.m_nVerticesCount * 3, bufferMem);
    mVertexBuffer->Unmap();
    RwD3D9SetStreamSource(0, mVertexBuffer->GetBuffer(), 0, stride);

    rwD3D9DrawPrimitive(D3DPT_TRIANGLELIST, 0, g_fx.m_nVerticesCount);
}

void SoftParticles::RenderBegin(RwRaster* raster, RwMatrix* transformLTM, unsigned int transformRenderFlags)
{
    g_fx.m_pTransformLTM = transformLTM;
    g_fx.m_nVerticesCount = 0;
    g_fx.m_nVerticesCount2 = 0;
    g_fx.m_pRasterToRender = raster;
    g_fx.m_nTransformRenderFlags = transformRenderFlags;
    g_fx.m_pVerts = aTempBufferVertices;

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)raster);
}

void SoftParticles::RenderAddTri(float x1, float y1, float z1,
                                 float x2, float y2, float z2,
                                 float x3, float y3, float z3,
                                 float u1, float v1,
                                 float u2, float v2,
                                 float u3, float v3,
                                 int r1, int g1, int b1, int a1,
                                 int r2, int g2, int b2, int a2,
                                 int r3, int g3, int b3, int a3)
{
    RwIm3DVertex*& verts = (RwIm3DVertex*&)g_fx.m_pVerts;

    RwIm3DVertexSetPos(&verts[0], x1, y1, z1);
    RwIm3DVertexSetRGBA(&verts[0], r1, g1, b1, a1);
    RwIm3DVertexSetPos(&verts[1], x2, y2, z2);
    RwIm3DVertexSetRGBA(&verts[1], r2, g2, b2, a2);
    RwIm3DVertexSetPos(&verts[2], x3, y3, z3);
    RwIm3DVertexSetRGBA(&verts[2], r3, g3, b3, a3);

    if(g_fx.m_pRasterToRender)
    {
        RwIm3DVertexSetU(&verts[0], u1);
        RwIm3DVertexSetV(&verts[0], v1);
        RwIm3DVertexSetU(&verts[1], u2);
        RwIm3DVertexSetV(&verts[1], v2);
        RwIm3DVertexSetU(&verts[2], u3);
        RwIm3DVertexSetV(&verts[2], v3);
    }

    g_fx.m_nVerticesCount++; // indices
    g_fx.m_nVerticesCount2 += 3;
    verts += 3;

    if(g_fx.m_nVerticesCount2 >= TOTAL_TEMP_BUFFER_VERTICES - 3 || g_fx.m_nVerticesCount >= TOTAL_TEMP_BUFFER_INDICES - 1)
    {
        RenderEnd();
    }
}

void SoftParticles::RenderEnd()
{
    if(!g_fx.m_nVerticesCount)
        return;

    Render();

    g_fx.m_pVerts = aTempBufferVertices;
    g_fx.m_nVerticesCount2 = 0;
    g_fx.m_nVerticesCount = 0;
}