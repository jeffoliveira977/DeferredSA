#include "Renderer.h"
#include "CommonD.h"
#include "GTADef.h"
#include "CGame.h"
#include "CRenderer.h"
#include "CStreaming.h"
#include "CWorld.h"
#include "CCamera.h"
#include "CTimeCycle.h"
#include "CMirrors.h"
#include "CVisibilityPlugins.h"
#include "CModelInfo.h"
#include "CGeneral.h"
#include "EnvironmentMapping.h"
#include "Water.h"
#include "CGameIdle.h"
#include "ShadowCaster.h"
#include "DeferredRenderer.h"
std::vector<CEntity*> Renderer::ms_aVisibleReflectionObjects{};

void Renderer::RenderReflectionEntity(CEntity* entity)
{
    if(entity->m_pRwObject == nullptr)
        return;
    
    entity->m_bImBeingRendered = true;

    if(entity->m_pRwObject->type == rpATOMIC)
        RpAtomicRender(entity->m_pRwAtomic);
    else
        RpClumpRender(entity->m_pRwClump);

    entity->m_bImBeingRendered = false;
}

void Renderer::RenderCubemapEntities()
{
    for(auto entity : ms_aVisibleReflectionObjects)
    {
        if(entity->m_pRwObject)
        {
            entity->m_bImBeingRendered = true;

            if(entity->m_pRwObject->type == rpATOMIC)
                RpAtomicRender(entity->m_pRwAtomic);
            else
                RpClumpRender(entity->m_pRwClump);

            entity->m_bImBeingRendered = false;
        }
    }
}

void Renderer::AddEntityToReflectionList(CEntity* entity, float renderDistance)
{
    if(ms_aVisibleReflectionObjects.size() > 500 || entity == NULL)
        return;

    if(renderDistance < 100)
        ms_aVisibleReflectionObjects.push_back(entity);
    else if(entity->m_pLod != NULL)
        ms_aVisibleReflectionObjects.push_back(entity->m_pLod);
}

void Renderer::ScanPtrListForReflections(CPtrList* ptrList, bool loadIfRequired)
{
    // If entity list is empty, than return. 
    if(ptrList == nullptr)
        return;

    for(auto pNode = ptrList->GetNode(); pNode; pNode = pNode->pNext)
    {
        CEntity* entity = reinterpret_cast<CEntity*>(pNode->pItem);
        if(entity->m_nScanCode != CWorld::ms_nCurrentScanCode)
        {
            entity->m_nScanCode = CWorld::ms_nCurrentScanCode;
            CVector entityPos = entity->m_pLod != NULL ? entity->m_pLod->GetPosition() : entity->GetPosition();
            CVector playerPos = FindPlayerCoors(0);
            auto xDist = entityPos.x - playerPos.x;
            auto yDist = entityPos.y - playerPos.y;
            auto zDist = entityPos.z - playerPos.z;
            float renderDistance = sqrt(xDist * xDist + yDist * yDist + zDist * zDist);

            if(ms_aVisibleReflectionObjects.size() < 500)
            {
                if(renderDistance < 100)
                    ms_aVisibleReflectionObjects.push_back(entity);
                else if(entity->m_pLod != NULL)
                    ms_aVisibleReflectionObjects.push_back(entity->m_pLod);
            }
        }
    }
}

void Renderer::ScanSectorListForReflections(int sectorX, int sectorY)
{
    if(sectorX >= 0 && sectorY >= 0 && sectorX < MAX_SECTORS_X && sectorY < MAX_SECTORS_Y)
    {
        CSector* sector = GetSector(sectorX, sectorY);
        CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);
        ScanPtrListForReflections(&sector->m_buildings, false);
        ScanPtrListForReflections(&repeatSector->m_lists[REPEATSECTOR_OBJECTS], false);
    }
}

#include "CascadedShadowRendering.h"

void Renderer::PreRender()
{
    for(std::int32_t i = 0; i < CRenderer::ms_nNoOfVisibleLods; ++i)
    {
        CRenderer::ms_aVisibleLodPtrs[i]->PreRender();
    }
    for(std::int32_t i = 0; i < CRenderer::ms_nNoOfVisibleEntities; ++i)
    {
        CRenderer::ms_aVisibleEntityPtrs[i]->PreRender();
    }
    for(std::int32_t i = 0; i < CRenderer::ms_nNoOfVisibleSuperLods; ++i)
    {
        CRenderer::ms_aVisibleSuperLodPtrs[i]->PreRender();
    }
    for(std::int32_t i = 0; i < CRenderer::ms_nNoOfInVisibleEntities; ++i)
    {
        CRenderer::ms_aInVisibleEntityPtrs[i]->PreRender();
    }
    for(auto link = CVisibilityPlugins::m_alphaEntityList.usedListHead.next;
        link != &CVisibilityPlugins::m_alphaEntityList.usedListTail;
        link = link->next)
    {
       
        if(link->data.m_pCallback == CVisibilityPlugins__RenderEntityADDR)
        {
            link->data.m_entity->m_bOffscreen = false;
            link->data.m_entity->PreRender();
        }
    }
    for(auto link = CVisibilityPlugins::m_alphaUnderwaterEntityList.usedListHead.next;
        link != &CVisibilityPlugins::m_alphaUnderwaterEntityList.usedListTail;
        link = link->next)
    {
        if(link->data.m_pCallback == CVisibilityPlugins__RenderEntityADDR)
        {
            link->data.m_entity->m_bOffscreen = false;
            link->data.m_entity->PreRender();
        }
    }
}

void Renderer::RenderOneRoad(CEntity* entity)
{
    if(CPostEffects__IsVisionFXActive())
    {
        CPostEffects__FilterFX_StoreAndSetDayNightBalance();
        entity->Render();
        CPostEffects__FilterFX_RestoreDayNightBalance();
        return;
    }
    entity->Render();
}

void Renderer::RenderRoads()
{
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK);

    for(std::int32_t i = 0; i < CRenderer::ms_nNoOfVisibleEntities; ++i)
    {
        CEntity* entity = CRenderer::ms_aVisibleEntityPtrs[i];
        if(entity->m_nType == ENTITY_TYPE_BUILDING && CModelInfo::ms_modelInfoPtrs[entity->m_nModelIndex]->IsRoad())
        {
            if(CPostEffects__IsVisionFXActive())
            {
                CPostEffects__FilterFX_StoreAndSetDayNightBalance();
                entity->Render();
                CPostEffects__FilterFX_RestoreDayNightBalance();
            }
            else
            {
                entity->Render();
            }
        }
    }
}

#include "CScene.h"

#define InsertSorted(alpha, info) ((CLink<CVisibilityPlugins::AlphaObjectInfo>* ( __thiscall*)(CLinkList<CVisibilityPlugins::AlphaObjectInfo>*, CVisibilityPlugins::AlphaObjectInfo*))0x733910)(alpha, info)

bool InsertEntityIntoEntityList(CEntity* entity, float distance, void* callback)
{
    CVisibilityPlugins::AlphaObjectInfo info{};
    info.m_entity = entity;
    info.m_pCallback = callback;
    info.m_distance = distance;
    return InsertSorted(&CVisibilityPlugins::m_alphaEntityList, &info);
}

bool InsertEntityIntoUnderwaterEntities(CEntity* entity, float distance)
{
    CVisibilityPlugins::AlphaObjectInfo entityAlphaObjectInfo{};
    entityAlphaObjectInfo.m_distance = distance;
    entityAlphaObjectInfo.m_entity = entity;
    entityAlphaObjectInfo.m_pCallback = CVisibilityPlugins__RenderEntityADDR;
    return InsertSorted(&CVisibilityPlugins::m_alphaUnderwaterEntityList, &entityAlphaObjectInfo);
}

void Renderer::RenderEverythingBarRoads()
{
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
    RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK);
    if(!CGame::currArea)
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)140u);

    for(std::int32_t i = 0; i < CRenderer::ms_nNoOfVisibleEntities; i++)
    {
        CEntity* entity = CRenderer::ms_aVisibleEntityPtrs[i];
        CVehicle* pVehicle = static_cast<CVehicle*>(entity);
        CPed* pPed = static_cast<CPed*>(entity);
        if(entity->m_nType == ENTITY_TYPE_BUILDING && CModelInfo::ms_modelInfoPtrs[entity->m_nModelIndex]->IsRoad())
            continue;

        bool bInserted = false;
        if(entity->m_nType == ENTITY_TYPE_VEHICLE || (entity->m_nType == ENTITY_TYPE_PED && CVisibilityPlugins::GetClumpAlpha(entity->m_pRwClump) != 255))
        {
            if(entity->m_nType == ENTITY_TYPE_VEHICLE)
            {
                bool bInsertIntoSortedList = false;
                if(pVehicle->m_nVehicleClass == VEHICLE_BOAT)
                {
                    eCamMode camMode = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode;
                    if(camMode == MODE_WHEELCAM || camMode == MODE_1STPERSON &&
                       TheCamera.GetLookDirection() != 3 && TheCamera.GetLookDirection() ||
                       CVisibilityPlugins::GetClumpAlpha(entity->m_pRwClump) != 255)
                    {
                        bInsertIntoSortedList = true;
                    }
                }
                else if(!pVehicle->m_nPhysicalFlags.bTouchingWater)
                {
                    bInsertIntoSortedList = true;
                }
                const float fMagnitude = (CRenderer::ms_vecCameraPosition - entity->GetPosition()).Magnitude();
                if(bInsertIntoSortedList)
                    bInserted = CVisibilityPlugins::InsertEntityIntoSortedList(entity, fMagnitude);
                else
                    bInserted = InsertEntityIntoUnderwaterEntities(entity, fMagnitude);
            }
        }
        if(!bInserted)
            RenderOneNonRoad(entity);
    }

    float oldzShift = Scene.m_pRwCamera->zShift;
    RwCameraEndUpdate(Scene.m_pRwCamera);
    Scene.m_pRwCamera->zShift = Scene.m_pRwCamera->zShift - 100.0f;
    RwCameraBeginUpdate(Scene.m_pRwCamera);
    for(std::int32_t i = 0; i < CRenderer::ms_nNoOfVisibleLods; ++i)
    {
        RenderOneNonRoad(CRenderer::ms_aVisibleLodPtrs[i]);
    }
    RwCameraEndUpdate(Scene.m_pRwCamera);
    Scene.m_pRwCamera->zShift = oldzShift;
    RwCameraBeginUpdate(Scene.m_pRwCamera);
}

void Renderer::RenderOneNonRoad(CEntity* entity)
{
    CPed* pPed = static_cast<CPed*>(entity);
    CVehicle* pVehicle = static_cast<CVehicle*>(entity);
    if(entity->m_nType != ENTITY_TYPE_PED || pPed->m_nPedState != PEDSTATE_DRIVING)
    {
        bool bSetupLighting = entity->SetupLighting();
        if(entity->m_nType == ENTITY_TYPE_VEHICLE)
        {
            CVisibilityPlugins::SetupVehicleVariables(entity->m_pRwClump);
            CVisibilityPlugins::InitAlphaAtomicList();
            pVehicle->RenderDriverAndPassengers();
            pVehicle->SetupRender();
        }
        else if(!entity->m_bBackfaceCulled)
        {
            RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
        }
        if(CPostEffects__IsVisionFXActive())
        {
            if(CPostEffects__m_bNightVision)
                CPostEffects__NightVisionSetLights();
            if(CPostEffects__m_bInfraredVision)
                CPostEffects__InfraredVisionSetLightsForDefaultObjects();
            CPostEffects__FilterFX_StoreAndSetDayNightBalance();
            entity->Render();
            CPostEffects__FilterFX_RestoreDayNightBalance();
        }
        else
        {
            entity->Render();
        }
        if(entity->m_nType == ENTITY_TYPE_VEHICLE)
        {
            pVehicle->m_bImBeingRendered = true;
            CVisibilityPlugins::RenderAlphaAtomics();
            pVehicle->m_bImBeingRendered = false;
            pVehicle->ResetAfterRender();
            pVehicle->RemoveLighting(bSetupLighting);
        }
        else
        {
            if(!entity->m_bBackfaceCulled)
                RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK);
            entity->RemoveLighting(bSetupLighting);
        }
    }
}

void Renderer::AddEntityToRenderList(CEntity* pEntity, float fDistance)
{
    CBaseModelInfo* pBaseModelInfo = CModelInfo::ms_modelInfoPtrs[pEntity->m_nModelIndex];
    pBaseModelInfo->SetHasBeenPreRendered(false);
    if(!pEntity->m_bDistanceFade)
    {
        if(pEntity->m_bDrawLast && CVisibilityPlugins::InsertEntityIntoSortedList(pEntity, fDistance))
        {
            pEntity->m_bDistanceFade = false;
            return;
        }
    }
    else if(CVisibilityPlugins::InsertEntityIntoSortedList(pEntity, fDistance))
    {
        return;
    }
    if(pEntity->m_nNumLodChildren && !pEntity->m_bUnderwater)
    {
        CRenderer::ms_aVisibleLodPtrs[CRenderer::ms_nNoOfVisibleLods] = pEntity;
        CRenderer::ms_nNoOfVisibleLods++;
    }
    else
    {
        CRenderer::ms_aVisibleEntityPtrs[CRenderer::ms_nNoOfVisibleEntities] = pEntity;
        CRenderer::ms_nNoOfVisibleEntities++;
    }

   // ShadowCasterEntity->AddEntityToCullList(pEntity);
}

void Renderer::AddToLodRenderList(CEntity* entity, float distance)
{
    CRenderer::ms_pLodRenderList->pEntity = entity;
    CRenderer::ms_pLodRenderList->distance = distance;
    ++CRenderer::ms_pLodRenderList;

    //ShadowCasterEntity->AddEntityToCullList(entity);
}

bool Renderer::InsertEntityIntoSortedList(CEntity* entity, float distance)
{
    if(entity->m_nModelIndex == *(unsigned short*)0x8CD6F4 && CVisibilityPlugins::InsertEntityIntoReallyDrawLastList(entity, distance))
        return true;
    if(entity->m_bUnderwater)
        return  InsertEntityIntoUnderwaterEntities(entity, distance);

    //ShadowCasterEntity->AddEntityToCullList(entity);
    return InsertEntityIntoEntityList(entity, distance, CVisibilityPlugins__RenderEntityADDR);
}

#include <complex>  
void Renderer::ScanSectorList(int sectorX, int sectorY)
{
    bool bRequestModel = false;
    float fDistanceX = GetSectorPosX(sectorX) - CRenderer::ms_vecCameraPosition.x;
    float fDistanceY = GetSectorPosY(sectorY) - CRenderer::ms_vecCameraPosition.y;
    float fAngleInRadians = atan2(-fDistanceX, fDistanceY) - CRenderer::ms_fCameraHeading;
    if(SquaredMagnitude(fDistanceX, fDistanceY) < MAX_STREAMING_RADIUS ||
       fabs(CGeneral::LimitRadianAngle(fAngleInRadians)) < DegreesToRadians(STREAMING_ANGLE_THRESHOLD))
    {
        bRequestModel = true;
    }
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
                    entity->m_bOffscreen = false;
                    bool bInvisibleEntity = false;
                    float fDistance = 0.0f;
                    int visibility = CRenderer::SetupEntityVisibility(entity, fDistance);
                    //if(visibility != RENDERER_STREAMME && !entity->IsEntityOccluded())
                    //     ShadowCasterEntity->AddEntityToCullList(entity);

                    switch(visibility)
                    {
                        case RENDERER_INVISIBLE:
                        {
                            ShadowCasterEntity->AddEntityToCullList(entity);

                            if(entity->m_nType == ENTITY_TYPE_OBJECT)
                            {
                                CBaseModelInfo* pBaseModelInfo = CModelInfo::ms_modelInfoPtrs[entity->m_nModelIndex]->AsAtomicModelInfoPtr();
                                if(pBaseModelInfo)
                                {
                                    if(pBaseModelInfo->nSpecialType == 4 || pBaseModelInfo->nSpecialType == 5)
                                    {
                                        bInvisibleEntity = true;
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                        case RENDERER_VISIBLE:
                        {
                            CRenderer::AddEntityToRenderList(entity, fDistance);
                            break;
                        }
                        case RENDERER_CULLED:
                        {
                            bInvisibleEntity = true;
                            break;
                        }
                        case RENDERER_STREAMME:
                        {
                            if(CStreaming::ms_disableStreaming || !entity->GetIsOnScreen() || CRenderer::ms_bInTheSky)
                                break;

                            if(bRequestModel)
                            {
                                auto pStreamingInfo = &CStreaming::ms_aInfoForModel[entity->m_nModelIndex];
                                if(pStreamingInfo->m_nLoadState == LOADSTATE_LOADED)
                                {
                                    CStreaming::RequestModel(entity->m_nModelIndex, 0);
                                    break;
                                }
                                else
                                {
                                    if(!entity->IsEntityOccluded())
                                    {
                                        CRenderer::m_loadingPriority = 1;
                                        CStreaming::RequestModel(entity->m_nModelIndex, 0);
                                        break;
                                    }
                                }
                            }
                            if(!CRenderer::m_loadingPriority || CStreaming::ms_numModelsRequested < 1)
                                CStreaming::RequestModel(entity->m_nModelIndex, 0);
                            break;
                        }
                        default:
                            break;
                    }

                    if(!bInvisibleEntity)
                        continue;

                    entity->m_bOffscreen = true;
                    if(entity->m_bHasPreRenderEffects)
                    {
                        float fDrawDistance = MAX_INVISIBLE_ENTITY_DISTANCE;
                        CVector2D distance = CRenderer::ms_vecCameraPosition - entity->GetPosition();
                        if(entity->m_nType == ENTITY_TYPE_VEHICLE)
                        {
                            CVehicle* pVehicle = static_cast<CVehicle*>(entity);
                            if(pVehicle->m_nVehicleFlags.bAlwaysSkidMarks)
                                fDrawDistance = MAX_INVISIBLE_VEHICLE_DISTANCE;
                        }
                        if(distance.x > -fDrawDistance && distance.x < fDrawDistance &&
                           distance.y > -fDrawDistance && distance.y < fDrawDistance)
                        {
                            if(CRenderer::ms_nNoOfInVisibleEntities < MAX_INVISIBLE_ENTITY_PTRS - 1)
                            {
                                CRenderer::ms_aInVisibleEntityPtrs[CRenderer::ms_nNoOfInVisibleEntities] = entity;
                                CRenderer::ms_nNoOfInVisibleEntities++;
                            }
                        }
                    }
                }
            }
        }
    }
}

void Renderer::ScanBigBuildingList(int sectorX, int sectorY)
{
    if(sectorX >= 0 && sectorY >= 0 && sectorX < MAX_LOD_PTR_LISTS_X && sectorY < MAX_LOD_PTR_LISTS_Y)
    {
        auto list = GetLodPtrList(sectorX, sectorY);
        bool bRequestModel = false;
        float fDistanceX = GetLodSectorPosX(sectorX) - CRenderer::ms_vecCameraPosition.x;
        float fDistanceY = GetLodSectorPosY(sectorY) - CRenderer::ms_vecCameraPosition.y;
        float fAngleInRadians = atan2(-fDistanceX, fDistanceY) - CRenderer::ms_fCameraHeading;
        if(SquaredMagnitude(fDistanceX, fDistanceY) < MAX_BIGBUILDING_STREAMING_RADIUS ||
           fabs(CGeneral::LimitRadianAngle(fAngleInRadians)) <= DegreesToRadians(BIGBUILDING_STREAMING_ANGLE_THRESHOLD))
        {
            bRequestModel = true;
        }

        for(auto pNode = list->GetNode(); pNode; pNode = pNode->pNext)
        {
            CEntity* entity = reinterpret_cast<CEntity*>(pNode->pItem);
            if(entity->m_nScanCode != CWorld::ms_nCurrentScanCode)
            {
                entity->m_nScanCode = CWorld::ms_nCurrentScanCode;
                float fDistance = 0.0f;
                std::int32_t visibility = CRenderer::SetupBigBuildingVisibility(entity, fDistance);
                if(visibility == RENDERER_VISIBLE)
                {
                    CRenderer::AddEntityToRenderList(entity, fDistance + 0.01f);
                    entity->m_bOffscreen = false;
                }
                else if(visibility == RENDERER_STREAMME && !CStreaming::ms_disableStreaming && bRequestModel)
                {
                    CStreaming::RequestModel(entity->m_nModelIndex, 0);
                }
            }
        }
    }
}

void Renderer::ConstructRenderList()
{
    if(CGame::currArea == 0 && CGameIdle::m_fShadowDNBalance <= 1.0)
    {
        CascadedShadowManagement->CalculateShadowDistances(Scene.m_pRwCamera->nearPlane, Scene.m_pRwCamera->farPlane);
        const auto sunDirs = reinterpret_cast<RwV3d*>(0xB7CA50);
        const auto curr_sun_dir = *reinterpret_cast<int*>(0xB79FD0);
        const auto curr_sun_dirvec = &sunDirs[curr_sun_dir];
        CascadedShadowManagement->DirectionalLightTransform(Scene.m_pRwCamera, sunDirs[curr_sun_dir], 0);
    }

    eZoneAttributes zoneAttributes = CCullZones__FindTunnelAttributesForCoors(TheCamera.GetPosition());
    CRenderer::ms_bRenderTunnels = (zoneAttributes & (eZoneAttributes::UNKNOWN_2 | eZoneAttributes::UNKNOWN_1)) != 0;
    if((zoneAttributes & eZoneAttributes::UNKNOWN_1) || !(zoneAttributes & eZoneAttributes::UNKNOWN_2))
        CRenderer::ms_bRenderOutsideTunnels = true;
    else
        CRenderer::ms_bRenderOutsideTunnels = false;

    CRenderer::ms_lowLodDistScale = 1.0f;
    CRenderer::ms_bInTheSky = false;
    CPlayerPed* player = FindPlayerPed(-1);

    if(player && player->m_nAreaCode == 0)
    {
        float fGroundHeightZ = TheCamera.CalculateGroundHeight(0);
        if(player->GetPosition().z - fGroundHeightZ > 50.0f)
        {
            float fGroundHeightZ = TheCamera.CalculateGroundHeight(2);
            if(player->GetPosition().z - fGroundHeightZ > 10.0f && FindPlayerVehicle(-1, false))
                CRenderer::ms_bInTheSky = true;
        }
        const float fCameraZ = TheCamera.GetPosition().z;
        if(fCameraZ > LOWLOD_CAMERA_HEIGHT_THRESHOLD)
        {
            float fScale = (fCameraZ - LOWLOD_CAMERA_HEIGHT_THRESHOLD) / (250.0f - LOWLOD_CAMERA_HEIGHT_THRESHOLD);
            if(fScale > 1.0f)
                fScale = 1.0f;
            CRenderer::ms_lowLodDistScale = fScale * (2.2f - 1.0f) + 1.0f;
        }
    }

    CRenderer::ms_lowLodDistScale *= CTimeCycle::m_CurrentColours.m_fLodDistMult;
    COcclusion__ProcessBeforeRendering();
    CRenderer::ms_nNoOfVisibleEntities = 0;
    CRenderer::ms_nNoOfVisibleLods = 0;
    CRenderer::ms_nNoOfInVisibleEntities = 0;
    CRenderer::ms_vecCameraPosition = TheCamera.GetPosition();
    CRenderer::ms_fCameraHeading = TheCamera.GetHeading();
    CRenderer::ms_fFarClipPlane = TheCamera.m_pRwCamera->farPlane;
    CRenderer::ResetLodRenderLists();
    ScanWorld();

    CRenderer::ProcessLodRenderLists();
    CStreaming::StartRenderEntities();
}

#include <thread>
void RenderShadowCascade2()
{
    ShadowCasterEntity->Render(0);
}
std::map<CEntity*, bool> ObjectInFrustum[4];
void Renderer::ScanWorld()
{
    for(size_t i = 0; i < 4; i++)
        ObjectInFrustum[i].clear();

    ms_aVisibleReflectionObjects.clear();

  //  ShadowCasterEntity->ClearCullList();

    auto camera = TheCamera.m_pRwCamera;

    CVector frustumPoints[13];
    float farPlane = camera->farPlane;
    const float width = camera->viewWindow.x;
    const float height = camera->viewWindow.y;
    frustumPoints[0] = CVector(0.0f, 0.0f, 0.0f);
    frustumPoints[1].x = frustumPoints[4].x = -(farPlane * width);
    frustumPoints[1].y = frustumPoints[2].y = farPlane * height;
    frustumPoints[2].x = frustumPoints[3].x = farPlane * width;
    frustumPoints[3].y = frustumPoints[4].y = -(farPlane * height);
    frustumPoints[1].z = frustumPoints[2].z = frustumPoints[3].z = frustumPoints[4].z = farPlane;

    for(std::int32_t i = 5; i < 13; i++)
    {
        frustumPoints[i] = CVector(0.0f, 0.0f, 0.0f);
    }

    CRenderer::m_pFirstPersonVehicle = nullptr;
    CVisibilityPlugins::InitAlphaEntityList();

    SetNextScanCode();
    int sectorCount = 10;
    int currentSectorX = (int)ceil((CRenderer::ms_vecCameraPosition.x - 25.0f) / 50.0f + 60.0f);
    int currentSectorY = (int)ceil((CRenderer::ms_vecCameraPosition.y - 25.0f) / 50.0f + 60.0f);
    for(int x = -sectorCount; x < sectorCount + 1; x++)
    {
        for(int y = -sectorCount; y < sectorCount + 1; y++)
        {
            ScanSectorListForReflections(currentSectorX + x, currentSectorY + y);
        }
    }

    CRenderer::m_loadingPriority = 0;
    SetNextScanCode();

    frustumPoints[5] = (frustumPoints[1] * MAX_LOD_DISTANCE) / farPlane;
    frustumPoints[7].x = frustumPoints[5].x / 5;
    frustumPoints[7].y = frustumPoints[5].y / 5;
    frustumPoints[7].z = frustumPoints[5].z;

    frustumPoints[6] = (frustumPoints[2] * MAX_LOD_DISTANCE) / farPlane;
    frustumPoints[8].x = frustumPoints[6].x / 5;
    frustumPoints[8].y = frustumPoints[6].y / 5;
    frustumPoints[8].z = frustumPoints[6].z;

    frustumPoints[9] = (frustumPoints[3] * MAX_LOD_DISTANCE) / farPlane;
    frustumPoints[11].x = frustumPoints[9].x / 5;
    frustumPoints[11].y = frustumPoints[9].y / 5;
    frustumPoints[11].z = frustumPoints[9].z;

    frustumPoints[10] = (frustumPoints[4] * MAX_LOD_DISTANCE) / farPlane;
    frustumPoints[12].x = frustumPoints[10].x / 5;
    frustumPoints[12].y = frustumPoints[10].y / 5;
    frustumPoints[12].z = frustumPoints[10].z;
    RwV3dTransformPoints((RwV3d*)frustumPoints, (RwV3d*)frustumPoints, 13, RwFrameGetMatrix(RwCameraGetFrame(camera)));

    CVector2D points[5];
    points[0].x = GetSectorfX(frustumPoints[0].x);
    points[0].y = GetSectorfY(frustumPoints[0].y);
    points[1].x = GetSectorfX(frustumPoints[5].x);
    points[1].y = GetSectorfY(frustumPoints[5].y);
    points[2].x = GetSectorfX(frustumPoints[6].x);
    points[2].y = GetSectorfY(frustumPoints[6].y);
    points[3].x = GetSectorfX(frustumPoints[9].x);
    points[3].y = GetSectorfY(frustumPoints[9].y);
    points[4].x = GetSectorfX(frustumPoints[10].x);
    points[4].y = GetSectorfY(frustumPoints[10].y);
    CWorldScan__ScanWorld(points, 5, ScanSectorList);

    points[0].x = GetLodSectorfX(frustumPoints[0].x);
    points[0].y = GetLodSectorfY(frustumPoints[0].y);
    points[1].x = GetLodSectorfX(frustumPoints[1].x);
    points[1].y = GetLodSectorfY(frustumPoints[1].y);
    points[2].x = GetLodSectorfX(frustumPoints[2].x);
    points[2].y = GetLodSectorfY(frustumPoints[2].y);
    points[3].x = GetLodSectorfX(frustumPoints[3].x);
    points[3].y = GetLodSectorfY(frustumPoints[3].y);
    points[4].x = GetLodSectorfX(frustumPoints[4].x);
    points[4].y = GetLodSectorfY(frustumPoints[4].y);

    CWorldScan__ScanWorld(points, 5, ScanBigBuildingList);
    if(CGame::currArea != 0 || (CGameIdle::m_fShadowDNBalance >= 1.0))
        return;

    //SetNextScanCode();
    //CRenderer::m_loadingPriority = 0;
   /* ShadowCasterEntity->Update(GetSectorX(CRenderer::ms_vecCameraPosition.x), GetSectorY(CRenderer::ms_vecCameraPosition.y));*/

    //EnvironmentMapping::SetRenderCallback(RenderCubemapEntities);
    EnvironmentMapping::SetRenderCallback(RenderShadowCascade2);
}
