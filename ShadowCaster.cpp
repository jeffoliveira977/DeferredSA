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

void ShadowCaster::AddObject(CEntity* entity)
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

    //if(renderDistance > 100.0 && isVehicle)
    //    return;

    float sphereRadius = col->m_boundSphere.m_fRadius + CRenderer::ms_fFarClipPlane;
    float minDistance = std::min(TheCamera.m_fLODDistMultiplier * modelinfo->m_fDrawDistance, sphereRadius);

    float fadingDistance = MAX_FADING_DISTANCE;
    float d = std::min(modelinfo->m_fDrawDistance, minDistance);
    if(d > MAX_LOWLOD_DISTANCE)
        fadingDistance = d / 15.0f + 10.0f;
    if(entity->m_bIsBIGBuilding)
        minDistance *= CRenderer::ms_lowLodDistScale;

    //PrintMessage("%f %f", renderDistance, col->m_boundSphere.m_fRadius);

    bool useLod = false;
    if(fadingDistance + renderDistance - MAX_FADING_DISTANCE > minDistance)
        useLod = true;

    if(renderDistance > 70.0 && isVehicle)
        useLod = true;

    for(size_t i = 0; i < CascadedShadowManagement->CascadeCount; i++)
    {
        bool intersects = CascadedShadowManagement->Desc[i].m_FrustumCulling.Intersects(aabb);

        if(intersects)
            ObjectInFrustum[i][entity] = true;
        else
            ObjectInFrustum[i][entity] = false;

        if(intersects)
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
            AddObject(entity);
        }
    }
}

void ShadowCaster::ScanSectorList(int sectorX, int sectorY)
{
    //if(sectorX >= 0 && sectorY >= 0 && sectorX < MAX_SECTORS_X && sectorY < MAX_SECTORS_Y)
    //{
      //  CSector* sector = GetSector(sectorX, sectorY);
    //    CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);

    //    float sectorPosX = (sectorX - 60) * 50.0f;
    //    float sectorPosY = (sectorY - 60) * 50.0f;
    //    float camDistX = sectorPosX - CRenderer::ms_vecCameraPosition.x;
    //    float camDistY = sectorPosY - CRenderer::ms_vecCameraPosition.y;
    //    float d = sqrt(camDistY * camDistY + camDistX * camDistX);

    //    //if(d >= CRenderer::ms_fFarClipPlane / 2)
    //    //    return;
    //    //PrintMessage("%f %f", CRenderer::ms_vecCameraPosition.z, distanceToSector);

    //    CastShadowSectorList(sector->m_buildings);
    //    /* CastShadowSectorList(sector->m_dummies);
    //     CastShadowSectorList(repeatSector->m_lists[REPEATSECTOR_VEHICLES]);
    //     CastShadowSectorList(repeatSector->m_lists[REPEATSECTOR_PEDS]);*/
    //    CastShadowSectorList(repeatSector->m_lists[REPEATSECTOR_OBJECTS]);
    //}

    if(sectorX >= 0 && sectorY >= 0 && sectorX < MAX_SECTORS_X && sectorY < MAX_SECTORS_Y)
    {
        CRenderer::SetupScanLists(sectorX, sectorY);
        CPtrListDoubleLink** pScanLists = reinterpret_cast<CPtrListDoubleLink**>(&PC_Scratch);
        for(std::int32_t scanListIndex = 0; scanListIndex < TOTAL_ENTITY_SCAN_LISTS; scanListIndex++)
        {
            CPtrListDoubleLink* pDoubleLinkList = pScanLists[scanListIndex];
            if(pDoubleLinkList)
            {
                CPtrNodeDoubleLink* pDoubleLinkNode = pDoubleLinkList->GetNode();
                while(pDoubleLinkNode)
                {
                    CEntity* entity = reinterpret_cast<CEntity*>(pDoubleLinkNode->pItem);
                    pDoubleLinkNode = pDoubleLinkNode->pNext;
                    if(entity->m_nScanCode != CWorld::ms_nCurrentScanCode)
                    {
                        entity->m_nScanCode = CWorld::ms_nCurrentScanCode;
                        AddObject(entity);
                    }
                }
            }
        }
    }
}

void ShadowCaster::Update(int x, int y)
{
    for(size_t i = 0; i < 4; i++)
        m_castEntity[i].clear();

    SetNextScanCode();

    x = GetSectorX(CRenderer::ms_vecCameraPosition.x);
    y = GetSectorY(CRenderer::ms_vecCameraPosition.y);

    CVector centre = CRenderer::ms_vecCameraPosition;
    CRect rect(centre.x - 20.0f,
               centre.y - 20.0f,
               centre.x + 20.0f,
               centre.y + 20.0f);
    int xstart = GetSectorX(rect.left);
    int ystart = GetSectorY(rect.top);
    int xend = GetSectorX(rect.right);
    int yend = GetSectorY(rect.bottom);
   // PrintMessage("%i %i", xstart, xend);
    int sectorCount = 10;
    for(int j = -sectorCount; j < sectorCount; j++)
    for(int i = -sectorCount; i < sectorCount; i++)
            ScanSectorList( x+i,y+  j);
        
    
}

#include "CGame.h"
#include "CVisibilityPlugins.h"
#include "CScene.h"
#include "VisibilityPlugins.h"

void ShadowCaster::Render(int i)
{
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
    if(!CGame::currArea)
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)140);

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
  
    for(auto entity : m_castEntity[i])
    {
        if(entity == nullptr ||  entity->m_pRwObject == nullptr)
            continue;

        if(entity->m_nType == ENTITY_TYPE_PED)
        {
            CPed* ped = static_cast<CPed*>(entity);
            CTaskSimpleJetPack* jetPack = ped->m_pIntelligence->GetTaskJetPack();
            if(jetPack && jetPack->m_pJetPackClump)
                RpClumpRender(jetPack->m_pJetPackClump);
        }

        entity->m_bImBeingRendered = true;

        CVehicle* vehicle = static_cast<CVehicle*>(entity);
        if(entity->m_nType == ENTITY_TYPE_VEHICLE)
        {
            CVisibilityPlugins::SetupVehicleVariables(entity->m_pRwClump);
            CVisibilityPlugins::InitAlphaAtomicList();  
            vehicle->SetupRender();
        }

        entity->Render();

        if(entity->m_nType == ENTITY_TYPE_VEHICLE)
        {   
            CVisibilityPlugins::RenderAlphaAtomics();      
            vehicle->ResetAfterRender();
        }

        entity->m_bImBeingRendered = false;
    }
    VisibilityPlugins::RenderWeaponPedsNoMuzzleFlash();
    // CVisibilityPlugins::RenderWeaponPedsForPC();
}
