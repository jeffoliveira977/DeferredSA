#include "MeshCulling.h"
#include "CVisibilityPlugins.h"

bool MeshCulling::InFrustum(CEntity* entity, Math::Frustum frustum)
{
	CColModel* col = entity->GetColModel();

	CVector position = entity->GetPosition();
	if (entity->m_pLod)
		position = entity->m_pLod->GetPosition();

	XMMATRIX world = RwMatrixToXMMATRIX(reinterpret_cast<RwMatrix*>(entity->GetMatrix()));

	CBoundingBox modelAABB = col->m_boundBox;

	XMFLOAT3 min, max;
	min = *reinterpret_cast<XMFLOAT3*>(&modelAABB.m_vecMin);
	max = *reinterpret_cast<XMFLOAT3*>(&modelAABB.m_vecMax);

	Math::AABB aabb(min, max);
	aabb.Transform(world);

	return frustum.Intersects(aabb);
}

void RenderEntity(CEntity* entity)
{
	if (entity->m_nType == ENTITY_TYPE_PED)
	{
		CPed* ped = static_cast<CPed*>(entity);
		CTaskSimpleJetPack* jetPack = ped->m_pIntelligence->GetTaskJetPack();
		if (jetPack && jetPack->m_pJetPackClump)
			RpClumpRender(jetPack->m_pJetPackClump);
	}

	entity->m_bImBeingRendered = true;

	CVehicle* vehicle = static_cast<CVehicle*>(entity);
	if (entity->m_nType == ENTITY_TYPE_VEHICLE)
	{
		CVisibilityPlugins::SetupVehicleVariables(entity->m_pRwClump);
		CVisibilityPlugins::InitAlphaAtomicList();
		vehicle->SetupRender();
	}
	else if (!entity->m_bBackfaceCulled)
	{
		RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
	}
	entity->Render();

	if (entity->m_nType == ENTITY_TYPE_VEHICLE)
	{
		CVisibilityPlugins::RenderAlphaAtomics();
		vehicle->ResetAfterRender();
	}
	else if (!entity->m_bBackfaceCulled)
	{
		RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK);
	}

	entity->m_bImBeingRendered = false;
}

#include"CModelInfo.h"
#include "LightManager.h"

bool CheckLamps(CEntity* entity)
{
	auto id = entity->m_nModelIndex;
	if (id == 1284 ||
		id == 1352 ||
		id == 1351 ||
		id == 1223 ||
		id == 1283 ||
		id == 1231 ||
		id == 1232)
		return true;
	return false;
}

bool MeshCulling::Render(vector<CEntity*> list, Math::Frustum frustum)
{
	for (auto entity : list)
	{
		CColModel* col = entity->GetColModel();
		if (col == nullptr)
			continue;

		bool castEntity = false;
		auto pModelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
		if (pModelInfo->m_nNum2dEffects)
			castEntity = true;

		auto id = entity->m_nModelIndex;
		if (id == 1232)
		PrintMessage("%d %d", castEntity, pModelInfo->m_nNum2dEffects);

		//if(castEntity && CheckLamps(entity))
		//	RenderEntity(entity);
		//else
		if (InFrustum(entity, frustum))
		{
			RenderEntity(entity);
		}
	}

	return true;
}

#include "Renderer.h"
#include "CRenderer.h"

bool MeshCulling::GetMeshRenderList(int8_t scan, std::vector<CEntity*>& meshList)
{
	auto ScanSectorList = [](int sectorX, int sectorY, int8_t scan, std::vector<CEntity*>& meshList) {
		if (sectorX >= 0 && sectorY >= 0 && sectorX < MAX_SECTORS_X && sectorY < MAX_SECTORS_Y)
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

			auto CastShadowSectorList = [](CPtrList& ptrList, std::vector<CEntity*>& meshList)
			{
				for (auto node = ptrList.GetNode(); node; node = node->pNext)
				{
					CEntity* entity = reinterpret_cast<CEntity*>(node->pItem);
					if (entity->m_nScanCode != CWorld::ms_nCurrentScanCode)
					{
						entity->m_nScanCode = CWorld::ms_nCurrentScanCode;
						meshList.push_back(entity);
					}
				}
			};

			if (scan & MeshRenderList::BUILDING_SCAN)
				CastShadowSectorList(sector->m_buildings, meshList);	
			CastShadowSectorList(sector->m_dummies, meshList);			
			if (scan & MeshRenderList::VEHICLE_SCAN)
				CastShadowSectorList(repeatSector->m_lists[REPEATSECTOR_VEHICLES], meshList);
			if (scan & MeshRenderList::PED_SCAN)
				CastShadowSectorList(repeatSector->m_lists[REPEATSECTOR_PEDS], meshList);
			if (scan & MeshRenderList::OBJECT_SCAN)
				CastShadowSectorList(repeatSector->m_lists[REPEATSECTOR_OBJECTS], meshList);
		}
	};

	auto x = GetSectorX(CRenderer::ms_vecCameraPosition.x);
	auto y = GetSectorY(CRenderer::ms_vecCameraPosition.y);

	int sectorCount = 10;
	for (int j = -sectorCount; j < sectorCount; j++)
		for (int i = -sectorCount; i < sectorCount; i++)
			ScanSectorList(x + i, y + j, scan, meshList);
	
	return true;
}
