#pragma once

#include "CommonD.h"

#define _RwIm3DTransform(pVerts, numVerts, ltm, flags) ((void *( _cdecl*)(RwIm3DVertex*, RwUInt32, RwMatrix*, RwUInt32))0x7EF450)(pVerts, numVerts, ltm, flags)

#define _RwIm3DRenderPrimitive( primType )                                   ( (RwBool( __cdecl * )( RwPrimitiveType ))0x7EF6B0 )( primType )
#define _RwIm3DEnd() ( (RwBool( __cdecl * )())0x7EF520 )()
#define _RwIm3DRenderIndexedPrimitive(primType, indices, numIndices) ((RwBool(_cdecl*)(RwPrimitiveType, RwImVertexIndex *, RwInt32))0x7EF550 )(primType, indices, numIndices)

#include <DirectXMath.h>
#include "imgui/imgui.h"

struct WaterReflection
{
    bool peds;
    bool vehicles;
    bool buldings;
    bool objects;
};

extern WaterReflection ObjectType;
#include "VertexBuffer.h"
#include "RwIndexBuffer.h"
class CWaterLevel{

private:

  

    static RwFrame* m_envFrame;
    static RwCamera* m_envCamera;
    static RwRaster* m_zRaster;
    static DirectX::XMMATRIX m_reflectionMat;
    static std::vector<CEntity*>m_renderableList;
public:
    static RwRaster* m_reflection;
    static RwRaster* m_refraction;
    static VertexBuffer* mVertexBuffer;
    static RwIndexBuffer* mIndexBuffer;
    static void AddObject(CEntity* entity, float distance);
    static void SectorList(CPtrList& ptrList);
    static void ScanSectorList(int sectorX, int sectorY);
    static void RenderReflection(RenderCallback renderCallback);
    static void SetupWaterShader();
    static void Hook();
    static void InitShaders();
    static void UpdateTextures();
    static void UpdateImgui();
};

