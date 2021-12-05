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
    vertexStream->geometryFlags = rpGEOMETRYLOCKVERTICES | rpGEOMETRYLOCKNORMALS | 
                                  rpGEOMETRYLOCKPRELIGHT | rpGEOMETRYLOCKTEXCOORDS1;

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
#include <memory>
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

RwUInt32
_rpD3D9VertexDeclarationInstTangent( RwV3d* pos,
                                     RwV3d* normal,
                                     RpVertexNormal* packedNormal,
                                     RwTexCoords* texCoord,
                                    RxD3D9ResEntryHeader* meshHeader, RwV3d* tangent)
{
    RwUInt32    numIndices, numVertices;
    RwUInt32    n, j;
    RwReal      unused;
    RwUInt32    i1, i2, i3;
    RwV3d * currentTangent;
    RwV3d       edge01, edge02, cp;
    RwUInt32            numMeshes;
    RxD3D9InstanceData* instancedData;
    const RxVertexIndex* indexBufferOrigin;
    const RxVertexIndex* indexBuffer;
    RwUInt32* tangentsRemapping;

    numVertices = meshHeader->totalNumVertex;

 

    IDirect3DIndexBuffer9_Lock((LPDIRECT3DINDEXBUFFER9)meshHeader->indexBuffer,
                               0,
                               (meshHeader->totalNumIndex) * sizeof(RwUInt16),
                               (void**)&indexBufferOrigin, 0);

    numMeshes = meshHeader->numMeshes;
    instancedData = (RxD3D9InstanceData*)(meshHeader + 1);

    do
    {
        indexBuffer = indexBufferOrigin + instancedData->startIndex;

        numIndices = instancedData->numIndex;

        if(meshHeader->primType == D3DPT_TRIANGLESTRIP)
        {
            numIndices -= 2;
        }

        n = 0;
        while(n < numIndices)
        {
            const RwV3d* vert1;
            const RwV3d* vert2;
            const RwV3d* vert3;
            const RwTexCoords* uv1;
            const RwTexCoords* uv2;
            const RwTexCoords* uv3;
            RwV3d* tang1;
            RwV3d* tang2;
            RwV3d* tang3;

            if(meshHeader->primType == D3DPT_TRIANGLELIST)
            {
                i1 = indexBuffer[n + 0] + instancedData->minVert;
                i2 = indexBuffer[n + 1] + instancedData->minVert;
                i3 = indexBuffer[n + 2] + instancedData->minVert;
                n += 3;
            }
            else /* if (meshHeader->primType == D3DPT_TRIANGLESTRIP) */
            {
                if(n & 0x01)
                {
                    i1 = indexBuffer[n + 2] + instancedData->minVert;
                    i2 = indexBuffer[n + 1] + instancedData->minVert;
                    i3 = indexBuffer[n + 0] + instancedData->minVert;
                }
                else
                {
                    i1 = indexBuffer[n + 0] + instancedData->minVert;
                    i2 = indexBuffer[n + 1] + instancedData->minVert;
                    i3 = indexBuffer[n + 2] + instancedData->minVert;
                }
                n++;

                if(i1 == i2 || i1 == i3 || i2 == i3)
                {
                    continue;
                }
            }

            vert1 = pos + i1;
            vert2 = pos + i2;
            vert3 = pos + i3;

            uv1 = texCoord + i1;
            uv2 = texCoord + i2;
            uv3 = texCoord + i3;

            tang1 = (tangent + i1);
            tang2 = (tangent + i2);
            tang3 = (tangent + i3);

            /* x, s, t */
            edge01.x = (vert2->x - vert1->x);
            edge01.y = (uv2->u - uv1->u);
            edge01.z = (uv2->v - uv1->v);

            edge02.x = (vert3->x - vert1->x);
            edge02.y = (uv3->u - uv1->u);
            edge02.z = (uv3->v - uv1->v);

            RwV3dCrossProductMacro(&cp, &edge01, &edge02);
            if((RwReal)fabs(cp.x) > SMALL_FLOAT)
            {
                const RwReal invcpx = 1.f / cp.x;

                tang1->x += -cp.y * invcpx;

                tang2->x += -cp.y * invcpx;

                tang3->x += -cp.y * invcpx;
            }

            /* y, s, t */
            edge01.x = (vert2->y - vert1->y);

            edge02.x = (vert3->y - vert1->y);

            RwV3dCrossProductMacro(&cp, &edge01, &edge02);
            if((RwReal)fabs(cp.x) > SMALL_FLOAT)
            {
                const RwReal invcpx = 1.f / cp.x;

                tang1->y += -cp.y * invcpx;

                tang2->y += -cp.y * invcpx;

                tang3->y += -cp.y * invcpx;
            }

            /* z, s, t */
            edge01.x = (vert2->z - vert1->z);

            edge02.x = (vert3->z - vert1->z);

            RwV3dCrossProductMacro(&cp, &edge01, &edge02);
            if((RwReal)fabs(cp.x) > SMALL_FLOAT)
            {
                const RwReal invcpx = 1.f / cp.x;

                tang1->z += -cp.y * invcpx;

                tang2->z += -cp.y * invcpx;

                tang3->z += -cp.y * invcpx;
            }
        }

        instancedData++;
    }
    while(--numMeshes);

    IDirect3DIndexBuffer9_Unlock((LPDIRECT3DINDEXBUFFER9)meshHeader->indexBuffer);

    /* Fix for similar vertices tangents */
    tangentsRemapping = (RwUInt32*)RwMalloc(sizeof(RwUInt32) * numVertices,
                                            rwMEMHINTDUR_FUNCTION | rwID_WORLDPIPEMODULE);

    for(n = 0; n < numVertices; n++)
    {
        tangentsRemapping[n] = n;
    }

    /* Lock for similar vertex with a similar tangent */
    for(n = 0; n < numVertices; n++)
    {
        currentTangent = (tangent + n);

        for(j = n + 1; j < numVertices; j++)
        {
            if(tangentsRemapping[j] > n)
            {
                if(fabs(pos[n].x - pos[j].x) <= 0.000001f &&
                   fabs(pos[n].y - pos[j].y) <= 0.000001f &&
                   fabs(pos[n].z - pos[j].z) <= 0.000001f)
                {
                    /* Check same normal */
                    RwReal cosangle;

                    if(normal != NULL)
                    {
                        cosangle = RwV3dDotProductMacro((normal + n), (normal + j));
                    }
                    else
                    {
                        RPV3DFROMVERTEXNORMAL(edge01, packedNormal[n]);
                        RPV3DFROMVERTEXNORMAL(edge02, packedNormal[j]);

                        cosangle = RwV3dDotProductMacro(&edge01, &edge02);
                    }

                    if(cosangle >= 0.9999f)
                    {
                        /* Check similar tangents */
                        cosangle = RwV3dDotProductMacro(currentTangent, (tangent + j));
                        cosangle /= RwV3dLength(currentTangent);
                        cosangle /= RwV3dLength(tangent + j);

                        if(cosangle > 0.7f)
                        {
                            /* accumulate */
                            currentTangent->x += tangent[j].x;
                            currentTangent->y += tangent[j].y;
                            currentTangent->z += tangent[j].z;

                            tangentsRemapping[j] = n;
                        }
                    }
                }
            }
        }
    }

    /* Normalize the tangents vectors */
    for(n = 0; n < numVertices; n++)
    {
        if(tangentsRemapping[n] == n)
        {
            _rwV3dNormalizeMacro(unused, (tangent + n), (tangent + n));
        }
        else
        {
            tangent[n] = tangent[tangentsRemapping[n]];
        }
    }

    RwFree(tangentsRemapping);


    return n;
}

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
        /*if ( tangentsRemapping[i] == i )
        {*/
        t.normalize();
        verticles[i].tangent = t.getRWVector();
    }
    /*
    std::vector<RwUInt32> tangentsRemapping;
    tangentsRemapping.reserve( vertexCount );
    for ( RwUInt32 i = 0; i < vertexCount; i++ )
    {
        tangentsRemapping.push_back( i );
    }
    for ( RwUInt32 i = 0; i < vertexCount; i++ )
    {
        auto &currentTangent = verticles[i].tangent;
        for ( RwUInt32 j = i + 1; j < vertexCount; j++ )
        {
            if ( tangentsRemapping[j] > i )
            {
                if ( fabs( verticles[i].pos.x - verticles[j].pos.x ) <=
                         0.000001f &&
                     fabs( verticles[i].pos.y - verticles[j].pos.y ) <=
                         0.000001f &&
                     fabs( verticles[i].pos.z - verticles[j].pos.z ) <=
                         0.000001f )
                {
                    // Check same normal
                    RwReal cosangle;
                    cosangle = RW::V3d( verticles[i].normal )
                                   .dot( RW::V3d( verticles[j].normal ) );
                    if ( cosangle >= 0.9999f )
                    {
                        // Check similar tangents
                        cosangle = RW::V3d( currentTangent )
                                       .dot( RW::V3d( verticles[j].tangent ) );
                        cosangle /= RW::V3d( currentTangent ).length();
                        cosangle /= RW::V3d( verticles[j].tangent ).length();
                        if ( cosangle > 0.7f )
                        {
                            // accumulate
                            currentTangent.x += verticles[j].tangent.x;
                            currentTangent.y += verticles[j].tangent.y;
                            currentTangent.z += verticles[j].tangent.z;
                            tangentsRemapping[j] = i;
                        }
                    }
                }
            }
        }
    }
    */
    /* normalize normals
    for ( RwUInt32 i = 0; i < vertexCount; i++ )
    {
        RW::V3d t = verticles[i].tangent;
        if ( tangentsRemapping[i] == i )
        {/
            t.normalize();
            verticles[i].tangent = t.getRWVector();
        }
        else
        {
            verticles[i].tangent = verticles[tangentsRemapping[i]].tangent;
        }
    }*/
}

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

    //if(rpGEOMETRYLOCKALL == lockedSinceLastInst ||
    //    (lockedSinceLastInst & vertexStream->geometryFlags) != 0)
    //{
        vbSize = vertexStream->stride * (resEntryHeader->totalNumVertex);

        IDirect3DVertexBuffer9_Lock((LPDIRECT3DVERTEXBUFFER9)vertexStream->vertexBuffer,
                                    vertexStream->offset, vbSize, (void**)&(lockedVertexBuffer),
                                    D3DLOCK_NOSYSLOCK);
   // }

    declarationIndex = 0;

    RwUInt8	indexRemap[252];
    memset(indexRemap, 0, sizeof(indexRemap));

    auto animHier = RpSkinAtomicGetHAnimHierarchy(atomic);

    if(animHier->numNodes > 0)
    {
        for(RwUInt8 i = 0; i < static_cast<RwUInt8>(animHier->numNodes); i++)
            indexRemap[i] = i;
    }

    std::unique_ptr<SimpleVertexSkin> vertexData(new SimpleVertexSkin[resEntryHeader->totalNumVertex]);
   
    
        bool hasNormals = geometry->morphTarget[0].normals != nullptr,
            hasTexCoords = geometry->texCoords[0] != nullptr,
            hasColors = geometry->preLitLum && geometry->flags & rpGEOMETRYPRELIT;

        for(RwUInt32 i = 0; i < resEntryHeader->totalNumVertex; i++)
        {
            vertexData.get()[i].pos = geometry->morphTarget[0].verts[i];
            vertexData.get()[i].normal = (hasNormals &&
                                          geometry->morphTarget[0].normals[i].x != NAN &&
                                          geometry->morphTarget[0].normals[i].y != NAN &&
                                          geometry->morphTarget[0].normals[i].z != NAN) ? geometry->morphTarget[0].normals[i] : RwV3d{0, 0, 0};
            vertexData.get()[i].uv = hasTexCoords ? geometry->texCoords[0][i] : RwTexCoords{0, 0};
            vertexData.get()[i].color = hasColors ? geometry->preLitLum[i] : RwRGBA{255, 255, 255, 255};
            vertexData.get()[i].weights = skin->vertexBoneWeights[i];
            vertexData.get()[i].binormal = RwV3d{1, 0, 0};

            RwUInt8* indicesToRemap = (RwUInt8*)&skin->vertexBoneIndices[i];
            RwUInt8* remapedIndices = (RwUInt8*)&vertexData.get()[i].indices;
            remapedIndices[0] = indexRemap[indicesToRemap[0]];
            remapedIndices[1] = indexRemap[indicesToRemap[1]];
            remapedIndices[2] = indexRemap[indicesToRemap[2]];
            remapedIndices[3] = indexRemap[indicesToRemap[3]];
        }

        if(!hasNormals)
            _GenerateNormals(vertexData.get(), resEntryHeader->totalNumVertex, geometry->triangles, geometry->numTriangles, true);
      
        memcpy(lockedVertexBuffer, vertexData.get(), vbSize);
     

    if(lockedVertexBuffer != NULL)
    {
        IDirect3DVertexBuffer9_Unlock((LPDIRECT3DVERTEXBUFFER9)resEntryHeader->vertexStream[0].vertexBuffer);
    }

    return TRUE;

    /* Positions */
    //if(lockedSinceLastInst & rpGEOMETRYLOCKVERTICES)
    //{
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
   // }

    /* Indices and Weight */
   // if(skin && skin->useVS
   //    && (lockedSinceLastInst & rpGEOMETRYLOCKALL))
   // {

        /* Find Weight */
        declarationIndex = 0;
        while(declaration[declarationIndex].Usage != D3DDECLUSAGE_BLENDWEIGHT ||
              declaration[declarationIndex].UsageIndex != 0)
        {
            declarationIndex++;
        }

    //    RwUInt32 n = 0;

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
   // }

    /* Normals */
    if(flags & rxGEOMETRY_NORMALS)
    {
        //if(lockedSinceLastInst & rpGEOMETRYLOCKNORMALS)
        //{
            RwV3d* pos;
            RwV3d* normal;
            RwTexCoords* texCoord;

            pos = geometry->morphTarget[0].verts;
            normal = geometry->morphTarget[0].normals;

            texCoord = (RwTexCoords*)(geometry->texCoords[numTextureCoords - 1]);
            _rpD3D9VertexDeclarationInstNormal(pos, normal, texCoord, resEntryHeader);

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
        //}
    }

    /* Pre-lighting */
    if(flags & rxGEOMETRY_PRELIT)
    {
        //if(lockedSinceLastInst & rpGEOMETRYLOCKPRELIGHT)
        //{
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
        //}
    }

    /* Texture coordinates */
    if(numTextureCoords)
    {
        RwUInt32 i;

        //if(lockedSinceLastInst & rpGEOMETRYLOCKTEXCOORDSALL)
        //{
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
        //}
    }

    /* Tangents */
    //if(flags & rxGEOMETRY_NORMALS/*(usageFlags & rpD3D9GEOMETRYUSAGE_CREATETANGENTS) != 0 */ &&
    //   numTextureCoords > 0)
    //{
    //if(lockedSinceLastInst & (rpGEOMETRYLOCKVERTICES | rpGEOMETRYLOCKTEXCOORDSALL))
    //{
        //const RwV3d* pos;
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
    //}
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

   /* if(geometry->lockedSinceLastInst)
    {
        resEntryHeader = (RxD3D9ResEntryHeader*)(resEntry + 1);

        _rpD3D9SkinGeometryReinstance(atomic,
                                      resEntryHeader,
                                      geometry->lockedSinceLastInst);

        geometry->lockedSinceLastInst = 0;
    }*/

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