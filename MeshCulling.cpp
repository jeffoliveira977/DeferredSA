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
bool MeshCulling::Render(vector<CEntity*> list, Math::Frustum frustum)
{
	for (auto entity : list)
	{
		bool castEntity = false;
		auto pModelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
		if (pModelInfo->m_nNum2dEffects)
			castEntity = true;

		/*if (castEntity)
		{
			RenderEntity(entity);
			continue; 
		}*/

		 if (InFrustum(entity, frustum))
		{
			RenderEntity(entity);
		}
	}
	return true;
}
