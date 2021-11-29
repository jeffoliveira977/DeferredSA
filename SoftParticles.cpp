#include "SoftParticles.h"
#include "CTimeCycle.h"
#include "CCamera.h"
#include "DeferredRenderer.h"
#include "Fx_c.h"
#include "ShaderManager.h"

SoftParticles* SoftParticlesContext;

SoftParticles::SoftParticles()
{
	VS_softParticles = nullptr;
	PS_softParticles = nullptr;
}

SoftParticles::~SoftParticles()
{
	RwD3D9DeleteVertexShader(VS_softParticles);
	RwD3D9DeletePixelShader(PS_softParticles);
}

void SoftParticles::hook()
{
    plugin::patch::RedirectJump(0x004A1410, RenderAddTri);
    plugin::patch::RedirectJump(0x004A13B0, RenderBegin);
    plugin::patch::RedirectJump(0x004A1600, RenderEnd);
}

void SoftParticles::initGraphicsBuffer()
{
	VS_softParticles = RwCreateCompiledVertexShader("SoftParticlesVS");
	PS_softParticles = RwCreateCompiledPixelShader("SoftParticlesPS");
}

void SoftParticles::SetupParams()
{
   // RwD3D9RenderStateReset();
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
    RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)120);
    //DefinedState();
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, FALSE);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, FALSE);
    RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
   // RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
    RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    if(g_fx.m_nTransformRenderFlags & rwIM3D_ALLOPAQUE)
    {
        _rwD3D9RenderStateVertexAlphaEnable(FALSE);
    }
    else
    {
        _rwD3D9RenderStateVertexAlphaEnable(TRUE);
    }
    if(g_fx.m_nTransformRenderFlags & rwIM3D_NOCLIP)
    {
        RwD3D9SetRenderState(D3DRS_CLIPPING, FALSE);
    }
    else
    {
        RwD3D9SetRenderState(D3DRS_CLIPPING, TRUE);
    }
    XMMATRIX worldMatrix = RwMatrixToXMMATRIX(g_fx.m_pTransformLTM);
    _rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);
    ShaderContext->SetViewProjectionMatrix(4, true);
    _rwD3D9SetVertexShaderConstant(12, &TheCamera.GetPosition(), 1);

    ShaderContext->SetFogParams(0);
    _rwD3D9RWSetRasterStage(DeferredContext->m_graphicsBuffer[1], 2);
    _rwD3D9SetVertexShader(VS_softParticles);
    _rwD3D9SetPixelShader(PS_softParticles);
}
constexpr int32_t TOTAL_TEMP_BUFFER_INDICES = 4096;
constexpr int32_t TOTAL_TEMP_BUFFER_VERTICES = 2048;
RxObjSpace3DVertex(&aTempBufferVertices)[TOTAL_TEMP_BUFFER_VERTICES] = *(RxObjSpace3DVertex(*)[TOTAL_TEMP_BUFFER_VERTICES])0xC4D958;

void SoftParticles::RenderBegin(RwRaster* raster, RwMatrix* transformLTM, unsigned int transformRenderFlags)
{
    g_fx.m_pTransformLTM = transformLTM;
    g_fx.m_nVerticesCount = 0;
    g_fx.m_nVerticesCount2 = 0;
    g_fx.m_pRasterToRender = raster;
    g_fx.m_nTransformRenderFlags = transformRenderFlags;
    g_fx.m_pVerts = aTempBufferVertices;

    RwRaster* currRaster{};
    RwRenderStateGet(rwRENDERSTATETEXTURERASTER, &currRaster);
    if(currRaster != raster)
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)raster);
}

void SoftParticles::RenderEnd()
{
    if(!g_fx.m_nVerticesCount)
        return;

    /*if(RwIm3DTransform(aTempBufferVertices, 3 * g_fx.m_nVerticesCount, g_fx.m_pTransformLTM, g_fx.m_nTransformRenderFlags))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
        RwIm3DEnd();
    }*/

    void* bufferMem = NULL;
    RwUInt32 baseIndex = 0;

    RwInt32 numVerts = 3 * g_fx.m_nVerticesCount;
    RwIm3DVertex* verts = aTempBufferVertices;

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
        RwD3D9SetStreamSource(0, CurrentVertexBuffer3D, 0, stride);

        SoftParticlesContext->SetupParams();
        rwD3D9DrawPrimitive(D3DPT_TRIANGLELIST, CurrentBaseIndex3D, numVerts / 3);
        _rwD3D9SetVertexShader(NULL);
        _rwD3D9SetPixelShader(NULL);
    }

    g_fx.m_pVerts = aTempBufferVertices;
    g_fx.m_nVerticesCount2 = 0;
    g_fx.m_nVerticesCount = 0;
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
    RwIm3DVertexSetRGBA(&verts[0], (RwUInt8)r1, (RwUInt8)g1, (RwUInt8)b1, (RwUInt8)a1);

    RwIm3DVertexSetPos(&verts[1], x2, y2, z2);
    RwIm3DVertexSetRGBA(&verts[1], (RwUInt8)r2, (RwUInt8)g2, (RwUInt8)b2, (RwUInt8)a2);

    RwIm3DVertexSetPos(&verts[2], x3, y3, z3);
    RwIm3DVertexSetRGBA(&verts[2], (RwUInt8)r3, (RwUInt8)g3, (RwUInt8)b3, (RwUInt8)a3);

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