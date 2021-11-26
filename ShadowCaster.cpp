#include "GTADef.h"
#include "ShadowCaster.h"
#include "CWorld.h"
#include "CRenderer.h"
#include "CModelInfo.h"
#include "CascadedShadowRendering.h"
#include "Renderer.h"
#include "Frustum.h"
#include "AABB.h"
#include "CCamera.h"

ShadowCaster* ShadowCasterEntity;

ShadowCaster::ShadowCaster()
{}

ShadowCaster::~ShadowCaster()
{}

bool CheckForTrees(CEntity* entity)
{
    auto id = entity->m_nModelIndex;
    return ((id > 614 && id < 793) ||
           (id > 857 && id < 896) ||
            (id > 18267 && id < 18274) ||
            id == 3505 ||
            id == 3507 ||
            id == 3510 ||
            id == 3511 ||
            id == 3517 ||
            id == 16060 ||
            id == 16061 );
}

void ShadowCaster::AddShadowEntity(int i, CEntity* entity, float distace)
{
   /* for(size_t j = 0; j < m_castEntity[i].size(); j++)
    {
        if(m_castEntity[i][j] == entity)
            return;
    }*/

    CBaseModelInfo* modelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
    auto modelbb = entity->GetColModel()->m_boundBox;

    auto entityPos = entity->GetPosition();
    auto ext = ((modelbb.m_vecMax+ entityPos) - (modelbb.m_vecMin+ entityPos));
    float renderDist = sqrt(ext.x * ext.x + ext.y * ext.y + ext.z * ext.z);


  /*  if(renderDist > longEdge)
        return;*/

    if(distace > 50 && entity->m_nType == ENTITY_TYPE_PED)
        return;


    static float m = 0.0;

    m = max(m, renderDist);

    if(distace > renderDist && !CheckForTrees(entity) && entity->m_pLod != nullptr && !entity->GetIsOnScreen())
    {
        m_castEntity[i].push_back(entity->m_pLod);
        return;
    }

    if(distace < 200 || CheckForTrees(entity))
        m_castEntity[i].push_back(entity);
    else if(entity->m_pLod != nullptr && distace < 600)
        m_castEntity[i].push_back(entity->m_pLod);

}

bool PlaneIntersectAABB(XMVECTOR* p, XMVECTOR min, XMVECTOR max)
{
    bool intersect = true;

    XMFLOAT3 mins;
    XMStoreFloat3(&mins, min);

    XMFLOAT3 maxs;
    XMStoreFloat3(&maxs, max);
    XMFLOAT3 vmin, vmax;

    for(int i = 0; i < 6; ++i)
    {
        if(XMVectorGetX(p[i]) > 0.0)
        {
            vmin.x = mins.x;
            vmax.x = maxs.x;
        }
        else
        {
            vmin.x = maxs.x;
            vmax.x = mins.x;
        }

        if(XMVectorGetY(p[i]) > 0.0)
        {
            vmin.y = mins.y;
            vmax.y = maxs.y;
        }
        else
        {
            vmin.y = maxs.y;
            vmax.y = mins.y;
        }

        if(XMVectorGetZ(p[i]) > 0.0)
        {
            vmin.z = mins.z;
            vmax.z = maxs.z;
        }
        else
        {
            vmin.z = maxs.z;
            vmax.z = mins.z;
        }

        float  distance = XMVectorGetW(p[i]);
        if(XMVectorGetW(XMPlaneDotNormal(p[i], XMLoadFloat3(&vmin))) - distance < 0.0 &&
           XMVectorGetW(XMPlaneDotNormal(p[i], XMLoadFloat3(&vmax))) - distance < 0.0)
        {
            intersect = false;
        }
    }

    return intersect;
}

void ShadowCaster::AddEntityToCullList(CEntity* entity)
{
    if(entity == nullptr || entity->m_pRwObject == nullptr)
        return;

    CColModel* col = entity->GetColModel();

    if(col == nullptr)
        return;

    CBaseModelInfo* modelinfo = CModelInfo::ms_modelInfoPtrs[entity->m_nModelIndex];

    XMMATRIX world = RwMatrixToXMMATRIX(reinterpret_cast<RwMatrix*>(entity->GetMatrix()));

    CBoundingBox modelAABB = col->m_boundBox;

    XMVECTOR min, max;
    min = XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&modelAABB.m_vecMin));
    max = XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&modelAABB.m_vecMax));

    min = XMVector3Transform(min, world);
    max = XMVector3Transform(max, world);

    Math::AABB aabb;
    XMStoreFloat3(&aabb.Min, min);
    XMStoreFloat3(&aabb.Max, max);

    CVector position = entity->GetPosition();
    if(entity->m_pLod)
        position = entity->m_pLod->GetPosition();

    float renderDistance = (position - CRenderer::ms_vecCameraPosition).Magnitude();

    bool isVehicle = entity->m_nType == ENTITY_TYPE_PED;

    if(renderDistance > 20.0 && col->m_boundSphere.m_fRadius < 2.0f && !isVehicle && !entity->GetIsBoundingBoxOnScreen())
        return;

    if(renderDistance > 70.0 && col->m_boundSphere.m_fRadius < 5.0f && !isVehicle)
        return;

    if(renderDistance > 100.0 && isVehicle)
        return;

    float sphereRadius = col->m_boundSphere.m_fRadius + CRenderer::ms_fFarClipPlane;
    float minDistance = min(TheCamera.m_fLODDistMultiplier * modelinfo->m_fDrawDistance, sphereRadius);

    float fadingDistance = MAX_FADING_DISTANCE;
    float d = min(modelinfo->m_fDrawDistance, minDistance);
    if(d > MAX_LOWLOD_DISTANCE)
        fadingDistance = d / 15.0f + 10.0f;
    if(entity->m_bIsBIGBuilding)
        minDistance *= CRenderer::ms_lowLodDistScale;

    //PrintMessage("%f %f", renderDistance, col->m_boundSphere.m_fRadius);

    bool useLod = false;
    if(fadingDistance + renderDistance - MAX_FADING_DISTANCE > minDistance * 0.5)
        useLod = true;

    if(renderDistance > 70.0 && isVehicle)
        useLod = true;

    for(auto i = 0; i < CascadedShadowManagement->CascadeCount; i++)
    {
        bool isInsideLightBBox;

        isInsideLightBBox = CascadedShadowManagement->Desc[i].m_FrustumCulling.Intersects(aabb);

        auto it = std::find(m_castEntity[i].begin(), m_castEntity[i].end(), entity);
        if(it != m_castEntity[i].end())
            continue;
       //     PrintMessage("found: %i", entity->m_nModelIndex);
        //isInsideLightBBox = Collider::IsIntersectAABBWithFrustum(aabb, CascadedShadowManagement->m_frustum[i]);
        // isInsideLightBBox |= PlaneIntersectAABB(CascadedShadowManagement->m_planes[i], min, max);

        if(isInsideLightBBox)
            ObjectInFrustum[i][entity] = true;
        else
            ObjectInFrustum[i][entity] = false;

        if(isInsideLightBBox)
        {
            if(useLod && entity->m_pLod)
                m_castEntity[i].push_back(entity->m_pLod);
            else
                m_castEntity[i].push_back(entity);
        }
    }
}

void ShadowCaster::ClearCullList()
{
    for(size_t i = 0; i < 4; i++)
        m_castEntity[i].clear();
}

void ShadowCaster::CastShadowSectorList(CPtrList& ptrList)
{
    for(auto node = ptrList.GetNode(); node; node = node->pNext)
    {
        CEntity* entity = reinterpret_cast<CEntity*>(node->pItem);
        if(entity->m_nScanCode != CWorld::ms_nCurrentScanCode)
        {
            entity->m_nScanCode = CWorld::ms_nCurrentScanCode;
            AddEntityToCullList(entity);
        }
    }
}

#include "CGeneral.h"
void ShadowCaster::ScanSectorList(int sectorX, int sectorY)
{
   // CRenderer::SetupScanLists(sectorX, sectorY);
    if(sectorX >= 0 && sectorY >= 0 && sectorX < MAX_SECTORS_X && sectorY < MAX_SECTORS_Y)
    {
        CSector* sector = GetSector(sectorX, sectorY);
        CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);
        
        float sectorPosX = (sectorX - 60) * 50.0f;
        float sectorPosY = (sectorY - 60) * 50.0f;
        float camDistX = sectorPosX - CRenderer::ms_vecCameraPosition.x;
        float camDistY = sectorPosY - CRenderer::ms_vecCameraPosition.y;
        float d = sqrt(camDistY * camDistY + camDistX * camDistX);

        //if(d >= CRenderer::ms_fFarClipPlane / 2)
        //    return;
        //PrintMessage("%f %f", CRenderer::ms_vecCameraPosition.z, distanceToSector);

        CastShadowSectorList(sector->m_buildings);
       /* CastShadowSectorList(sector->m_dummies);
        CastShadowSectorList(repeatSector->m_lists[REPEATSECTOR_VEHICLES]);
        CastShadowSectorList(repeatSector->m_lists[REPEATSECTOR_PEDS]);*/
        CastShadowSectorList(repeatSector->m_lists[REPEATSECTOR_OBJECTS]);
    }
}

void ShadowCaster::Update(int x, int y)
{
    int sectorCount = 4;
    for(int i = -sectorCount; i < sectorCount+1; i++)
        for(int j = -sectorCount; j < sectorCount+1; j++)
            ScanSectorList(x + i, y + j);

}

void ShadowCaster::Render(int i)
{
    for(auto entity : m_castEntity[i])
    {
        if(entity == nullptr || 
           entity->m_pRwObject == nullptr)
            continue;

        entity->m_bImBeingRendered = true;

        // We need to render some sub-entities for peds
        if(entity->m_nType == eEntityType::ENTITY_TYPE_PED)
        {
            CPed* ped = (CPed*)entity;
            auto jetPackTask = ped->m_pIntelligence->GetTaskJetPack();
            if(jetPackTask && jetPackTask->m_pJetPackClump)
                RpClumpRender(jetPackTask->m_pJetPackClump);
            //CVisibilityPluginsRH::RenderWeaponsForPed(ped);
        }

        if(entity->m_pRwObject->type == rpATOMIC)
           RpAtomicRender(entity->m_pRwAtomic);
        else
            RpClumpRender(entity->m_pRwClump);

        entity->m_bImBeingRendered = false;
    }
}
