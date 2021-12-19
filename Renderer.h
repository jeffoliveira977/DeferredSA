#pragma once
#include "CEntity.h"
#include <vector>
#include "CPtrList.h"
#include "CommonD.h"
#include "RwMath.h"
#include <DirectXCollision.h>
#include "CWorld.h"
#include <map>
struct sLodListEntry
{
    CEntity* entity;
    float distance;
};
enum class RendererVisibility
{
    INVISIBLE,
    VISIBLE,
    OFFSCREEN,
    NOT_LOADED,
};


enum eAreaCodes
{
    AREA_CODE_NORMAL_WORLD,
    AREA_CODE_1,
    AREA_CODE_2,
    AREA_CODE_3,
    AREA_CODE_4,
    AREA_CODE_5,
    AREA_CODE_6,
    AREA_CODE_7,
    AREA_CODE_8,
    AREA_CODE_9,
    AREA_CODE_10,
    AREA_CODE_11,
    AREA_CODE_12,
    AREA_CODE_13,
    AREA_CODE_14,
    AREA_CODE_15,
    AREA_CODE_16,
    AREA_CODE_17,
    AREA_CODE_18,
};
const std::int32_t TOTAL_ENTITY_SCAN_LISTS = 5;
const float MAX_LOD_DISTANCE = 300.0f;
const float MAX_LOWLOD_DISTANCE = 150.0f;
const float MAX_FADING_DISTANCE = 20.0f;
const float MAX_STREAMING_DISTANCE = 50.0f;
const float MAX_INVISIBLE_ENTITY_DISTANCE = 30.0f;
const float MAX_INVISIBLE_VEHICLE_DISTANCE = 200.0f;
const float MAX_STREAMING_RADIUS = 10000.0f;
const float STREAMING_ANGLE_THRESHOLD = 20.62648f;
const float MAX_BIGBUILDING_STREAMING_RADIUS = 80000.0f;
const float BIGBUILDING_STREAMING_ANGLE_THRESHOLD = 40.107f;
const float LOWLOD_CAMERA_HEIGHT_THRESHOLD = 80.0f;

enum class eGroundHeightType : std::int32_t
{
    ENTITY_BOUNDINGBOX_BOTTOM = 0, // ground height + boundingBoxMin.z of colliding entity
    EXACT_GROUND_HEIGHT = 1, // ignores height of colliding entity at position
    ENTITY_BOUNDINGBOX_TOP = 2 // ground height + boundingBoxMax.z of colliding entity
};

enum eRendererVisiblity
{
    RENDERER_INVISIBLE = 0,
    RENDERER_VISIBLE,
    RENDERER_CULLED,
    RENDERER_STREAMME
};

constexpr std::int32_t MAX_WORLD_UNITS = 6000;

// returns sector index in range 0 to 60 (covers half of the map) 
inline float GetHalfMapSectorX(float x)
{
    return x / (MAX_WORLD_UNITS / MAX_SECTORS_X);
}
inline float GetHalfMapSectorY(float y)
{
    return y / (MAX_WORLD_UNITS / MAX_SECTORS_Y);
}
inline float GetSectorfX(float x)
{
    return GetHalfMapSectorX(x) + (MAX_SECTORS_X / 2);
}
inline float GetSectorfY(float y)
{
    return GetHalfMapSectorY(y) + (MAX_SECTORS_Y / 2);
}

// returns sector index in range 0 to 120 (covers full map) 
inline std::int32_t GetSectorX(float x)
{
    return static_cast<std::int32_t>(std::floor(GetSectorfX(x)));
}
inline std::int32_t GetSectorY(float y)
{
    return static_cast<std::int32_t>(std::floor(GetSectorfY(y)));
}
inline float GetSectorPosX(std::int32_t sector)
{
    const std::int32_t HalfOfTotalSectorsX = MAX_SECTORS_Y / 2;
    const float fTotalMapUnitsX = MAX_WORLD_UNITS / MAX_SECTORS_Y;
    return (sector - HalfOfTotalSectorsX) * fTotalMapUnitsX + (fTotalMapUnitsX / 2);
}
inline float GetSectorPosY(std::int32_t sector)
{
    const std::int32_t HalfOfTotalSectorsY = MAX_SECTORS_Y / 2;
    const float fTotalMapUnitsY = MAX_WORLD_UNITS / MAX_SECTORS_Y;
    return (sector - HalfOfTotalSectorsY) * fTotalMapUnitsY + (fTotalMapUnitsY / 2);
}

// returns sector index in range 0 to 15 (covers half of the map) 
inline float GetHalfMapLodSectorX(float fSector)
{
    return fSector / (MAX_WORLD_UNITS / MAX_LOD_PTR_LISTS_X);
}
inline float GetHalfMapLodSectorY(float fSector)
{
    return fSector / (MAX_WORLD_UNITS / MAX_LOD_PTR_LISTS_Y);
}
inline float GetLodSectorfX(float fSector)
{
    return GetHalfMapLodSectorX(fSector) + (MAX_LOD_PTR_LISTS_X / 2);
}
inline float GetLodSectorfY(float fSector)
{
    return GetHalfMapLodSectorY(fSector) + (MAX_LOD_PTR_LISTS_Y / 2);
}
// returns sector index in range 0 to 30 (covers full map) 
inline std::int32_t GetLodSectorX(float fSector)
{
    return static_cast<std::int32_t>(std::floor(GetLodSectorfX(fSector)));
}
inline std::int32_t GetLodSectorY(float fSector)
{
    return static_cast<std::int32_t>(std::floor(GetLodSectorfY(fSector)));
}
inline float GetLodSectorPosX(std::int32_t sector)
{
    const std::int32_t HalfOfTotalSectorsX = MAX_LOD_PTR_LISTS_X / 2;
    const float fTotalMapUnitsX = MAX_WORLD_UNITS / MAX_LOD_PTR_LISTS_X;
    return (sector - HalfOfTotalSectorsX) * fTotalMapUnitsX + (fTotalMapUnitsX / 2);
}
inline float GetLodSectorPosY(std::int32_t sector)
{
    const std::int32_t HalfOfTotalSectorsY = MAX_LOD_PTR_LISTS_Y / 2;
    const float fTotalMapUnitsY = MAX_WORLD_UNITS / MAX_LOD_PTR_LISTS_Y;
    return (sector - HalfOfTotalSectorsY) * fTotalMapUnitsY + (fTotalMapUnitsY / 2);
}

extern std::map<CEntity*, bool> ObjectInFrustum[4];
class Renderer
{
public:
    static void Hook();
    static bool SetupLightingForEntity(CPhysical* entity);
    static void RenderRoads();
    static void RenderCubemapEntities();
    static void RenderEverythingBarRoads();
    static void ConstructRenderList();
    static void AddEntityToReflectionList(CEntity* entity, float renderDistance);
    static void AddEntityToRenderList(CEntity* pEntity, float fDistance);
    static void AddToLodRenderList(CEntity* entity, float distance);
    static bool InsertEntityIntoSortedList(CEntity* entity, float distance);
    static void RenderOneRoad(CEntity*);
    static void RenderOneNonRoad(CEntity*);
    static void ScanSectorList(int x, int y);
    static void ScanPtrListForReflections(CPtrList* ptrList, bool loadIfRequired);
    static void ScanBigBuildingList(int x, int y);
    static void ScanSectorListForReflections(int x, int y);
    static void PreRender();
    static void ScanWorld();
    static void RenderReflectionEntity(CEntity*);
public:
    static bool TOBJpass;
    static std::vector<CEntity*> ms_aVisibleReflectionObjects;
};

