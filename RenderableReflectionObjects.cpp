#include "RenderableReflectionObjects.h"
#include "CWorld.h"
#include "CRenderer.h"
#include "Renderer.h"

std::vector<CEntity*> RenderableReflectionObjects::m_renderableList;

void RenderableReflectionObjects::AddObject(CEntity* entity, float distance)
{
    if(distance > 200)
        return;
   
    if(distance < 100)
        m_renderableList.push_back(entity);
    else if(entity->m_pLod != NULL)
        m_renderableList.push_back(entity->m_pLod);
}

void RenderableReflectionObjects::SectorList(CPtrList& ptrList)
{
    for(auto node = ptrList.GetNode(); node; node = node->pNext)
    {
        CEntity* entity = reinterpret_cast<CEntity*>(node->pItem);
        if(entity->m_nScanCode != CWorld::ms_nCurrentScanCode)
        {
            entity->m_nScanCode = CWorld::ms_nCurrentScanCode;

            //CVector position = entity->GetPosition();
            //if(entity->m_pLod)
            //    position = entity->m_pLod->GetPosition();

            //float distance = (position - CRenderer::ms_vecCameraPosition).Magnitude();
            CVector entityPos = entity->m_pLod != NULL ? entity->m_pLod->GetPosition() : entity->GetPosition();
            CVector playerPos = FindPlayerCoors(0);
            auto xDist = entityPos.x - playerPos.x;
            auto yDist = entityPos.y - playerPos.y;
            auto zDist = entityPos.z - playerPos.z;
            float renderDistance = sqrt(xDist * xDist + yDist * yDist + zDist * zDist);
            AddObject(entity, renderDistance);
        }
    }
}

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
    m_renderableList.clear();
    SetNextScanCode();

    int x = GetSectorX(CRenderer::ms_vecCameraPosition.x);
    int y = GetSectorY(CRenderer::ms_vecCameraPosition.y);

    int sectorCount = 10;
    for(int j = -sectorCount; j < sectorCount; j++)
        for(int i = -sectorCount; i < sectorCount; i++)
            ScanSectorList(x + i, y + j);
}

std::vector<CEntity*> RenderableReflectionObjects::GetRenderList()
{
    return m_renderableList;
}
