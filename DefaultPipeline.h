#pragma once
#include "common.h"

RwUInt32 _rpD3D9VertexDeclarationInstNormal(RwV3d *pos,
                                            RwV3d *normal,
                                            RwTexCoords *texCoord,
                                            RxD3D9ResEntryHeader *meshHeader);

RwBool D3D9AtomicDefaultInstanceCallback(void *object, RxD3D9ResEntryHeader *resEntryHeader, RwBool reinstance);


RwBool D3D9AtomicDefaultReinstanceCallback(void* object,
                                           RwResEntry* resEntry,
                                           RxD3D9AllInOneInstanceCallBack instanceCallback);

RwBool D3D9AtomicAllInOnePipelineInit(RxPipelineNode* node);

void GenerateNormals(RpTriangle* triangles, RwV3d* normals, RwV3d* verts, RwUInt32 numVertices, RwUInt32 numFaces);