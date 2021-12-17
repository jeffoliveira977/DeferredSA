#include "DefaultPipeline.h"
#include "GTADef.h"

#include <iostream>
#include <fstream>
using namespace std;
void RwV3dCrossProduct2(RwV3d* out, RwV3d* ina, RwV3d* inb)
{
    out->x = inb->z * ina->y - ina->z * inb->y;
    out->y = ina->z * inb->x - inb->z * ina->x;
    out->z = ina->x * inb->y - inb->x * ina->y;
}
void RwV3dCrossProduct2(RwV3d* inaout, RwV3d* inb)
{
    RwV3d ina;
    ina.x = inaout->x;
    ina.y = inaout->y;
    ina.z = inaout->z;
    inaout->x = inb->z * ina.y - ina.z * inb->y;
    inaout->y = ina.z * inb->x - inb->z * ina.x;
    inaout->z = ina.x * inb->y - inb->x * ina.y;
}
float RwV3dNormalize2(RwV3d* out, RwV3d* in)
{
    float w = sqrt(in->x * in->x + in->y * in->y + in->z * in->z);
    if(w > FLT_EPSILON)
    {
        out->x = in->x / w;
        out->y = in->y / w;
        out->z = in->z / w;
    }
    else
        w = 0.0;
    return w;
}
float RwV3dNormalize2(RwV3d* inout)
{
    float w = sqrt(inout->x * inout->x + inout->y * inout->y + inout->z * inout->z);
    if(w > FLT_EPSILON)
    {
        inout->x /= w;
        inout->y /= w;
        inout->z /= w;
    }
    else
        w = 0.0;
    return w;
}
void RwV3dAdd2(RwV3d* out, RwV3d* ina, RwV3d* inb)
{
    out->x = ina->x + inb->x;
    out->y = ina->y + inb->y;
    out->z = ina->z + inb->z;
}
void RwV3dAdd2(RwV3d* inaout, RwV3d* inb)
{
    inaout->x += inb->x;
    inaout->y += inb->y;
    inaout->z += inb->z;
}
void RwV3dSub2(RwV3d* out, RwV3d* ina, RwV3d* inb)
{
    out->x = ina->x - inb->x;
    out->y = ina->y - inb->y;
    out->z = ina->z - inb->z;
}
void RwV3dSub2(RwV3d* inaout, RwV3d* inb)
{
    inaout->x -= inb->x;
    inaout->y -= inb->y;
    inaout->z -= inb->z;
}
void RwV3dNegate2(RwV3d* out, RwV3d* in)
{
    out->x = -in->x;
    out->y = -in->y;
    out->z = -in->z;
}
void RwV3dNegate2(RwV3d* inout)
{
    inout->x = -inout->x;
    inout->y = -inout->y;
    inout->z = -inout->z;
}

void GenerateNormals(RpTriangle*triangles, RwV3d*normals, RwV3d*verts, RwUInt32 numVertices, RwUInt32 numFaces)
{
   // RwV3d vec1, vec2, normal;
   // RwV3d vert1, vert2, vert3;

   // //memset(normals, 0, numVertices * 12);
   ///* for(RwUInt32 n = 0; n < numVertices; n++)
   // {
   //     normals[n] = (normals[n].x != NAN && normals[n].y != NAN && normals[n].z != NAN) ? normals[n] : RwV3d{0, 0, 0};
   // }*/

   // for(RwUInt32 i = 0; i < numFaces; i++)
   // {

   //     RwUInt16   i1, i2, i3;

   //     i1 = triangles[i].vertIndex[0];
   //     i2 = triangles[i].vertIndex[1];
   //     i3 = triangles[i].vertIndex[2];

   //     vert1 = verts[i1];
   //     vert2 = verts[i2];
   //     vert3 = verts[i3];

   //     RwV3dSub2(&vec1, &vert2, &vert1);
   //     RwV3dSub2(&vec2, &vert3, &vert1);

   //     RwV3dCrossProduct2(&normal, &vec1, &vec2);

   //     //_RwV3dNormalize(&normal, &normal);

   //     RwV3dAdd2(&normals[i1], &normals[i1], &normal);
   //     RwV3dAdd2(&normals[i2], &normals[i2], &normal);
   //     RwV3dAdd2(&normals[i3], &normals[i3], &normal);

   // }

   // for(RwUInt32 i = 0; i < numVertices; i++)
   // {
   //     RwV3dNormalize2(&normals[i], &normals[i]);
   // }
    int i, i_triangle, i_vertex, i_normal;
    RwV3d vec1, vec2, normal;
    WORD vert1, vert2, vert3;

    memset(normals, 0, numVertices * 12);
    for(i_triangle = 0; i_triangle < numFaces; i_triangle++)
    {
        //Находим нормаль для треугольника
        vert1 = triangles[i_triangle].vertIndex[0];
        vert2 = triangles[i_triangle].vertIndex[1];
        vert3 = triangles[i_triangle].vertIndex[2];
        RwV3dSub2(&vec1, &verts[vert1], &verts[vert2]);
        RwV3dSub2(&vec2, &verts[vert1], &verts[vert3]);
        RwV3dCrossProduct2(&normal, &vec1, &vec2);
        if(RwV3dNormalize2(&normal) != 0.0)
        {
            for(i_vertex = 0; i_vertex < 3; i_vertex++)
                RwV3dAdd2(&normals[triangles[i_triangle].vertIndex[i_vertex]], &normal);
        }
    }
    //Нормализируем нормали
    for(i_normal = 0; i_normal < numVertices; i_normal++)
    {
        if(RwV3dNormalize2(&normals[i_normal]) == 0.0)
        {
           normals[i_normal].x = 0.0;
            normals[i_normal].y = 0.0;
            normals[i_normal].z = 1.0;
        }
    }
}

RwBool D3D9AtomicDefaultInstanceCallback(void *object, RxD3D9ResEntryHeader *resEntryHeader, RwBool reinstance)
{

    D3DVERTEXELEMENT9   declaration[20];
    const RpAtomic      *atomic;
    const RpGeometry    *geometry;
    RpGeometryFlag      flags;
    unsigned int        usageFlags;
    RwUInt32            numTextureCoords;
    RxD3D9VertexStream *vertexStream;
    RwUInt32            vbSize;
    RwUInt8             *lockedVertexBuffer;
    RwUInt32            declarationIndex, n;
    RwUInt32            lockedSinceLastInst;
    RwUInt32            numMeshes;
    RxD3D9InstanceData *instancedData;

    atomic           = (const RpAtomic *)object;
    geometry         = (const RpGeometry *)RpAtomicGetGeometry(atomic);
    flags            = (RpGeometryFlag)RpGeometryGetFlags(geometry);
    usageFlags       = RpD3D9GeometryGetUsageFlags(geometry);

    numTextureCoords = RpGeometryGetNumTexCoordSets(geometry);

    vertexStream     = &resEntryHeader->vertexStream[0];
    bool	hasNormals = geometry->morphTarget[0].normals != nullptr;
    if(!reinstance)
    {
        resEntryHeader->totalNumVertex = geometry->numVertices;

        if(vertexStream->vertexBuffer != NULL)
        {

            RwD3D9DestroyVertexBuffer(vertexStream->stride,
                                       vertexStream->stride * (resEntryHeader->totalNumVertex),
                                       vertexStream->vertexBuffer,
                                       vertexStream->offset);


            vertexStream->vertexBuffer = NULL;
        }

        vertexStream->offset = 0;
        vertexStream->stride = 0;
        vertexStream->geometryFlags = 0;
        vertexStream->managed = FALSE;
        vertexStream->dynamicLock = FALSE;

        /* Set declarations elements */
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
            vertexStream->stride += sizeof(RwV3d);
            vertexStream->geometryFlags |= rpGEOMETRYLOCKNORMALS;
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
        for(n = 0; n < numTextureCoords; n++)
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
        if((usageFlags & rpD3D9WORLDSECTORUSAGE_CREATETANGENTS) != 0 &&
           numTextureCoords > 0)
        {
            declaration[declarationIndex].Stream = 0;
            declaration[declarationIndex].Offset = vertexStream->stride;
            declaration[declarationIndex].Type = D3DDECLTYPE_FLOAT3;
            declaration[declarationIndex].Method = D3DDECLMETHOD_DEFAULT;
            declaration[declarationIndex].Usage = D3DDECLUSAGE_TANGENT;
            declaration[declarationIndex].UsageIndex = 0;
            declarationIndex++;
            vertexStream->stride += sizeof(RwV3d);
        }

        /* End of declaration */
        declaration[declarationIndex].Stream = 0xFF;
        declaration[declarationIndex].Offset = 0;
        declaration[declarationIndex].Type = D3DDECLTYPE_UNUSED;
        declaration[declarationIndex].Method = 0;
        declaration[declarationIndex].Usage = 0;
        declaration[declarationIndex].UsageIndex = 0;

        RwD3D9CreateVertexDeclaration(declaration,
                                       &resEntryHeader->vertexDeclaration);

        vbSize = (vertexStream->stride) * (resEntryHeader->totalNumVertex);
        vertexStream->managed = TRUE;

        if(FALSE == RwD3D9CreateVertexBuffer(vertexStream->stride, vbSize,
           &vertexStream->vertexBuffer,
           &vertexStream->offset))
        {
            return FALSE;
        }

        resEntryHeader->useOffsets = FALSE;

        numMeshes = resEntryHeader->numMeshes;
        instancedData = (RxD3D9InstanceData *)(resEntryHeader + 1);

        do
        {
            instancedData->baseIndex = instancedData->minVert +
                (resEntryHeader->vertexStream->offset / resEntryHeader->vertexStream->stride);

            instancedData++;
        }
        while(--numMeshes);

    }
    else
    {
        IDirect3DVertexDeclaration9_GetDeclaration((LPDIRECT3DVERTEXDECLARATION9)resEntryHeader->vertexDeclaration,
                                                   declaration,
                                                   &n);
    }

    lockedVertexBuffer = NULL;

    if(!reinstance)
        lockedSinceLastInst = rpGEOMETRYLOCKALL;
    else
        lockedSinceLastInst = geometry->lockedSinceLastInst;

    if(lockedSinceLastInst == rpGEOMETRYLOCKALL || vertexStream->geometryFlags & lockedSinceLastInst)
    {
        vbSize = vertexStream->stride * (resEntryHeader->totalNumVertex);

        IDirect3DVertexBuffer9_Lock((LPDIRECT3DVERTEXBUFFER9)vertexStream->vertexBuffer,
                                    vertexStream->offset, vbSize, (void **)&lockedVertexBuffer,
                                    D3DLOCK_NOSYSLOCK);
    }

    declarationIndex = 0;

    /* Positions */
    if(lockedSinceLastInst & rpGEOMETRYLOCKVERTICES)
    {
        const RwV3d *pos;

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

    /* Normals */
    if(flags & rxGEOMETRY_NORMALS)
    {
        if(lockedSinceLastInst & rpGEOMETRYLOCKNORMALS)
        {
            RwV3d *pos;
            RwV3d *normal;
            RwTexCoords *texCoord;

            normal = geometry->morphTarget[0].normals;
            pos = geometry->morphTarget[0].verts;

            texCoord = (RwTexCoords *)(geometry->texCoords[numTextureCoords - 1]);
           
           // if(normal)
           // _rpD3D9VertexDeclarationInstNormal(pos, normal, texCoord, resEntryHeader);  
            GenerateNormals(geometry->triangles, normal, pos, geometry->numVertices, geometry->numTriangles);
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
            const RwRGBA *color;
            RwUInt32 stride;

            /* Find prelit */
            declarationIndex = 0;
            while(declaration[declarationIndex].Usage != D3DDECLUSAGE_COLOR ||
                  declaration[declarationIndex].UsageIndex != 0)
            {
                declarationIndex++;
            }

            color = (const RwRGBA *)(geometry->preLitLum);
            stride = vertexStream->stride;

            numMeshes = resEntryHeader->numMeshes;
            instancedData = (RxD3D9InstanceData *)(resEntryHeader + 1);

            do
            {
                instancedData->vertexAlpha =
                    _rpD3D9VertexDeclarationInstColor(lockedVertexBuffer +
                                                       declaration[declarationIndex].Offset +
                                                       (instancedData->minVert * stride),
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
        RwUInt32    i;

        if(lockedSinceLastInst & rpGEOMETRYLOCKTEXCOORDSALL)
        {
            const RwV2d *texCoord;

            for(i = 0; i < numTextureCoords; i++)
            {
                if(lockedSinceLastInst & (rpGEOMETRYLOCKTEXCOORDS1 << i))
                {
                    texCoord = (const RwV2d *)(geometry->texCoords[i]);

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
    if((usageFlags & rpD3D9GEOMETRYUSAGE_CREATETANGENTS) != 0 &&
       numTextureCoords > 0)
    {
        if(lockedSinceLastInst & (rpGEOMETRYLOCKVERTICES | rpGEOMETRYLOCKTEXCOORDSALL))
        {
            const RwV3d *pos;
            const RwV3d *normal;
            const RwTexCoords *texCoord;

            /* Find tangents */
            declarationIndex = 0;
            while(declaration[declarationIndex].Usage != D3DDECLUSAGE_TANGENT ||
                  declaration[declarationIndex].UsageIndex != 0)
            {
                declarationIndex++;
            }

            pos = geometry->morphTarget[0].verts;
            normal = geometry->morphTarget[0].normals;
            texCoord = (const RwTexCoords *)(geometry->texCoords[0]);

            _rpD3D9VertexDeclarationInstTangent(declaration[declarationIndex].Type,
                                                lockedVertexBuffer +
                                                declaration[declarationIndex].Offset,
                                                pos,                                          
                                                texCoord,
                                                resEntryHeader,
                                                vertexStream->stride);
        }
    }

    if(lockedVertexBuffer != NULL)
    {
        IDirect3DVertexBuffer9_Unlock((LPDIRECT3DVERTEXBUFFER9)vertexStream->vertexBuffer);
    }

    return TRUE;
}

RwBool D3D9AtomicDefaultReinstanceCallback(void* object,
                                    RwResEntry* resEntry,
                                    RxD3D9AllInOneInstanceCallBack instanceCallback)
{
    const RpAtomic* atomic;
    RpGeometry* geometry;

    atomic = (const RpAtomic*)object;
    geometry = RpAtomicGetGeometry(atomic);

    /* If were a morpth target, morph if were dirty */
    if((RpGeometryGetNumMorphTargets(geometry) != 1))
    {
        const RpInterpolator* interp = &(atomic->interpolator);

        if((interp->flags & rpINTERPOLATORDIRTYINSTANCE) ||
           geometry->lockedSinceLastInst)
        {
            RpGeometryFlag          lockedFlags;
            RxD3D9ResEntryHeader* resEntryHeader;

            lockedFlags = (RpGeometryFlag)geometry->lockedSinceLastInst;

            geometry->lockedSinceLastInst |= (rpGEOMETRYLOCKVERTICES | rpGEOMETRYLOCKNORMALS);

            resEntryHeader = (RxD3D9ResEntryHeader*)(resEntry + 1);

            D3D9AtomicDefaultInstanceCallback(object,
                                              resEntryHeader,
                                              TRUE);

            geometry->lockedSinceLastInst = lockedFlags;
        }
    }
    else
    {
        /*
         * Call the instance callback
         */
        if(instanceCallback)
        {
            RxD3D9ResEntryHeader* resEntryHeader;

            resEntryHeader = (RxD3D9ResEntryHeader*)(resEntry + 1);

            if(!instanceCallback(object,
                                 resEntryHeader,
                                 TRUE))
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

#include "DefaultMeshPipeline.h"
#include "CommonD.h"

void _rxD3D9DefaultRenderCallback(RwResEntry* entry, void* object, RwUInt8 type, RwUInt32 flags)
{
    DefaultMeshPipe->RenderCallBack(entry, object, type, flags);
}

typedef struct __rxD3D9InstanceNodeData _rxD3D9InstanceNodeData;
struct __rxD3D9InstanceNodeData
{
    RxD3D9AllInOneInstanceCallBack      instanceCallback;   /* Instance callback */
    RxD3D9AllInOneReinstanceCallBack    reinstanceCallback; /* Instance callback */
    RxD3D9AllInOneLightingCallBack      lightingCallback;   /* Lighting callback */
    RxD3D9AllInOneRenderCallBack        renderCallback;     /* Render callback */
};

RwBool D3D9AtomicAllInOnePipelineInit(RxPipelineNode* node)
{
    _rxD3D9InstanceNodeData* instanceData;


    instanceData = (_rxD3D9InstanceNodeData*)node->privateData;

    /* Default instance callback */
    instanceData->instanceCallback = D3D9AtomicDefaultInstanceCallback;

    /* Default reinstance callback */
    instanceData->reinstanceCallback = D3D9AtomicDefaultReinstanceCallback;

    auto rwD3D9AtomicDefaultLightingCallback = [](void* object)
    {
        _rwD3D9AtomicDefaultLightingCallback(object);
    };

    /* Default lighting callback */
    instanceData->lightingCallback = (RxD3D9AllInOneLightingCallBack)(rwD3D9AtomicDefaultLightingCallback);

    /* Default render callback */
    instanceData->renderCallback = _rxD3D9DefaultRenderCallback;

    return TRUE;
}

#include "CommonD.h"

#define SMALL_FLOAT 1e-12f

RwUInt32
_rpD3D9VertexDeclarationInstNormal(RwV3d *pos,
                                   RwV3d *normal,
                                   RwTexCoords *texCoord,
                                   RxD3D9ResEntryHeader *meshHeader)
{
    RwUInt32    numIndices, numVertices;
    RwUInt32    n;
    RwUInt32    i1, i2, i3;
    RwV3d       edge01, edge02;
    RwUInt32            numMeshes;
    RxD3D9InstanceData *instancedData;
    const RxVertexIndex *indexBufferOrigin;
    const RxVertexIndex *indexBuffer;


    numVertices = meshHeader->totalNumVertex;

    IDirect3DIndexBuffer9_Lock((LPDIRECT3DINDEXBUFFER9)meshHeader->indexBuffer,
                               0,
                               (meshHeader->totalNumIndex) * sizeof(RwUInt16),
                               (void **)&indexBufferOrigin, 0);

    numMeshes = meshHeader->numMeshes;
    instancedData = (RxD3D9InstanceData *)(meshHeader + 1);

    for(n = 0; n < numVertices; n++)
    {
        normal[n] = (normal[n].x != NAN && normal[n].y != NAN && normal[n].z != NAN) ? normal[n] : RwV3d{0, 0, 0};
    }

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
             RwV3d *vert1;
             RwV3d *vert2;
             RwV3d *vert3;

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

            RwV3dSub2(&edge01, vert2, vert1);
            RwV3dSub2(&edge02, vert3, vert1);

            RwV3d normal2;
            RwV3dCrossProduct2(&normal2, &edge01, &edge02);
            RwV3dNormalize2(&normal2, &normal2);

            RwV3dAdd2(&normal[i1], &normal[i1], &normal2);
            RwV3dAdd2(&normal[i2], &normal[i2], &normal2);
            RwV3dAdd2(&normal[i3], &normal[i3], &normal2);

        }

        instancedData++;
    }
    while(--numMeshes);

    /* Normalize the tangents vectors */
    for(n = 0; n < numVertices; n++)
    {
        RwV3dNormalize2(&normal[n], &normal[n]);
    }

    IDirect3DIndexBuffer9_Unlock((LPDIRECT3DINDEXBUFFER9)meshHeader->indexBuffer);
    return 1;
}