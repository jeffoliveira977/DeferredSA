#include "Immediate3D.h"
#include "ShaderManager.h"
#include "PixelShaderConstant.h"
#include "IndexBufferManager.h"
VertexBuffer* Immediate3D::mVertexBuffer = nullptr;
RwIndexBuffer* Immediate3D::mIndexBuffer = nullptr;
VertexShader* Immediate3D::mVertexShader = nullptr;
PixelShader* Immediate3D::mPixelShader = nullptr;

void Immediate3D::Hook()
{
    plugin::patch::RedirectJump(0x80DFB0, Close);
    plugin::patch::RedirectJump(0x80E020, Open);
    plugin::patch::RedirectJump(0x80E270, Render);
}

void Immediate3D::Close(void)
{
    IndexBufferManager::Release();

    if(VertexDeclIm3DNoTex != NULL)
    {
        rwD3D9DeleteVertexDeclaration(VertexDeclIm3DNoTex);
        VertexDeclIm3DNoTex = NULL;
    }

    if(VertexDeclIm3DOld != NULL)
    {
        rwD3D9DeleteVertexDeclaration(VertexDeclIm3DOld);
        VertexDeclIm3DOld = NULL;
    }

    delete mVertexBuffer;
    delete mPixelShader;
    delete mVertexShader;
    delete mIndexBuffer;

   return;
}

RwBool Immediate3D::Open(void)
{
     IndexBufferManager::Restore();
    {
        D3DVERTEXELEMENT9 declaration[] =
        {
            {0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,     0},
            {0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,       0},
            {0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,        0},
            {0, 28, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,     0},
            D3DDECL_END()
        };

        RwD3D9CreateVertexDeclaration(declaration,
                                      &VertexDeclIm3DOld);
    }

    {
        D3DVERTEXELEMENT9 declaration[] =
        {
            {0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,     0},
            {0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,        0},
            D3DDECL_END()
        };

        RwD3D9CreateVertexDeclaration(declaration,
                                      &VertexDeclIm3DNoTex);
    }

   /// try
   // {
        mVertexShader = new VertexShader();
        mVertexShader->CreateFromBinary("Im3dVS");
        mPixelShader = new PixelShader();
        mPixelShader->CreateFromBinary("Im3dPS");

        mVertexBuffer = new VertexBuffer();
        mVertexBuffer->Initialize(BUFFER_MAX_INDEX, sizeof(RwIm3DVertex));
        mIndexBuffer = new RwIndexBuffer();
        mIndexBuffer->Initialize(BUFFER_MAX_INDEX);
   /* }
    catch(const std::exception&e)
    {
        MessageBox(0, e.what(), "Error", MB_OK);
    }*/

    return TRUE;
}

RwBool Immediate3D::Render(RxPipelineNodeInstance* self, const RxPipelineNodeParam* params)
{
    RwUInt32 numVerts = _rwD3D9ImmPool->numElements;

    if(numVerts > 0)
    {
        RwIm3DVertex*       verts;
        _rwIm3DPoolStash*   stash;
        RwImVertexIndex*    indices;
        RwUInt32            stride;
        RwUInt32            numPrimitives;
        RwUInt32            numIndices;
        RwUInt32            primitiveType;

        stash = &_rwD3D9ImmPool->stash;
        verts = (RwIm3DVertex*)_rwD3D9ImmPool->elements;

        PixelShaderConstant::SetInt(0, stash->flags & rwIM3D_VERTEXUV ? 1 : 0);

        if(stash->flags & rwIM3D_ALLOPAQUE)
            _rwD3D9RenderStateVertexAlphaEnable(FALSE);
        else
            _rwD3D9RenderStateVertexAlphaEnable(TRUE);

        XMMATRIX worldMatrix = RwMatrixToXMMATRIX(stash->ltm);
        _rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);
        ShaderContext->SetViewProjectionMatrix(4, true);
   
        if(stash->flags & rwIM3D_NOCLIP)
            RwD3D9SetRenderState(D3DRS_CLIPPING, FALSE);
        else
            RwD3D9SetRenderState(D3DRS_CLIPPING, TRUE);

        _rwD3D9SetVertexDeclaration(VertexDeclIm3DOld);

        _rwD3D9SetVertexShader(mVertexShader->GetShader());
        _rwD3D9SetPixelShader(mPixelShader->GetShader());
 
        primitiveType = _RwD3D9PrimConv[stash->primType];
   
        stride = sizeof(RwIm3DVertex);

        RwIm3DVertex* bufferMem = nullptr;
        mVertexBuffer->Map(numVerts * stride, (void**)&bufferMem);
        std::copy(verts, verts + numVerts, bufferMem);
        mVertexBuffer->Unmap();
        RwD3D9SetStreamSource(0, mVertexBuffer->GetBuffer(), 0, stride);

        if(stash->indices && stash->numIndices > 0)
        {
            indices = stash->indices;
            numIndices = stash->numIndices;

            switch(primitiveType)
            {
                case D3DPT_LINELIST:
                {
                    numPrimitives = numIndices / 2;
                    break;
                }
                case D3DPT_LINESTRIP:
                {
                    numPrimitives = numIndices - 1;
                    break;
                }
                case D3DPT_TRIANGLELIST:
                {
                    numPrimitives = numIndices / 3;
                    break;
                }
                case D3DPT_TRIANGLESTRIP:
                {
                    numPrimitives = numIndices - 2;
                    break;
                }
                case D3DPT_TRIANGLEFAN:
                {
                    numPrimitives = numIndices - 2;
                    break;
                }
            }

            RwImVertexIndex* indexBuffer = nullptr;
            mIndexBuffer->Map(numIndices * sizeof(RwImVertexIndex), (void**)&indexBuffer);
            std::copy(indices, indices + numIndices, indexBuffer);
            mIndexBuffer->Unmap();

            _rwD3D9SetIndices(mIndexBuffer->GetBuffer());
            _rwD3D9DrawIndexedPrimitive(primitiveType, 0, 0, numVerts, 0, numPrimitives);
        }

        else
        {
            switch(primitiveType)
            {
                case D3DPT_LINELIST:
                {
                    numPrimitives = numVerts / 2;
                    break;
                }
                case D3DPT_LINESTRIP:
                {
                    numPrimitives = numVerts - 1;
                    break;
                }
                case D3DPT_TRIANGLELIST:
                {
                    numPrimitives = numVerts / 3;
                    break;
                }
                case D3DPT_TRIANGLESTRIP:
                {
                    numPrimitives = numVerts - 2;
                    break;
                }
                case D3DPT_TRIANGLEFAN:
                {
                    numPrimitives = numVerts - 2;
                    break;
                }
            }
            rwD3D9DrawPrimitive(primitiveType, 0, numPrimitives);

        }
    }

    return TRUE;
}