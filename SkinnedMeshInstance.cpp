#include "RenderWare.h"
#include "CommonD.h"
#include "GTADef.h"
#include "SkinnedMeshInstance.h"
#include "SkinnedMeshPipeline.h"

RwBool _rpD3D9SkinAtomicCreateVertexBuffer(void* object, RxD3D9ResEntryHeader* resEntryHeader)
{
    D3DVERTEXELEMENT9       declaration[20];
    const RpAtomic* atomic;
    const RpGeometry* geometry;
    RpGeometryFlag          flags;
    RpD3D9GeometryUsageFlag usageFlags;
    RwUInt32                numTextureCoords;
    RxD3D9VertexStream* vertexStream;
    RwUInt32                declarationIndex;
    RwUInt32                numMeshes;
    RxD3D9InstanceData* instancedData;

    atomic = (const RpAtomic*)object;
    geometry = (const RpGeometry*)RpAtomicGetGeometry(atomic);
    flags = (RpGeometryFlag)RpGeometryGetFlags(geometry);
    usageFlags = RpD3D9GeometryGetUsageFlags(geometry);

    numTextureCoords = RpGeometryGetNumTexCoordSets(geometry);
    resEntryHeader->totalNumVertex = geometry->numVertices;
    vertexStream = &resEntryHeader->vertexStream[0];

    if(vertexStream->vertexBuffer)
    {
        RwD3D9DestroyVertexBuffer(vertexStream->stride,
                                  vertexStream->stride * resEntryHeader->totalNumVertex,
                                  vertexStream->vertexBuffer,
                                  vertexStream->offset);
        vertexStream->offset = 0;
        vertexStream->stride = 0;
        vertexStream->geometryFlags = 0;
        vertexStream->managed = FALSE;
        vertexStream->dynamicLock = FALSE;
        vertexStream->vertexBuffer = NULL;
    }

    declarationIndex = 0;

    declaration[declarationIndex].Stream = 0;
    declaration[declarationIndex].Offset = vertexStream->stride;
    declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT3;
    declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
    declaration[declarationIndex].Usage = D3DDECLUSAGE_POSITION;
    declaration[declarationIndex].UsageIndex = 0;
    declarationIndex++;
    vertexStream->stride = sizeof(RwV3d);
    vertexStream->geometryFlags = rpGEOMETRYLOCKVERTICES;

    /* Normals */
    if(flags & rxGEOMETRY_NORMALS)
    {
        declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = vertexStream->stride;
        declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT3;
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_NORMAL;
        declaration[declarationIndex].UsageIndex = 0;
        declarationIndex++;
        vertexStream->geometryFlags |= rpGEOMETRYLOCKNORMALS;
        vertexStream->stride += sizeof(RwV3d);
    }

    /* Pre-lighting */
    if(flags & rxGEOMETRY_PRELIT)
    {
        declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = vertexStream->stride;
        declaration[declarationIndex].Type = D3DDECLTYPE_D3DCOLOR;
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_COLOR;
        declaration[declarationIndex].UsageIndex = 0;
        declarationIndex++;
        vertexStream->stride += sizeof(RwRGBA);
        vertexStream->geometryFlags |= rpGEOMETRYLOCKPRELIGHT;
    }

    /* Texture coordinates */
    for(RwUInt32 n = 0; n < numTextureCoords; n++)
    {
        declaration[declarationIndex].Stream = 0;
        declaration[declarationIndex].Offset = vertexStream->stride;
        declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT2;
        declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
        declaration[declarationIndex].Usage = D3DDECLUSAGE_TEXCOORD;
        declaration[declarationIndex].UsageIndex = n;
        declarationIndex++;
        vertexStream->stride += sizeof(RwV2d);
        vertexStream->geometryFlags |= (rpGEOMETRYLOCKTEXCOORDS1 << n);
    }

    /* Tangents */
    //if(flags & rxGEOMETRY_NORMALS/*(usageFlags & rpD3D9GEOMETRYUSAGE_CREATETANGENTS) != 0*/ &&
    //   numTextureCoords > 0)
    //{
    declaration[declarationIndex].Stream = 0;
    declaration[declarationIndex].Offset = vertexStream->stride;
    declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT3;
    declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
    declaration[declarationIndex].Usage = D3DDECLUSAGE_TANGENT;
    declaration[declarationIndex].UsageIndex = 0;
    declarationIndex++;
    vertexStream->stride += sizeof(RwV3d);
    //}

    /* Indices */
    declaration[declarationIndex].Stream = 0;
    declaration[declarationIndex].Offset = vertexStream->stride;
    declaration[declarationIndex].Type = D3DDECLTYPE_UBYTE4;
    declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
    declaration[declarationIndex].Usage = D3DDECLUSAGE_BLENDINDICES;
    declaration[declarationIndex].UsageIndex = 0;
    declarationIndex++;
    vertexStream->stride += sizeof(RwUInt32);

    /* Weight */
    declaration[declarationIndex].Stream = 0;
    declaration[declarationIndex].Offset = vertexStream->stride;
    declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT4;
    declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
    declaration[declarationIndex].Usage = D3DDECLUSAGE_BLENDWEIGHT;
    declaration[declarationIndex].UsageIndex = 0;
    declarationIndex++;
    vertexStream->stride += sizeof(RwV4d);

    /* End of declaration */
    declaration[declarationIndex].Stream = 0xFF;
    declaration[declarationIndex].Offset = 0;
    declaration[declarationIndex].Type = D3DDECLTYPE_UNUSED;
    declaration[declarationIndex].Method = 0;
    declaration[declarationIndex].Usage = 0;
    declaration[declarationIndex].UsageIndex = 0;

    RwD3D9CreateVertexDeclaration(declaration,
                                  &(resEntryHeader->vertexDeclaration));

    vertexStream->managed = TRUE;
    if(FALSE == RwD3D9CreateVertexBuffer(vertexStream->stride,
                                         vertexStream->stride * resEntryHeader->totalNumVertex,
                                         &vertexStream->vertexBuffer,
                                         &vertexStream->offset))
    {
        return FALSE;
    }

    resEntryHeader->useOffsets = FALSE;

    numMeshes = resEntryHeader->numMeshes;
    instancedData = (RxD3D9InstanceData*)(resEntryHeader + 1);

    do
    {
        instancedData->baseIndex = instancedData->minVert +
            (vertexStream->offset / vertexStream->stride);


        instancedData++;
    }
    while(--numMeshes);


    return TRUE;
}

#include "DefaultPipeline.h"

RwBool _rpD3D9SkinGeometryReinstance(void* object, RxD3D9ResEntryHeader* resEntryHeader, RwUInt32 lockedSinceLastInst)
{

    D3DVERTEXELEMENT9       declaration[20];
    const RpAtomic* atomic;
    const RpGeometry* geometry;
    RpGeometryFlag          flags;
    RpD3D9GeometryUsageFlag usageFlags;
    RwUInt32                numTextureCoords;
    RxD3D9VertexStream* vertexStream;
    RwUInt32                numStreams, stream;
    RwUInt32                vbSize;
    RwUInt8* lockedVertexBuffer;
    RwUInt32                declarationIndex, n;

    RwUInt32                numMeshes;
    RxD3D9InstanceData* instancedData;
    RpSkin* skin;

    atomic = (const RpAtomic*)object;
    skin = rpSkinGeometryGetSkin(atomic->geometry);
    geometry = (const RpGeometry*)RpAtomicGetGeometry(atomic);
    flags = (RpGeometryFlag)RpGeometryGetFlags(geometry);
    usageFlags = RpD3D9GeometryGetUsageFlags(geometry);
    numTextureCoords = RpGeometryGetNumTexCoordSets(geometry);

    IDirect3DVertexDeclaration9_GetDeclaration((LPDIRECT3DVERTEXDECLARATION9)resEntryHeader->vertexDeclaration,
                                               declaration,
                                               &n);

    lockedVertexBuffer = NULL;
    vertexStream = &(resEntryHeader->vertexStream[0]);

    if(rpGEOMETRYLOCKALL == lockedSinceLastInst ||
        (lockedSinceLastInst & vertexStream->geometryFlags) != 0)
    {
        vbSize = vertexStream->stride * (resEntryHeader->totalNumVertex);

        IDirect3DVertexBuffer9_Lock((LPDIRECT3DVERTEXBUFFER9)vertexStream->vertexBuffer,
                                    vertexStream->offset, vbSize, (void**)&(lockedVertexBuffer),
                                    D3DLOCK_NOSYSLOCK);
    }

    declarationIndex = 0;

    /* Positions */
    if(lockedSinceLastInst & rpGEOMETRYLOCKVERTICES)
    {
        const RwV3d* pos;

        pos = geometry->morphTarget[0].verts;

        /* Find positions */
        declarationIndex = 0;
        while(declaration[declarationIndex].Usage != D3DDECLUSAGE_POSITION ||
              declaration[declarationIndex].UsageIndex != 0)
        {
            declarationIndex++;
        }

        _rpD3D9VertexDeclarationInstV3d(declaration[declarationIndex].Type,
                                        lockedVertexBuffer +
                                        declaration[declarationIndex].Offset,
                                        pos,
                                        resEntryHeader->totalNumVertex,
                                        vertexStream->stride);
    }

    /* Indices and Weight */
    if(skin && skin->useVS
       && (lockedSinceLastInst & rpGEOMETRYLOCKALL))
    {

        /* Find Weight */
        declarationIndex = 0;
        while(declaration[declarationIndex].Usage != D3DDECLUSAGE_BLENDWEIGHT ||
              declaration[declarationIndex].UsageIndex != 0)
        {
            declarationIndex++;
        }

        RwUInt32 n = 0;

        if(rpSkinGetMeshBoneRLECount(skin) > 0)
            MessageBox(0, "BoneRLE not implemented", "error", MB_OK | MB_ICONERROR);

        _rpD3D9VertexDeclarationInstWeights(declaration[declarationIndex].Type,
                                            lockedVertexBuffer +
                                            declaration[declarationIndex].Offset,
                                            (RwV4d*)rpSkinGetVertexBoneWeights(skin),
                                            resEntryHeader->totalNumVertex,
                                            vertexStream->stride);
        /* Find Indices */
        declarationIndex = 0;
        while(declaration[declarationIndex].Usage != D3DDECLUSAGE_BLENDINDICES ||
              declaration[declarationIndex].UsageIndex != 0)
        {
            declarationIndex++;
        }

        RwUInt8 remapIndices[252];
        memset(remapIndices, 0, sizeof(remapIndices));

        for(RwUInt8 i = 0; i < rpSkinGetNumBones(skin); i++)
            remapIndices[i] = i;

        _rpD3D9VertexDeclarationInstIndicesRemap(declaration[declarationIndex].Type,
                                                 lockedVertexBuffer +
                                                 declaration[declarationIndex].Offset,
                                                 rpSkinGetVertexBoneIndices(skin),
                                                 remapIndices,
                                                 resEntryHeader->totalNumVertex,
                                                 vertexStream->stride);
    }

    /* Normals */
    if(flags & rxGEOMETRY_NORMALS)
    {
        if(lockedSinceLastInst & rpGEOMETRYLOCKNORMALS)
        {
            RwV3d* pos;
            RwV3d* normal;
            RwTexCoords* texCoord;

            pos = geometry->morphTarget[0].verts;
            normal = geometry->morphTarget[0].normals;

            texCoord = (RwTexCoords*)(geometry->texCoords[numTextureCoords - 1]);
            //_rpD3D9VertexDeclarationInstNormal(pos, normal, texCoord, resEntryHeader);

            /* Find normals */
            declarationIndex = 0;
            while(declaration[declarationIndex].Usage != D3DDECLUSAGE_NORMAL ||
                  declaration[declarationIndex].UsageIndex != 0)
            {
                declarationIndex++;
            }

            _rpD3D9VertexDeclarationInstV3d(declaration[declarationIndex].Type,
                                            lockedVertexBuffer +
                                            declaration[declarationIndex].Offset,
                                            normal,
                                            resEntryHeader->totalNumVertex,
                                            vertexStream->stride);
        }
    }

    /* Pre-lighting */
    if(flags & rxGEOMETRY_PRELIT)
    {
        if(lockedSinceLastInst & rpGEOMETRYLOCKPRELIGHT)
        {
            RwRGBA* color;
            RwRGBA defaultColor;
            RwUInt32 stride;

            /* Find prelit */
            declarationIndex = 0;
            while(declaration[declarationIndex].Usage != D3DDECLUSAGE_COLOR ||
                  declaration[declarationIndex].UsageIndex != 0)
            {
                declarationIndex++;
            }

            defaultColor = {255, 255, 255, 255};
            color = (RwRGBA*)geometry->preLitLum ? geometry->preLitLum : &defaultColor;

            stride = vertexStream->stride;

            numMeshes = resEntryHeader->numMeshes;
            instancedData = (RxD3D9InstanceData*)(resEntryHeader + 1);

            do
            {
                instancedData->vertexAlpha = _rpD3D9VertexDeclarationInstColor(lockedVertexBuffer +
                                                                               declaration[declarationIndex].Offset +
                                                                               ((instancedData->minVert) * stride),
                                                                               color + instancedData->minVert,
                                                                               instancedData->numVertices,
                                                                               stride);

                instancedData++;
            }
            while(--numMeshes);
        }
    }

    /* Texture coordinates */
    if(numTextureCoords)
    {
        RwUInt32 i;

        if(lockedSinceLastInst & rpGEOMETRYLOCKTEXCOORDSALL)
        {
            const RwV2d* texCoord;

            for(i = 0; i < numTextureCoords; i++)
            {
                if(lockedSinceLastInst & (rpGEOMETRYLOCKTEXCOORDS1 << i))
                {
                    texCoord = (const RwV2d*)(geometry->texCoords[i]);

                    /* Find tex coords */
                    declarationIndex = 0;
                    while(declaration[declarationIndex].Usage != D3DDECLUSAGE_TEXCOORD ||
                          declaration[declarationIndex].UsageIndex != i)
                    {
                        declarationIndex++;
                    }

                    _rpD3D9VertexDeclarationInstV2d(declaration[declarationIndex].Type,
                                                    lockedVertexBuffer +
                                                    declaration[declarationIndex].Offset,
                                                    texCoord,
                                                    resEntryHeader->totalNumVertex,
                                                    vertexStream->stride);
                }
            }
        }
    }

    /* Tangents */
    //if(flags & rxGEOMETRY_NORMALS/*(usageFlags & rpD3D9GEOMETRYUSAGE_CREATETANGENTS) != 0 */ &&
    //   numTextureCoords > 0)
    //{
    if(lockedSinceLastInst & (rpGEOMETRYLOCKVERTICES | rpGEOMETRYLOCKTEXCOORDSALL))
    {
        const RwV3d* pos;
        const RwV3d* normal;
        const RwTexCoords* texCoord;

        /* Find tangents */
        declarationIndex = 0;
        while(declaration[declarationIndex].Usage != D3DDECLUSAGE_TANGENT ||
              declaration[declarationIndex].UsageIndex != 0)
        {
            declarationIndex++;
        }

        pos = geometry->morphTarget[0].verts;
        normal = geometry->morphTarget[0].normals;
        texCoord = geometry->texCoords[numTextureCoords - 1];

        _rpD3D9VertexDeclarationInstTangent(declaration[declarationIndex].Type,
                                            lockedVertexBuffer +
                                            declaration[declarationIndex].Offset,
                                            pos,
                                            texCoord,
                                            resEntryHeader,
                                            vertexStream->stride);
    }
    //}

    if(lockedVertexBuffer != NULL)
    {
        IDirect3DVertexBuffer9_Unlock((LPDIRECT3DVERTEXBUFFER9)resEntryHeader->vertexStream[0].vertexBuffer);
    }

    return TRUE;
}

D3DPRIMITIVETYPE* _RwD3D9SkinPrimConvTable = (D3DPRIMITIVETYPE*)0x882D74;

RwResEntry* _rxD3D9SkinInstance(RpAtomic* atomic, void* owner, RwResEntry** resEntryPointer, RpMeshHeader* meshHeader)
{
    RxD3D9ResEntryHeader* resEntryHeader;
    RxD3D9InstanceData* instancedData;
    RwResEntry* resEntry;
    const RpMesh* mesh;
    RwInt32               numMeshes;
    RwUInt32              size, n;
    RwUInt32              startIndex;
    RxVertexIndex* indexBuffer;

    RpSkin* skin;
    RpHAnimHierarchy* animHier;
    RpGeometry* geometry;

    startIndex = 0;

    geometry = RpAtomicGetGeometry(atomic);
    skin = rpSkinGeometryGetSkin(geometry);

    /* RxD3D9ResEntryHeader, stores serialNumber & numMeshes */
    size = sizeof(RxD3D9ResEntryHeader);

    /* RxD3D9InstanceData structures, one for each mesh */
    size += sizeof(RxD3D9InstanceData) * meshHeader->numMeshes;

    auto ResourceEntryInstanceDataDestroy = [](RwResEntry* repEntry)
    {
        rwD3D9ResourceEntryInstanceDataDestroy(repEntry);
    };

    resEntry = rwResourcesAllocateResEntry(owner,
                                           resEntryPointer,
                                           size,
                                           ResourceEntryInstanceDataDestroy);

    /* Blank the RxD3D9ResEntryHeader & RxD3D9InstanceData's to '0' */
    memset((resEntry + 1), 0, size);


    resEntryHeader = (RxD3D9ResEntryHeader*)(resEntry + 1);
    resEntryHeader->serialNumber = meshHeader->serialNum;
    resEntryHeader->numMeshes = meshHeader->numMeshes;

    indexBuffer = NULL;
    resEntryHeader->indexBuffer = NULL;
    resEntryHeader->totalNumIndex = 0;

    /* Calculate total num indices */
    mesh = (const RpMesh*)(meshHeader + 1);
    numMeshes = meshHeader->numMeshes;
    while(numMeshes--)
    {
        resEntryHeader->totalNumIndex += mesh->numIndices;

        mesh++;
    }

    /* Create index buffer */
    if(_rwD3D9IndexBufferCreate(resEntryHeader->totalNumIndex, &(resEntryHeader->indexBuffer)))
    {
        IDirect3DIndexBuffer9_Lock((LPDIRECT3DINDEXBUFFER9)resEntryHeader->indexBuffer,
                                   0, 0, (void**)&indexBuffer, 0);
    }

    resEntryHeader->primType = _RwD3D9SkinPrimConvTable[RpMeshHeaderGetPrimType(meshHeader)];

    /* Initialize the vertex buffers pointers */
    for(n = 0; n < RWD3D9_MAX_VERTEX_STREAMS; n++)
    {
        resEntryHeader->vertexStream[n].vertexBuffer = NULL;
        resEntryHeader->vertexStream[n].offset = 0;
        resEntryHeader->vertexStream[n].stride = 0;
        resEntryHeader->vertexStream[n].geometryFlags = FALSE;
        resEntryHeader->vertexStream[n].managed = FALSE;
        resEntryHeader->vertexStream[n].dynamicLock = FALSE;
    }

    /* Vertex declaration */
    resEntryHeader->vertexDeclaration = NULL;

    /* Get the first RxD3D9InstanceData pointer */
    instancedData = (RxD3D9InstanceData*)(resEntryHeader + 1);

    mesh = (const RpMesh*)(meshHeader + 1);
    numMeshes = meshHeader->numMeshes;
    while(numMeshes--)
    {
        RwUInt32 numIndices = mesh->numIndices;

        rwD3D9MeshGetNumVerticesMinIndex(mesh->indices, numIndices,
                                         &instancedData->numVertices,
                                         &instancedData->minVert);

        instancedData->material = mesh->material;
        instancedData->vertexShader = NULL;
        instancedData->vertexAlpha = FALSE;
        instancedData->numIndex = numIndices;

        if(indexBuffer != NULL)
        {
            instancedData->startIndex = startIndex;

            if(instancedData->minVert)
            {
                RxVertexIndex* indexSrc;
                RxVertexIndex* indexDst;
                RwUInt32        n;

                indexSrc = mesh->indices;
                indexDst = indexBuffer;

                n = numIndices;
                while(n--)
                {
                    *indexDst = (RxVertexIndex)((*indexSrc) - (RxVertexIndex)instancedData->minVert);

                    indexSrc++;
                    indexDst++;
                }
            }
            else
            {
                memcpy(indexBuffer, mesh->indices, sizeof(RxVertexIndex) * numIndices);
            }

            if(resEntryHeader->primType == D3DPT_TRIANGLELIST)
            {
                _rwD3D9SortTriListIndices(indexBuffer, numIndices);
            }

            indexBuffer += numIndices;
            startIndex += numIndices;
        }
        else
        {
            instancedData->numIndex = 0;
            instancedData->startIndex = 0;
        }

        switch(resEntryHeader->primType)
        {
            case D3DPT_LINELIST:
            {
                instancedData->numPrimitives = numIndices / 2;
                break;
            }
            case D3DPT_LINESTRIP:
            {
                instancedData->numPrimitives = numIndices - 1;
                break;
            }
            case D3DPT_TRIANGLELIST:
            {
                instancedData->numPrimitives = numIndices / 3;
                break;
            }
            case D3DPT_TRIANGLESTRIP:
            {
                instancedData->numPrimitives = numIndices - 2;
                break;
            }
            case D3DPT_TRIANGLEFAN:
            {
                instancedData->numPrimitives = numIndices - 2;
                break;
            }

            default:
                instancedData->numPrimitives = 0;
                break;
        }

        instancedData->baseIndex = 0;

        instancedData++;
        mesh++;
    }

    resEntryHeader->totalNumVertex = geometry->numVertices;

    if(indexBuffer != NULL)
    {
        IDirect3DIndexBuffer9_Unlock((LPDIRECT3DINDEXBUFFER9)resEntryHeader->indexBuffer);
    }

    _rpD3D9SkinAtomicCreateVertexBuffer(atomic, resEntryHeader);

    if (skin)
    {
        skin->boneCount = skin->numBoneIds;
        skin->useVS = TRUE;
    }

    _rpD3D9SkinGeometryReinstance(atomic, resEntryHeader, rpGEOMETRYLOCKALL);

    return resEntry;
}

RwBool _rwSkinD3D9AtomicAllInOneNode(RxPipelineNodeInstance* self, const RxPipelineNodeParam* params)
{
    RpAtomic* atomic;
    RpGeometry* geometry;
    RwResEntry* resEntry;
    RpMeshHeader* meshHeader;
    RxD3D9ResEntryHeader* resEntryHeader;

    atomic = (RpAtomic*)RxPipelineNodeParamGetData(params);
    geometry = RpAtomicGetGeometry(atomic);
    meshHeader = geometry->mesh;

    if(geometry->numVertices <= 0 ||
       meshHeader->numMeshes <= 0)
    {
        return TRUE;
    }

    /* If the geometry has more than one morph target the resEntry in the
     * atomic is used else the resEntry in the geometry */
    if(RpGeometryGetNumMorphTargets(geometry) != 1)
    {
        resEntry = atomic->repEntry;
    }
    else
    {
        resEntry = geometry->repEntry;
    }

    /* If the meshes have changed we should re-instance */
    if(resEntry)
    {
        resEntryHeader = (RxD3D9ResEntryHeader*)(resEntry + 1);
        if(resEntryHeader->serialNumber != meshHeader->serialNum)
        {
            RwResourcesFreeResEntry(resEntry);
            resEntry = NULL;
        }
    }

    /* Check to see if a resource entry already exists */
    if(resEntry)
    {
        RwResourcesUseResEntry(resEntry);
    }
    else
    {
        RwResEntry** resEntryPointer;
        void* owner;

        meshHeader = geometry->mesh;

        if(RpGeometryGetNumMorphTargets(geometry) != 1)
        {
            owner = (void*)atomic;
            resEntryPointer = &atomic->repEntry;
        }
        else
        {
            owner = (void*)geometry;
            resEntryPointer = &geometry->repEntry;
        }

        resEntry = _rxD3D9SkinInstance(atomic, owner, resEntryPointer, meshHeader);
    }

    if(geometry->lockedSinceLastInst)
    {
        resEntryHeader = (RxD3D9ResEntryHeader*)(resEntry + 1);

        _rpD3D9SkinGeometryReinstance(atomic,
                                      resEntryHeader,
                                      geometry->lockedSinceLastInst);

        geometry->lockedSinceLastInst = 0;
    }

    atomic->interpolator.flags &= ~rpINTERPOLATORDIRTYINSTANCE;

    SkinnedMeshPipe->RenderCallBack(geometry->repEntry, atomic, rpATOMIC, rxGEOMETRY_TEXTURED2 | rxGEOMETRY_TEXTURED);

    return TRUE;
}

//RxNodeDefinition* RxNodeDefinitionGetD3D9SkinAtomicAllInOne()
//{
//    static RwChar _AtomicInstance_csl[] = "nodeDefinitionGetD3D9SkinAtomicAllInOne.csl";
//
//    static RxNodeDefinition nodeD3D9VertexShaderAtomicAllInOneCSL = { /* */
//        _AtomicInstance_csl,                        /* Name */
//        {                                           /* Nodemethods */
//            _rwSkinD3D9AtomicAllInOneNode,          /* +-- nodebody */
//            NULL,                                   /* +-- nodeinit */
//            NULL,                                   /* +-- nodeterm */
//            NULL,                                   /* +-- pipelinenodeinit */
//            NULL,                                   /* +-- pipelineNodeTerm */
//            NULL,                                   /* +-- pipelineNodeConfig */
//            NULL,                                   /* +-- configMsgHandler */
//        },
//        {                                           /* Io */
//            NULL,                                   /* +-- NumClustersOfInterest */
//            NULL,                                   /* +-- ClustersOfInterest */
//            NULL,                                   /* +-- InputRequirements */
//            NULL,                                   /* +-- NumOutputs */
//            NULL                                    /* +-- Outputs */
//        },
//        0,                                          /* PipelineNodePrivateDataSize */
//        (RxNodeDefEditable)FALSE,                   /* editable */
//        0                                           /* inPipes */
//    };
//
//    return &nodeD3D9VertexShaderAtomicAllInOneCSL;
//}