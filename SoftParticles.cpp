#include "SoftParticles.h"
#include "CTimeCycle.h"
#include "CCamera.h"
#include "DeferredRenderer.h"
#include "Fx_c.h"
#include "ShaderManager.h"

SoftParticles* SoftParticlesContext;

void Fx_c__Render_HOOK(Fx_c* ptfx, RwCamera* cam, int bHeatHaze)
{
	SoftParticlesContext->SetupParams();
	g_fxMan.Render(cam, bHeatHaze);
	_rwD3D9SetVertexShader(NULL);
	_rwD3D9SetPixelShader(NULL);
}

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

void RenderBegin_HOOK(RwRaster* raster, RwMatrix* transformLTM, unsigned int transformRenderFlags);
void RenderEnd_HOOK();
void RenderAddTri_HOOK(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float u1, float v1, float u2, float v2, float u3, float v3, int r1, int g1, int b1, int a1, int r2, int g2, int b2, int a2, int r3, int g3, int b3, int a3);
void SoftParticles::hook()
{
   /* plugin::patch::RedirectCall(0x004A3D74, RenderAddTri_HOOK);
    plugin::patch::RedirectCall(0x004A3E73, RenderAddTri_HOOK);

    plugin::patch::RedirectJump(0x004A13B0, RenderBegin_HOOK);
    plugin::patch::RedirectJump(0x4A1600, RenderEnd_HOOK);*/
}

void SoftParticles::initGraphicsBuffer()
{
	VS_softParticles = RwCreateCompiledVertexShader("SoftParticlesVS");
	PS_softParticles = RwCreateCompiledPixelShader("SoftParticlesPS");
}

void SoftParticles::SetupParams()
{
    //RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
    //RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
    //RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    //RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
    //RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    //RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)5);
    //RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)6);

    XMMATRIX worldMatrix = RwMatrixToXMMATRIX(g_fx.m_pTransformLTM);
    _rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);
    ShaderContext->SetViewProjectionMatrix(4, true);
    _rwD3D9SetVertexShaderConstant(12, &TheCamera.GetPosition(), 1);

    ShaderContext->SetFogParams(0);
    _rwD3D9RWSetRasterStage(DeferredContext->m_graphicsBuffer[1], 2);
    _rwD3D9SetVertexShader(VS_softParticles);
    _rwD3D9SetPixelShader(PS_softParticles);
}

void SoftParticlesPipeline();
void RenderBegin_HOOK(RwRaster* raster, RwMatrix* transformLTM, unsigned int transformRenderFlags)
{
    RwRaster* newRaster; // esi

    newRaster = raster;
    g_fx.m_pTransformLTM = transformLTM;
    g_fx.m_nVerticesCount = 0;
    g_fx.m_nVerticesCount2 = 0;
    g_fx.m_pRasterToRender = raster;
    g_fx.m_nTransformRenderFlags = transformRenderFlags;
    g_fx.m_pVerts = TempVertexBuffer;
    RwRenderStateGet(rwRENDERSTATETEXTURERASTER, &raster);
    if(raster != newRaster)
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)newRaster);
}

void RenderEnd_HOOK()
{
    if(g_fx.m_nVerticesCount)
    {
        // RwIm3DTransform(TempVertexBuffer, 3 * g_fx.m_nVerticesCount, g_fx.m_pTransformLTM, g_fx.m_nTransformRenderFlags);
        //RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
        //RwIm3DEnd();
        PrintMessage("%d %d", g_fx.m_nVerticesCount, g_fx.m_nVerticesCount2);
        SoftParticlesPipeline();

        g_fx.m_nVerticesCount = 0;
        g_fx.m_nVerticesCount2 = 0;
        g_fx.m_pVerts = TempVertexBuffer;
    }
}



void RenderAddTri_HOOK(float x1, float y1, float z1, 
                       float x2, float y2, float z2, 
                       float x3, float y3, float z3, 
                       float u1, float v1, float u2, 
                       float v2, float u3, float v3,
                       int r1, int g1, int b1, int a1, 
                       int r2, int g2, int b2, int a2, 
                       int r3, int g3, int b3, int a3)
{
    
    RwIm3DVertex* verts = (RwIm3DVertex*)g_fx.m_pVerts;
    verts->objVertex.x = x1;
    verts->objVertex.y = y1;
    verts->objVertex.z = z1;
    verts->color = b1 | ((g1 | ((r1 | (a1 << 8)) << 8)) << 8);
    verts->u = u1;
    verts->v = v1;
    verts++;
    g_fx.m_nVerticesCount2++;

    verts->objVertex.x = x2;
    verts->objVertex.y = y2;
    verts->objVertex.z = z2;
    verts->color = b2 | ((g2 | ((r2 | (a2 << 8)) << 8)) << 8);
    verts->u = u2;
    verts->v = v2;
    verts++;
    g_fx.m_nVerticesCount2++;

    verts->objVertex.x = x3;
    verts->objVertex.y = y3;
    verts->objVertex.z = z3;
    verts->color = b3 | ((g3 | ((r3 | (a3 << 8)) << 8)) << 8);
    verts->u = u3;
    verts->v = v3;
    verts++;
    g_fx.m_nVerticesCount2++;
    g_fx.m_nVerticesCount++;

    //if(g_fx.m_nVerticesCount2 >= 2045 || g_fx.m_nVerticesCount++ >= 4095)
    //{
        SoftParticlesPipeline();
        g_fx.m_nVerticesCount = 0;
        g_fx.m_nVerticesCount2 = 0;
        g_fx.m_pVerts = TempVertexBuffer;
   // }
}

void SoftParticlesPipeline()
{
    void* bufferMem = NULL;
    RwUInt32                 baseIndex = 0;

    RwInt32 numVerts =  3*g_fx.m_nVerticesCount;
    RwIm3DVertex* verts = TempVertexBuffer;

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
        RwInt32 numPrimitives = numVerts / 3;
        rwD3D9DrawPrimitive(D3DPT_TRIANGLELIST, CurrentBaseIndex3D, numPrimitives);
        _rwD3D9SetVertexShader(NULL);
        _rwD3D9SetPixelShader(NULL);
    }
}