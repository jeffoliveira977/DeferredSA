#include "PointLightShadow.h"
#include "GTADef.h"
#include "ShaderManager.h"
#include "Renderer.h"
#include "Lights.h"

#include "CScene.h"
#include "DeferredRenderer.h"
#include "CRenderer.h"
#include "CGame.h"
#include "CVisibilityPlugins.h"
#include "LightManager.h"

PointLightShadow* PointShadow;

PointLightShadow::PointLightShadow()
{
	for (size_t i = 0; i < 30; i++)
		mColorRaster[i] = nullptr;

	mDepthRaster = nullptr;
}

PointLightShadow::~PointLightShadow()
{
	for (size_t i = 0; i < 30; i++)
	{
		//RwRasterDestroy(mColorRaster[i]);
	}
	RwRasterDestroy(mDepthRaster);
}
void PointLightShadow::Initialize()
{
	m_nShadowSize = 512;

	for (size_t i = 0; i < 30; i++)
	{
		mColorRaster[i] = RwD3D9RasterCreate(m_nShadowSize, m_nShadowSize, D3DFMT_G32R32F, rwRASTERTYPECAMERATEXTURE);
		rwD3D9CubeRasterCreate(mColorRaster[i], D3DFMT_G32R32F, 1);


		mColorCube[i] = RwD3D9RasterCreate(m_nShadowSize*6, m_nShadowSize, D3DFMT_G32R32F, rwRASTERTYPECAMERATEXTURE);
	}

	//mColorRaster[i] = RwRasterCreate(m_nShadowSize, m_nShadowSize, 32, rwRASTERTYPECAMERATEXTURE);
	mDepthRaster = RwRasterCreate(m_nShadowSize * 6, m_nShadowSize, 32, rwRASTERTYPEZBUFFER);
}

void PointLightShadow::AddObject(int i, CEntity* entity, float )
{
	CColModel* col = entity->GetColModel();
	if (col == nullptr)
		return;

	CVector position = entity->GetPosition();
	if (entity->m_pLod)
		position = entity->m_pLod->GetPosition();

	float distance = (position - CRenderer::ms_vecCameraPosition).Magnitude();
	//XMMATRIX world = RwMatrixToXMMATRIX(reinterpret_cast<RwMatrix*>(entity->GetMatrix()));

	//CBoundingBox modelAABB = col->m_boundBox;

	//XMFLOAT3 min, max;
	//min = *reinterpret_cast<XMFLOAT3*>(&modelAABB.m_vecMin);
	//max = *reinterpret_cast<XMFLOAT3*>(&modelAABB.m_vecMax);

	//Math::AABB aabb(min, max);
	//aabb.Transform(world);
	//
	//for (size_t i = 0; i < gLightManager.GetPointLightCount(); i++)
	//{
		//auto light = gLightManager.GetPointLightAt(i);
		//PrintMessage("%d", 1);
		//for (size_t j = 0; j < 6; j++)
		//{
		//	if (light.GetFrustum(j).Intersects(aabb))
		//	{
				
				//AddObject(i, entity, distance);
				if (distance < 100)
					m_renderableList[0].push_back(entity);
				//else if (entity->m_pLod != NULL)
				//	m_renderableList[i].push_back(entity->m_pLod);
	//		}
	//	}
	//}
	/*if (distance > 200)
		return;

	if (distance < 100)
		m_renderableList[i].push_back(entity);
	else if (entity->m_pLod != NULL)
		m_renderableList[i].push_back(entity->m_pLod);*/
}

void PointLightShadow::SectorList(CPtrList& ptrList)
{
	for (auto node = ptrList.GetNode(); node; node = node->pNext)
	{
		CEntity* entity = reinterpret_cast<CEntity*>(node->pItem);
		if (entity->m_nScanCode != CWorld::ms_nCurrentScanCode)
		{
			entity->m_nScanCode = CWorld::ms_nCurrentScanCode;

			CColModel* col = entity->GetColModel();
			if (col == nullptr)
				continue;

			CVector position = entity->GetPosition();
			if (entity->m_pLod)
				position = entity->m_pLod->GetPosition();

			float distance = (position - CRenderer::ms_vecCameraPosition).Magnitude();
			XMMATRIX world = RwMatrixToXMMATRIX(reinterpret_cast<RwMatrix*>(entity->GetMatrix()));

			CBoundingBox modelAABB = col->m_boundBox;

			XMFLOAT3 min, max;
			min = *reinterpret_cast<XMFLOAT3*>(&modelAABB.m_vecMin);
			max = *reinterpret_cast<XMFLOAT3*>(&modelAABB.m_vecMax);

			Math::AABB aabb(min, max);
			aabb.Transform(world);

			for (size_t i = 0; i < gLightManager.GetPointLightCount(); i++)
			{
				auto light = gLightManager.GetPointLightAt(i);

				for (size_t j = 0; j < 6; j++)
				{
					if (light->GetFrustum(j).Intersects(aabb))
					{
						AddObject(i, entity, distance);
					}
				}
			}
		}
	}
}

void PointLightShadow::ScanSectorList(int sectorX, int sectorY)
{
	if (sectorX >= 0 && sectorY >= 0 && sectorX < MAX_SECTORS_X && sectorY < MAX_SECTORS_Y)
	{
		CSector* sector = GetSector(sectorX, sectorY);
		CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);

		// SectorList(sector->m_buildings);
		//SectorList(sector->m_dummies);
		SectorList(repeatSector->m_lists[REPEATSECTOR_VEHICLES]);
			SectorList(repeatSector->m_lists[REPEATSECTOR_PEDS]);
		//SectorList(repeatSector->m_lists[REPEATSECTOR_OBJECTS]);
	}
}
#include "CPointLights.h"
#include "CCamera.h"
#include "CTimeCycle.h"
void PointLightShadow::Update()
{

	//CVector pos = FindPlayerCoors(0);
	//XMMATRIX translation = XMMatrixTranslation(pos.x, pos.y, pos.z);

	//// Scan entity list
	//SetNextScanCode();

	//int x = GetSectorX(CRenderer::ms_vecCameraPosition.x);
	//int y = GetSectorY(CRenderer::ms_vecCameraPosition.y);

	//int sectorCount = 10;
	//for (int j = -sectorCount; j < sectorCount; j++)
	//{
	//	for (int i = -sectorCount; i < sectorCount; i++)
	//	{
	//		ScanSectorList(x + i, y + j);
	//	}
	//}
	
	uint32_t maxLights = min((size_t)29, gLightManager.GetPointLightCount());

	RwRGBA ambient = { CTimeCycle::m_CurrentColours.m_nSkyTopRed, CTimeCycle::m_CurrentColours.m_nSkyTopGreen, CTimeCycle::m_CurrentColours.m_nSkyTopBlue, 255 };
	 RwD3D9SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED| D3DCOLORWRITEENABLE_GREEN);
	RWSRCGLOBAL(curCamera) = Scene.m_pRwCamera;
	for (size_t i = 0; i < maxLights; i++)
	{
		PointLight* data = gLightManager.GetPointLightAt(i);

		CVector camPos = TheCamera.GetPosition();
		float visibleRadius = 20.0;
		CVector dx = CVector(data->GetPosition().x, data->GetPosition().y, data->GetPosition().z) - camPos;
		float intensity = 1.0;

		if (dx.Magnitude() > visibleRadius)
			continue;


		gRenderState = stageCascadeShadow;
		auto radius = data->GetRadius();
		_rwD3D9SetPixelShaderConstant(1, &data->GetPosition(), 1);
		_rwD3D9SetPixelShaderConstant(2, &radius, 1);
		gLightManager.mPointLightList[i].mShadowRaster = mColorCube[i];
		gLightManager.mPointLightList[i].mCastShadow = true;


		for (size_t j = 0; j < 6; j++)
		{
			auto m_viewMatrix = data->GetViewMatrix(j);
			auto m_projectionMatrix = data->GetProjection();

			//_rwD3D9CubeRasterSelectFace(mColorRaster[i], j);


			D3DVIEWPORT9 viewport;
			viewport.X = m_nShadowSize * j;
			viewport.Y = 0;
			viewport.Width = m_nShadowSize;
			viewport.Height = m_nShadowSize;
			viewport.MinZ = 0;
			viewport.MaxZ = 1;
			RwD3DDevice->SetViewport(&viewport);
			RwD3D9SetRenderTarget(0, mColorCube[i]);
			rwD3D9SetDepthStencil(mDepthRaster);
			rwD3D9Clear(nullptr, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);

			if (rwD3D9TestState())
			{

				RwD3D9SetTransform(D3DTS_VIEW, &m_viewMatrix);
				RwD3D9SetTransform(D3DTS_PROJECTION, &m_projectionMatrix);

				_rwD3D9SetVertexShaderConstant(4, &m_viewMatrix, 4);
				_rwD3D9SetVertexShaderConstant(8, &m_projectionMatrix, 4);

				RenderEntities(data, i, j);

			}
		}
	}
	RWSRCGLOBAL(curCamera) = NULL;

	// Restore all channels colors
	RwD3D9SetRenderState(D3DRS_COLORWRITEENABLE,
		D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED |
		D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
}

void PointLightShadow::RenderEntities(PointLight* light, int i, int j)
{
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLFRONT);
	if (!CGame::currArea)
		RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)140);

	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);

	int culled = 0;
	for (auto entity : m_renderableList[0])
	{
		if (entity == nullptr || entity->m_pRwObject == nullptr)
			continue;

		CColModel* col = entity->GetColModel();
		//if (col == nullptr)
		//	continue;

		CVector position = entity->GetPosition();
		if (entity->m_pLod)
			position = entity->m_pLod->GetPosition();

		float distance = (position - CRenderer::ms_vecCameraPosition).Magnitude();
		XMMATRIX world = RwMatrixToXMMATRIX(reinterpret_cast<RwMatrix*>(entity->GetMatrix()));

		CBoundingBox modelAABB = col->m_boundBox;

		XMFLOAT3 min, max;
		min = *reinterpret_cast<XMFLOAT3*>(&modelAABB.m_vecMin);
		max = *reinterpret_cast<XMFLOAT3*>(&modelAABB.m_vecMax);

		Math::AABB aabb(min, max);
		aabb.Transform(world);

		if ( light->GetFrustum(j).Intersects(aabb))

		{
			culled++;
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
	}

	PrintMessage("%d %d",culled, m_renderableList[0].size());
	CVisibilityPlugins::RenderWeaponPedsForPC();
}