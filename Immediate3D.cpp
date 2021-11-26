#include "Immediate3D.h"
#include "ShaderManager.h"

void _rwD3D9Im3DRenderClose(void);
RwBool _rwD3D9Im3DRenderOpen(void);

void* VS_Immediate3D = NULL;
void* PS_Immediate3D = NULL;

void Immediate3D__Hook()
{
    plugin::patch::RedirectJump(0x80DFB0, _rwD3D9Im3DRenderClose);
    plugin::patch::RedirectJump(0x80E020, _rwD3D9Im3DRenderOpen);
    plugin::patch::RedirectJump(0x80E270, rxD3D9SubmitNode);
}

void _rwD3D9Im3DRenderClose(void)
{
    if(IndexBuffer3D)
    {
        IDirect3DIndexBuffer9_Release(IndexBuffer3D);
        IndexBuffer3D = NULL;
    }

    if(VertexDeclIm3DNoTex != NULL)
    {
        rwD3D9DeleteVertexDeclaration(VertexDeclIm3DNoTex);
        VertexDeclIm3DNoTex = NULL;
    }

    if(VertexDeclIm3D != NULL)
    {
        rwD3D9DeleteVertexDeclaration(VertexDeclIm3D);
        VertexDeclIm3D = NULL;
    }

    if(VertexDeclIm3DOld != NULL)
    {
        rwD3D9DeleteVertexDeclaration(VertexDeclIm3DOld);
        VertexDeclIm3DOld = NULL;
    }

    if(VS_Immediate3D != NULL)
    {
        RwD3D9DeleteVertexShader(VS_Immediate3D);
    }

    if(PS_Immediate3D != NULL)
    {
        RwD3D9DeletePixelShader(PS_Immediate3D);
    }

   return;
}


RwBool _rwD3D9Im3DRenderOpen(void)
{
    RwBool rv = FALSE;
    RwUInt32    usage;
    D3DPOOL     d3dPool;

    IB3DOffset = 0;

    usage = D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;

    if(RwD3D9DeviceCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        d3dPool = D3DPOOL_DEFAULT;
    }
    else
    {
        d3dPool = D3DPOOL_SYSTEMMEM;
    }

    if(D3D_OK == IDirect3DDevice9_CreateIndexBuffer(RwD3DDevice,
                                                    BUFFER_MAX_INDEX * sizeof(RwUInt16),
                                                    usage, D3DFMT_INDEX16, d3dPool,
                                                    &IndexBuffer3D,
                                                    NULL))
    {
        rv = TRUE;
    }

    if(!rv)
    {
        _rwD3D9Im3DRenderClose();
    }
    else
    {
        D3DVERTEXELEMENT9 declaration[5];
        RwUInt32 declarationIndex, offset;

        /*
         * Old Format
         */
        declarationIndex = 0;
        offset = 0;

        /* Positions */
        declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = offset;
        declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT3;
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_POSITION;
        declaration[declarationIndex].UsageIndex = 0;
        declarationIndex++;
        offset += sizeof(RwV3d);

        /* Normals */
        declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = offset;
        declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT3;
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_NORMAL;
        declaration[declarationIndex].UsageIndex = 0;
        declarationIndex++;
        offset += sizeof(RwV3d);

        /* Pre-lighting */
        declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = offset;
        declaration[declarationIndex].Type = D3DDECLTYPE_D3DCOLOR;
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_COLOR;
        declaration[declarationIndex].UsageIndex = 0;
        declarationIndex++;
        offset += sizeof(RwRGBA);

        /* Texture coordinates */
        declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = offset;
        declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT2;
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_TEXCOORD;
        declaration[declarationIndex].UsageIndex = 0;
        declarationIndex++;
        offset += sizeof(RwV2d);

        declaration[declarationIndex].Stream = 0xFF;
        declaration[declarationIndex].Offset = 0;
        declaration[declarationIndex].Type = D3DDECLTYPE_UNUSED;
        declaration[declarationIndex].Method = 0;
        declaration[declarationIndex].Usage = 0;
        declaration[declarationIndex].UsageIndex = 0;

        RwD3D9CreateVertexDeclaration(declaration,
                                      &VertexDeclIm3DOld);

        /*
         * New Format
         */
        declarationIndex = 0;
        offset = 0;

        /* Positions */
        declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = offset;
        declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT3;
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_POSITION;
        declaration[declarationIndex].UsageIndex = 0;
        declarationIndex++;
        offset += sizeof(RwV3d);

        /* Pre-lighting */
        declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = offset;
        declaration[declarationIndex].Type = D3DDECLTYPE_D3DCOLOR;
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_COLOR;
        declaration[declarationIndex].UsageIndex = 0;
        declarationIndex++;
        offset += sizeof(RwRGBA);

        /* Texture coordinates */
        declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = offset;
        declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT2;
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_TEXCOORD;
        declaration[declarationIndex].UsageIndex = 0;
        declarationIndex++;
        offset += sizeof(RwV2d);

        declaration[declarationIndex].Stream = 0xFF;
        declaration[declarationIndex].Offset = 0;
        declaration[declarationIndex].Type = D3DDECLTYPE_UNUSED;
        declaration[declarationIndex].Method = 0;
        declaration[declarationIndex].Usage = 0;
        declaration[declarationIndex].UsageIndex = 0;

        RwD3D9CreateVertexDeclaration(declaration,
                                      &VertexDeclIm3D);


        /*
         * New Format no texture coords
         */
        declarationIndex = 0;
        offset = 0;

        /* Positions */
        declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = offset;
        declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT3;
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_POSITION;
        declaration[declarationIndex].UsageIndex = 0;
        declarationIndex++;
        offset += sizeof(RwV3d);

        /* Pre-lighting */
        declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = offset;
        declaration[declarationIndex].Type = D3DDECLTYPE_D3DCOLOR;
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_COLOR;
        declaration[declarationIndex].UsageIndex = 0;
        declarationIndex++;
        offset += sizeof(RwRGBA);

        declaration[declarationIndex].Stream = 0xFF;
        declaration[declarationIndex].Offset = 0;
        declaration[declarationIndex].Type = D3DDECLTYPE_UNUSED;
        declaration[declarationIndex].Method = 0;
        declaration[declarationIndex].Usage = 0;
        declaration[declarationIndex].UsageIndex = 0;

        RwD3D9CreateVertexDeclaration(declaration,
                                      &VertexDeclIm3DNoTex);

        VS_Immediate3D = RwCreateCompiledVertexShader("Im3dVS");
        PS_Immediate3D = RwCreateCompiledPixelShader("Im3dPS");
    }

    return rv;
}

RwBool
rxD3D9SubmitNode(RxPipelineNodeInstance* self __RWUNUSED__,
                 const RxPipelineNodeParam* params __RWUNUSED__)
{
    const RwIm3DVertex* verts;
    const _rwIm3DPoolStash* stash;
    const RwImVertexIndex* indices;
    RwBool              useTexCoords;
    LPDIRECT3DVERTEXDECLARATION9 vertexDecl;
    RwUInt32            stride;
    RwUInt32            numVerts;
    RwUInt32            numPrimitives;
    RwUInt32            numIndices;
    RwUInt32            primitiveType;
    HRESULT             hr = E_FAIL;

    stash = &_rwD3D9ImmPool->stash;

    verts = (RwIm3DVertex*)_rwD3D9ImmPool->elements;

    /* Check vertex type */
    if(stash->flags & rwIM3D_VERTEXUV)
    {
        useTexCoords = TRUE;

        vertexDecl = VertexDeclIm3D;

        stride = sizeof(RxD3D9Im3DVertex);

        float params = 1.0;
        _rwD3D9SetPixelShaderConstant(0, &params, 1);
    }
    else
    {
        useTexCoords = FALSE;

        vertexDecl = VertexDeclIm3DNoTex;

        stride = sizeof(_rxD3D9Im3DVertexNoTex);

        float params = 0.0;
        _rwD3D9SetPixelShaderConstant(0, &params, 1);
    }

    /* Set blending modes */
    if(stash->flags & rwIM3D_ALLOPAQUE)
    {
        _rwD3D9RenderStateVertexAlphaEnable(FALSE);
    }
    else
    {
        _rwD3D9RenderStateVertexAlphaEnable(TRUE);
    }

    XMMATRIX worldMatrix = RwMatrixToXMMATRIX(stash->ltm);
    _rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);
    ShaderContext->SetViewProjectionMatrix(4, true);

    /* Enable clipping if needed*/
    if(stash->flags & rwIM3D_NOCLIP)
    {
        RwD3D9SetRenderState(D3DRS_CLIPPING, FALSE);
    }
    else
    {
        RwD3D9SetRenderState(D3DRS_CLIPPING, TRUE);
    }

    /* Set the vertex Declaration */
    _rwD3D9SetVertexDeclaration(vertexDecl);

    /* Disable any vertex shader */
    _rwD3D9SetVertexShader(VS_Immediate3D);

    /* Disable any pixel shader */
    _rwD3D9SetPixelShader(PS_Immediate3D);

    primitiveType = _RwD3D9PrimConv[stash->primType];

    numVerts = _rwD3D9ImmPool->numElements;

    if(numVerts > 0)
    {
        if((NULL != stash->indices) && (stash->numIndices > 0))
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

            /* Check for inefficient index primitive */
            if(numIndices <= numVerts && numIndices <= 8)
            {
                /* Use numIndices instead of numVerts */
                void* bufferMem;

                /* Fill Vertex Buffer */
                if(RwD3D9DynamicVertexBufferLock(stride,
                                                 numIndices,
                                                 (void**)&CurrentVertexBuffer3D,
                                                 &bufferMem,
                                                 &CurrentBaseIndex3D))
                {
                    float* vertexDes = (float*)bufferMem;
                    RwUInt32 i;

                    if(useTexCoords)
                    {
                        for(i = 0; i < numIndices; i++)
                        {
                            const float* vertexSrc = (const float*)(verts + indices[i]);

                            vertexDes[0] = vertexSrc[0];
                            vertexDes[1] = vertexSrc[1];
                            vertexDes[2] = vertexSrc[2];

                            ((RwUInt32*)vertexDes)[3] = ((const RwUInt32*)vertexSrc)[6];

                            vertexDes[4] = vertexSrc[7];
                            vertexDes[5] = vertexSrc[8];

                            vertexDes += 6;
                        }
                    }
                    else
                    {
                        for(i = 0; i < numIndices; i++)
                        {
                            const float* vertexSrc = (const float*)(verts + indices[i]);

                            vertexDes[0] = vertexSrc[0];
                            vertexDes[1] = vertexSrc[1];
                            vertexDes[2] = vertexSrc[2];

                            ((RwUInt32*)vertexDes)[3] = ((const RwUInt32*)vertexSrc)[6];

                            vertexDes += 4;
                        }
                    }

                    RwD3D9DynamicVertexBufferUnlock(CurrentVertexBuffer3D);

                    RwD3D9SetStreamSource(0, CurrentVertexBuffer3D, 0, stride);

                    _rwD3D9RenderStateFlushCache();

                    hr = IDirect3DDevice9_DrawPrimitive(RwD3DDevice,
                        (D3DPRIMITIVETYPE)primitiveType,
                                                        CurrentBaseIndex3D,
                                                        numPrimitives);
                }
            }
            else
            {
                /* Check index buffer */
                if(IndexBuffer3D && (numIndices <= BUFFER_MAX_INDEX))
                {
                    void* bufferMem;

                    /* Fill Vertex Buffer */
                    if(RwD3D9DynamicVertexBufferLock(stride,
                                                     numVerts,
                                                     (void**)&CurrentVertexBuffer3D,
                                                     &bufferMem,
                                                     &CurrentBaseIndex3D))
                    {
                        float* vertexDes = (float*)bufferMem;
                        const float* vertexSrc = (const float*)(verts);
                        RwUInt32 i = numVerts;

                        if(useTexCoords)
                        {
                            do
                            {
                                vertexDes[0] = vertexSrc[0];
                                vertexDes[1] = vertexSrc[1];
                                vertexDes[2] = vertexSrc[2];

                                ((RwUInt32*)vertexDes)[3] = ((const RwUInt32*)vertexSrc)[6];

                                vertexDes[4] = vertexSrc[7];
                                vertexDes[5] = vertexSrc[8];

                                vertexDes += 6;
                                vertexSrc += 9;
                                i--;
                            }
                            while(i);
                        }
                        else
                        {
                            do
                            {
                                vertexDes[0] = vertexSrc[0];
                                vertexDes[1] = vertexSrc[1];
                                vertexDes[2] = vertexSrc[2];

                                ((RwUInt32*)vertexDes)[3] = ((const RwUInt32*)vertexSrc)[6];

                                vertexDes += 4;
                                vertexSrc += 9;
                                i--;
                            }
                            while(i);
                        }

                        RwD3D9DynamicVertexBufferUnlock(CurrentVertexBuffer3D);

                        /* Fill the Index Buffer */
                        if((IB3DOffset + numIndices) > BUFFER_MAX_INDEX)
                        {
                            IB3DOffset = 0;

                            hr = IDirect3DIndexBuffer9_Lock(IndexBuffer3D,
                                                            0,
                                                            numIndices * sizeof(RwUInt16),
                                                            (void**)&bufferMem,
                                                            D3DLOCK_DISCARD | D3DLOCK_NOSYSLOCK);
                        }
                        else
                        {
                            hr = IDirect3DIndexBuffer9_Lock(IndexBuffer3D,
                                                            IB3DOffset * sizeof(RwUInt16),
                                                            numIndices * sizeof(RwUInt16),
                                                            (void**)&bufferMem,
                                                            D3DLOCK_NOOVERWRITE | D3DLOCK_NOSYSLOCK);
                        }

                        if(SUCCEEDED(hr))
                        {
                            memcpy(bufferMem, indices, numIndices * sizeof(RwUInt16));

                            IDirect3DIndexBuffer9_Unlock(IndexBuffer3D);

                            RwD3D9SetStreamSource(0, CurrentVertexBuffer3D, 0, stride);

                            _rwD3D9SetIndices(IndexBuffer3D);

                            _rwD3D9RenderStateFlushCache();

                            hr = IDirect3DDevice9_DrawIndexedPrimitive(RwD3DDevice,
                                (D3DPRIMITIVETYPE)primitiveType,
                                                                       CurrentBaseIndex3D,
                                                                       0, numVerts,
                                                                       IB3DOffset, numPrimitives);

                            IB3DOffset += numIndices;
                        }
                    }
                }
            }

            if(FAILED(hr))
            {
                _rwD3D9SetVertexDeclaration(VertexDeclIm3DOld);
                RwD3D9SetStreamSource(0, NULL, 0, 0);
                _rwD3D9SetIndices(NULL);

                _rwD3D9RenderStateFlushCache();

                IDirect3DDevice9_DrawIndexedPrimitiveUP(RwD3DDevice,
                    (D3DPRIMITIVETYPE)primitiveType,
                                                        0,
                                                        numVerts,
                                                        numPrimitives,
                                                        indices, D3DFMT_INDEX16,
                                                        verts,
                                                        sizeof(RwIm3DVertex));
            }
        }
        else
        {
            void* bufferMem;

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


            /* Fill Vertex Buffer */
            if(RwD3D9DynamicVertexBufferLock(stride,
                                             numVerts,
                                             (void**)&CurrentVertexBuffer3D,
                                             &bufferMem,
                                             &CurrentBaseIndex3D))
            {
                float* vertexDes = (float*)bufferMem;
                const float* vertexSrc = (const float*)(verts);
                RwUInt32 i = numVerts;

                if(useTexCoords)
                {
                    do
                    {
                        vertexDes[0] = vertexSrc[0];
                        vertexDes[1] = vertexSrc[1];
                        vertexDes[2] = vertexSrc[2];

                        ((RwUInt32*)vertexDes)[3] = ((const RwUInt32*)vertexSrc)[6];

                        vertexDes[4] = vertexSrc[7];
                        vertexDes[5] = vertexSrc[8];

                        vertexDes += 6;
                        vertexSrc += 9;
                        i--;
                    }
                    while(i);
                }
                else
                {
                    do
                    {
                        vertexDes[0] = vertexSrc[0];
                        vertexDes[1] = vertexSrc[1];
                        vertexDes[2] = vertexSrc[2];

                        ((RwUInt32*)vertexDes)[3] = ((const RwUInt32*)vertexSrc)[6];

                        vertexDes += 4;
                        vertexSrc += 9;
                        i--;
                    }
                    while(i);
                }

                RwD3D9DynamicVertexBufferUnlock(CurrentVertexBuffer3D);

                RwD3D9SetStreamSource(0, CurrentVertexBuffer3D, 0, stride);

                _rwD3D9RenderStateFlushCache();

                hr = IDirect3DDevice9_DrawPrimitive(RwD3DDevice,
                    (D3DPRIMITIVETYPE)primitiveType,
                                                    CurrentBaseIndex3D,
                                                    numPrimitives);
            }

            if(FAILED(hr))
            {
                _rwD3D9SetVertexDeclaration(VertexDeclIm3DOld);
                RwD3D9SetStreamSource(0, NULL, 0, 0);

                _rwD3D9RenderStateFlushCache();

                IDirect3DDevice9_DrawPrimitiveUP(RwD3DDevice,
                    (D3DPRIMITIVETYPE)primitiveType,
                                                 numPrimitives,
                                                 verts,
                                                 sizeof(RwIm3DVertex));
            }
        }

    }

    return TRUE;
}