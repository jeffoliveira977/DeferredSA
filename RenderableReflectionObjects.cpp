#include "RenderableReflectionObjects.h"
#include "CWorld.h"
#include "CRenderer.h"
#include "Renderer.h"
#include "AABB.h"
#include "EnvironmentMapping.h"
//std::vector<CEntity*> RenderableReflectionObjects::m_renderableList;
//
//void RenderableReflectionObjects::AddObject(CEntity* entity, float distance)
//{
//    if(distance > 400)
//        return;
//   
//    if(distance < 100)
//        m_renderableList.push_back(entity);
//    else if(entity->m_pLod != NULL)
//        m_renderableList.push_back(entity->m_pLod);
//}
//
//void RenderableReflectionObjects::SectorList(CPtrList& ptrList)
//{
//    int count[6] = {0,0,0,0,0};
//    for(size_t i = 0; i < 6; i++)
//    {
//        for(auto node = ptrList.GetNode(); node; node = node->pNext)
//        {
//            CEntity* entity = reinterpret_cast<CEntity*>(node->pItem);
//            if(entity->m_nScanCode != CWorld::ms_nCurrentScanCode)
//            {
//                entity->m_nScanCode = CWorld::ms_nCurrentScanCode;
//
//                CColModel* col = entity->GetColModel();
//                if(col == nullptr)
//                    continue;
//
//                CVector position = entity->GetPosition();
//                if(entity->m_pLod)
//                    position = entity->m_pLod->GetPosition();
//
//                float distance = (position - CRenderer::ms_vecCameraPosition).Magnitude();
//
//                XMMATRIX world = RwMatrixToXMMATRIX(reinterpret_cast<RwMatrix*>(entity->GetMatrix()));
//
//                CBoundingBox modelAABB = col->m_boundBox;
//
//                XMVECTOR min, max;
//                min = XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&modelAABB.m_vecMin));
//                max = XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&modelAABB.m_vecMax));
//                min = XMVector3Transform(min, world);
//                max = XMVector3Transform(max, world);
//
//                Math::AABB aabb;
//                XMStoreFloat3(&aabb.Min, min);
//                XMStoreFloat3(&aabb.Max, max);
//
//                if(EnvironmentMapping::m_frustum[i].Intersects(aabb))
//                {
//                    AddObject(entity, distance);
//                    count[i]++;
//                }
//            }
//        }
//    } 
//    PrintMessage("%i %i %i %i %i %i", count[0], count[1], count[2], count[3], count[4], count[5]);
//}

void RenderableReflectionObjects::SectorList(CPtrList& ptrList)
{}

void RenderableReflectionObjects::ScanSectorList(int sectorX, int sectorY)
{  
    if(sectorX >= 0 && sectorY >= 0 && sectorX < MAX_SECTORS_X && sectorY < MAX_SECTORS_Y)
    {
        CSector* sector = GetSector(sectorX, sectorY);
        CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);

        SectorList(sector->m_buildings);
        /*SectorList(sector->m_dummies);
         SectorList(repeatSector->m_lists[REPEATSECTOR_VEHICLES]);
         SectorList(repeatSector->m_lists[REPEATSECTOR_PEDS]);*/
        SectorList(repeatSector->m_lists[REPEATSECTOR_OBJECTS]);
    }
}

void RenderableReflectionObjects::Update()
{
    SetNextScanCode();

    int x = GetSectorX(CRenderer::ms_vecCameraPosition.x);
    int y = GetSectorY(CRenderer::ms_vecCameraPosition.y);

    int sectorCount = 10;
    for(int j = -sectorCount; j < sectorCount; j++)
        for(int i = -sectorCount; i < sectorCount; i++)
            ScanSectorList(x + i, y + j);
}

//std::vector<CEntity*> RenderableReflectionObjects::GetRenderList()
//{
//    return m_renderableList;
//}
