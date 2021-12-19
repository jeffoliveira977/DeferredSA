#include "SpotlightShadow.h"
#include "GTADef.h"
#include "ShaderManager.h"
#include "Renderer.h"
#include "Lights.h"

#include "CScene.h"
#include "DeferredRenderer.h"
#include "CRenderer.h"
#include "CGame.h"
#include "CVisibilityPlugins.h"

SpotlightShadow* SpotShadow;

SpotlightShadow::SpotlightShadow()
{
	for (size_t i = 0; i < 30; i++)
		mColorRaster[i] = nullptr;

	mDepthRaster = nullptr;
}

SpotlightShadow::~SpotlightShadow()
{
	for (size_t i = 0; i < 30; i++)
		RwRasterDestroy(mColorRaster[i]);

	RwRasterDestroy(mDepthRaster);
}

void SpotlightShadow::Initialize()
{
	m_nShadowSize = 512.0;

	for (size_t i = 0; i < 30; i++)
		mColorRaster[i] = RwD3D9RasterCreate(m_nShadowSize, m_nShadowSize, D3DFMT_R32F, rwRASTERTYPECAMERATEXTURE);

	mDepthRaster = RwRasterCreate(m_nShadowSize, m_nShadowSize, 32, rwRASTERTYPEZBUFFER);
}

void SpotlightShadow::AddObject(int i, CEntity* entity, float distance)
{
	if (distance > 200)
		return;

	if (distance < 100)
		m_renderableList[i].push_back(entity);
	else if (entity->m_pLod != NULL)
		m_renderableList[i].push_back(entity->m_pLod);
}

void SpotlightShadow::SectorList(CPtrList& ptrList)
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

			int n = 0;
			for (int i = 0; i < Lights::LightList.size(); i++)
			{
				LightData* data = &Lights::LightList[i];
				if (data && data->type)
				{
					if (m_frustum[n].Intersects(aabb))
					{
						AddObject(n, entity, distance);
					}
					n++;
				}
			}
		}
	}
}

void SpotlightShadow::ScanSectorList(int sectorX, int sectorY)
{
	if (sectorX >= 0 && sectorY >= 0 && sectorX < MAX_SECTORS_X && sectorY < MAX_SECTORS_Y)
	{
		CSector* sector = GetSector(sectorX, sectorY);
		CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);

		SectorList(sector->m_buildings);
		//SectorList(sector->m_dummies);
		 SectorList(repeatSector->m_lists[REPEATSECTOR_VEHICLES]);
		 SectorList(repeatSector->m_lists[REPEATSECTOR_PEDS]);
		//SectorList(repeatSector->m_lists[REPEATSECTOR_OBJECTS]);
	}
}

void SpotlightShadow::Update()
{
	static int n = 0;
	for(int i = 0; i < Lights::LightList.size(); i++)
	{
		LightData* data = &Lights::LightList[i];
		//PrintMessage("%f", data->type);
		if(data && data->type)
		{
		
			m_spotPos = XMVectorSet(data->pos.x, data->pos.y, data->pos.z, 1.0);
			m_spotDir = XMVectorSet(data->dir.x, data->dir.y, data->dir.z, 1.0);

			float angle = 30.0;
			float minCos = DegreesToRadians(angle);
			m_spotAngle = minCos;
			m_spotRadius = data->radius;

			XMMATRIX view = XMMatrixLookAtLH(m_spotDir, m_spotPos, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			XMMATRIX projection = XMMatrixPerspectiveFovLH(m_spotAngle, 1.777, 0.01f * m_spotRadius, m_spotRadius);

			m_shadowMatrix[n] = view * projection;
			gRenderState = stageCascadeShadow;

			RwD3D9SetRenderTarget(0, mColorRaster[i]);
			rwD3D9SetDepthStencil(mDepthRaster);

			D3DVIEWPORT9 viewport;
			viewport.X = 0;
			viewport.Y = 0;
			viewport.Width = mColorRaster[i]->width;
			viewport.Height = mColorRaster[i]->height;
			viewport.MinZ = 0;
			viewport.MaxZ = 1;
			RwD3DDevice->SetViewport(&viewport);

			rwD3D9Clear(nullptr, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);

			if (rwD3D9TestState())
			{
				RwD3D9SetTransform(D3DTS_VIEW, &view);
				RwD3D9SetTransform(D3DTS_PROJECTION, &projection);

				_rwD3D9SetVertexShaderConstant(4, &view, 4);
				_rwD3D9SetVertexShaderConstant(8, &projection, 4);

				RenderEntities(n);
			}

			n++;
		}
	}
}

void SpotlightShadow::RenderEntities(int i)
{
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
	if (!CGame::currArea)
		RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)140);

	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);

	for (auto& entity : m_renderableList[i])
	{
		if (entity->m_pRwObject == nullptr)
			continue;

		if (entity->m_nType == ENTITY_TYPE_PED)
		{
			CPed* ped = static_cast<CPed*>(entity);
			CTaskSimpleJetPack* jetPack = ped->m_pIntelligence->GetTaskJetPack();
			if (jetPack && jetPack->m_pJetPackClump)
				RpClumpRender(jetPack->m_pJetPackClump);
		}

		if (!entity->m_bBackfaceCulled)
		{
			RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
		}

		entity->m_bImBeingRendered = true;
		entity->Render();
		entity->m_bImBeingRendered = false;
	} 
	CVisibilityPlugins::RenderWeaponPedsForPC();
}

