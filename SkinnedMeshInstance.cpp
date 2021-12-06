#include "RenderWare.h"
#include "CommonD.h"
#include "GTADef.h"
#include "SkinnedMeshInstance.h"
#include "SkinnedMeshPipeline.h"

RwBool _rpD3D9SkinAtomicCreateVertexBuffer(void* object, RxD3D9ResEntryHeader* resEntryHeader)
{
    RpAtomic* atomic;
    RpGeometry* geometry;
    RxD3D9VertexStream* vertexStream;
    RwUInt32                numMeshes;
    RxD3D9InstanceData* instancedData;

    atomic = (RpAtomic*)object;
    geometry = RpAtomicGetGeometry(atomic);

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

    D3DVERTEXELEMENT9 declaration[] =
    {
        {0, 0,  D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,     0},
        {0, 12, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,     0},
        {0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,        0},
        {0, 24, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,       0},
        {0, 36, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL,     0},
        {0, 48, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,      0},
        {0, 60, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT,  0},
        {0, 76, D3DDECLTYPE_UBYTE4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
        D3DDECL_END()
    };

    RwD3D9CreateVertexDeclaration(declaration, &(resEntryHeader->vertexDeclaration));

    vertexStream->stride = 80;

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

struct SimpleVertexSkin
{
    RwV3d           pos;
    RwTexCoords     uv;
    RwRGBA          color;
    RwV3d           normal;
    RwV3d           binormal;
    RwV3d           tangent;
    RwMatrixWeights weights;
    RwUInt32        indices;
};

#define SMALL_FLOAT 1e-12f
#include "RwMath.h"

void _GenerateNormals(SimpleVertexSkin* verticles, unsigned int vertexCount, RpTriangle* triangles, unsigned int triangleCount, bool isTriStrip)
{
    auto fast_abs = [](float x)
    {
        return x > 0 ? x : -x;
    };

    // generate normal for each triangle and vertex in mesh
    for(RwUInt32 i = 0; i < triangleCount; i++)
    {
        auto triangle = triangles[i];
        bool swapIds = isTriStrip;
        auto iA = triangle.vertIndex[0], iB = triangle.vertIndex[1], iC = triangle.vertIndex[2];

        if(swapIds && i % 2 == 0)
        {
            RwUInt16 t = iC;
            iC = iB;
            iB = t;
        }

        auto vA = verticles[iA],
            vB = verticles[iB],
            vC = verticles[iC];

        // tangent vector
        RwV3d edge1 = {
            vB.pos.x - vA.pos.x,
            vB.pos.y - vA.pos.y,
            vB.pos.z - vA.pos.z
        };

        // bitangent vector
        RwV3d edge2 = {
            vC.pos.x - vA.pos.x,
            vC.pos.y - vA.pos.y,
            vC.pos.z - vA.pos.z
        };

        RwTexCoords tex0 = verticles[iA].uv;
        RwTexCoords tex1 = verticles[iB].uv;
        RwTexCoords tex2 = verticles[iC].uv;

        RwV2d uv1 = {tex1.u - tex0.u, tex1.v - tex0.v};
        RwV2d uv2 = {tex2.u - tex0.u, tex2.v - tex0.v};
        float diff = (uv1.x * uv2.y - uv1.y * uv2.x);

        RW::V3d edge01, edge02, cp;
        edge01 = {edge1.x, uv1.x, uv1.y};
        edge02 = {edge2.x, uv2.x, uv2.y};
        cp = edge01.cross(edge02);

        if((RwReal)fast_abs(cp.getX()) > SMALL_FLOAT)
        {
            const RwReal invcpx = 1.f / cp.getX();
            verticles[iA].tangent.x += -cp.getY() * invcpx;
            verticles[iB].tangent.x += -cp.getY() * invcpx;
            verticles[iC].tangent.x += -cp.getY() * invcpx;
        }

        /* y, s, t */
        edge01 = {edge1.y, uv1.x, uv1.y};
        edge02 = {edge2.y, uv2.x, uv2.y};

        cp = edge01.cross(edge02);
        if((RwReal)fast_abs(cp.getX()) > SMALL_FLOAT)
        {
            const RwReal invcpx = 1.f / cp.getX();
            verticles[iA].tangent.y += -cp.getY() * invcpx;
            verticles[iB].tangent.y += -cp.getY() * invcpx;
            verticles[iC].tangent.y += -cp.getY() * invcpx;
        }

        /* z, s, t */
        edge01 = {edge1.z, uv1.x, uv1.y};
        edge02 = {edge2.z, uv2.x, uv2.y};

        cp = edge01.cross(edge02);
        if((RwReal)fast_abs(cp.getX()) > SMALL_FLOAT)
        {
            const RwReal invcpx = 1.f / cp.getX();
            verticles[iA].tangent.z += -cp.getY() * invcpx;
            verticles[iB].tangent.z += -cp.getY() * invcpx;
            verticles[iC].tangent.z += -cp.getY() * invcpx;
        }

        RwV3d normal = {(edge1.y * edge2.z - edge1.z * edge2.y),
                        (edge1.z * edge2.x - edge1.x * edge2.z),
                        (edge1.x * edge2.y - edge1.y * edge2.x)};

        // increase normals of each vertex in triangle 
        verticles[iA].normal = {
            verticles[iA].normal.x + normal.x,
            verticles[iA].normal.y + normal.y,
            verticles[iA].normal.z + normal.z
        };
        verticles[iB].normal = {
            verticles[iB].normal.x + normal.x,
            verticles[iB].normal.y + normal.y,
            verticles[iB].normal.z + normal.z
        };
        verticles[iC].normal = {
            verticles[iC].normal.x + normal.x,
            verticles[iC].normal.y + normal.y,
            verticles[iC].normal.z + normal.z
        };
    }

    for(RwUInt32 i = 0; i < vertexCount; i++)
    {
        RwReal length = sqrt(verticles[i].normal.x * verticles[i].normal.x +
                             verticles[i].normal.y * verticles[i].normal.y +
                             verticles[i].normal.z * verticles[i].normal.z);
        if(length > 0.001f)
            verticles[i].normal = {verticles[i].normal.x / length,
                                   verticles[i].normal.y / length,
                                   verticles[i].normal.z / length};
        RW::V3d t = verticles[i].tangent;
        t.normalize();
      
        verticles[i].tangent = t.getRWVector();
        verticles[i].binormal = t.cross(verticles[i].normal).getRWVector();
    }
}

void _GenerateTangentsOnly(SimpleVertexSkin* verticles,
                                                 unsigned int  vertexCount,
                                                 RpTriangle* triangles,
                                                 unsigned int  triangleCount,
                                                 bool          isTriStrip)
{
    // generate normal for each triangle and vertex in mesh
    for(RwUInt32 i = 0; i < triangleCount; i++)
    {
        auto triangle = triangles[i];
        bool swapIds = isTriStrip;
        auto iA = triangle.vertIndex[0], iB = triangle.vertIndex[1],
            iC = triangle.vertIndex[2];

        if(swapIds && i % 2 == 0)
        {
            RwUInt16 t = iC;
            iC = iB;
            iB = t;
        }

        auto vA = verticles[iA], vB = verticles[iB], vC = verticles[iC];
        // tangent vector
        RwV3d edge1 = {vB.pos.x - vA.pos.x, vB.pos.y - vA.pos.y,
                       vB.pos.z - vA.pos.z};

        // bitangent vector
        RwV3d edge2 = {vC.pos.x - vA.pos.x, vC.pos.y - vA.pos.y,
                       vC.pos.z - vA.pos.z};

        RwTexCoords tex0 = verticles[iA].uv;
        RwTexCoords tex1 = verticles[iB].uv;
        RwTexCoords tex2 = verticles[iC].uv;

        RwV2d uv1 = {tex1.u - tex0.u, tex1.v - tex0.v};
        RwV2d uv2 = {tex2.u - tex0.u, tex2.v - tex0.v};
        float diff = (uv1.x * uv2.y - uv1.y * uv2.x);
        if(diff < 0.00001F && diff > -0.00001F)
            continue;

        float r = 1.0f / diff;

        RwV3d tangent{((edge1.x * uv2.y) - (edge2.x * uv1.y)) * r,
                      ((edge1.y * uv2.y) - (edge2.y * uv1.y)) * r,
                      ((edge1.z * uv2.y) - (edge2.z * uv1.y)) * r};

        RwV3d bitangent{((edge2.x * uv1.x) - (edge1.x * uv2.x)) * r,
                        ((edge2.y * uv1.x) - (edge1.y * uv2.x)) * r,
                        ((edge2.z * uv1.x) - (edge1.z * uv2.x)) * r};

        verticles[iA].tangent = {verticles[iA].tangent.x + tangent.x,
                                 verticles[iA].tangent.y + tangent.y,
                                 verticles[iA].tangent.z + tangent.z};
        verticles[iB].tangent = {verticles[iB].tangent.x + tangent.x,
                                 verticles[iB].tangent.y + tangent.y,
                                 verticles[iB].tangent.z + tangent.z};
        verticles[iC].tangent = {verticles[iC].tangent.x + tangent.x,
                                 verticles[iC].tangent.y + tangent.y,
                                 verticles[iC].tangent.z + tangent.z};

        verticles[iA].binormal = {verticles[iA].binormal.x + bitangent.x,
                                   verticles[iA].binormal.y + bitangent.y,
                                   verticles[iA].binormal.z + bitangent.z};
        verticles[iB].binormal = {verticles[iB].binormal.x + bitangent.x,
                                   verticles[iB].binormal.y + bitangent.y,
                                   verticles[iB].binormal.z + bitangent.z};
        verticles[iC].binormal = {verticles[iC].binormal.x + bitangent.x,
                                   verticles[iC].binormal.y + bitangent.y,
                                   verticles[iC].binormal.z + bitangent.z};
    }
    // normalize normals
    for(RwUInt32 i = 0; i < vertexCount; i++)
    {
        RW::V3d n = verticles[i].normal;
        RW::V3d t = verticles[i].tangent;
        RW::V3d b = verticles[i].binormal;

        // make tangent orthoganal to normal
        t = t - n * n.dot(t);
        t.normalize();

        if(n.cross(t).dot(b) < 0.0f)
        {
            t = t * -1.0f;
        }
        verticles[i].tangent = {t.getX(), t.getY(), t.getZ()};
    }
}

void RemapIndices(RwUInt32* dst, RwUInt32* mem, RwUInt8* remap)
{
    RwUInt8* dstPosition = reinterpret_cast<RwUInt8*>(dst);
    RwUInt8* src = reinterpret_cast<RwUInt8*>(mem);

    dstPosition[0] = remap[src[0]];
    dstPosition[1] = remap[src[1]];
    dstPosition[2] = remap[src[2]];
    dstPosition[3] = remap[src[3]];
}

RwBool _rpD3D9SkinGeometryReinstance(void* object, RxD3D9ResEntryHeader* resEntryHeader, RwUInt32 lockedSinceLastInst)
{
    const RpAtomic* atomic;
    const RpGeometry* geometry;
    RxD3D9VertexStream* vertexStream;

    RwUInt32                vbSize;
    SimpleVertexSkin* lockedVertexBuffer;

    RpSkin* skin;

    atomic = (const RpAtomic*)object;
    skin = rpSkinGeometryGetSkin(atomic->geometry);
    geometry = (const RpGeometry*)RpAtomicGetGeometry(atomic);

    lockedVertexBuffer = NULL;
    vertexStream = &resEntryHeader->vertexStream[0];

    vbSize = vertexStream->stride * resEntryHeader->totalNumVertex;

    IDirect3DVertexBuffer9* vertexBuffer = reinterpret_cast<IDirect3DVertexBuffer9*>(vertexStream->vertexBuffer);

    vertexBuffer->Lock(vertexStream->offset, vbSize, (void**)&lockedVertexBuffer, D3DLOCK_NOSYSLOCK);

    std::vector<RwUInt8> remap(resEntryHeader->totalNumVertex);
    for(RwUInt8 i = 0; i < static_cast<RwUInt8>(rpSkinGetNumBones(skin)); i++)
        remap[i] = i;

    std::unique_ptr<SimpleVertexSkin> vertexData(new SimpleVertexSkin[resEntryHeader->totalNumVertex]);

    bool hasNormals = geometry->morphTarget[0].normals != nullptr,
        hasTexCoords = geometry->texCoords[0] != nullptr,
        hasColors = geometry->preLitLum && geometry->flags & rpGEOMETRYPRELIT;

    for(RwUInt32 i = 0; i < resEntryHeader->totalNumVertex; i++)
    {
        vertexData.get()[i].pos = geometry->morphTarget[0].verts[i];
        vertexData.get()[i].uv = hasTexCoords ? geometry->texCoords[0][i] : RwTexCoords{0, 0};
        vertexData.get()[i].color = hasColors ? geometry->preLitLum[i] : RwRGBA{255, 255, 255, 255};
        vertexData.get()[i].normal = (hasNormals &&
                                      geometry->morphTarget[0].normals[i].x != NAN &&
                                      geometry->morphTarget[0].normals[i].y != NAN &&
                                      geometry->morphTarget[0].normals[i].z != NAN) ? geometry->morphTarget[0].normals[i] : RwV3d{0.0f, 1.0f, 0.0f};
        vertexData.get()[i].binormal = RwV3d{0.0f, 0.0f, -1.0f};
        vertexData.get()[i].tangent = RwV3d{1.0f, 0.0f, 0.0f};
        vertexData.get()[i].weights = skin->vertexBoneWeights[i];
        RemapIndices(&vertexData.get()[i].indices, &skin->vertexBoneIndices[i], remap.data());
    }

    if(!hasNormals)
        _GenerateNormals(vertexData.get(), resEntryHeader->totalNumVertex, geometry->triangles, geometry->numTriangles, false);
    else
        _GenerateTangentsOnly(vertexData.get(), resEntryHeader->totalNumVertex,
                              geometry->triangles, geometry->numTriangles, false);


    std::copy(vertexData.get(), vertexData.get() + resEntryHeader->totalNumVertex, lockedVertexBuffer);
    vertexBuffer->Unlock();

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

    /* initGraphics the vertex buffers pointers */
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

    if(skin)
    {
        if(skin->meshBoneRLECount)
            skin->boneCount = skin->boneLimit;
        else
            skin->boneCount = skin->numBoneIds;

        animHier = RpSkinAtomicGetHAnimHierarchy(atomic);

        if(animHier)
        {
            skin->useVS = TRUE;
        }
        else
        {
            skin->useVS = FALSE;
        }
    }

    _rpD3D9SkinGeometryReinstance(atomic, resEntryHeader, rpGEOMETRYLOCKALL);
   
    return resEntry;
}

RwBool _rwSkinD3D9AtomicAllInOneNode(RxPipelineNodeInstance* self, const RxPipelineNodeParam* params)
{
    CPerfTimer tim("t");
    tim.Start();
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
    tim.Stop();

    PrintMessage("%s", tim.GetTimerResult().c_str());

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