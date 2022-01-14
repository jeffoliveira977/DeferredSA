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
#include "LightManager.h"
#include "CCamera.h"

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
	m_nShadowSize = 512;

	for (size_t i = 0; i < 30; i++)
		mColorRaster[i] = RwD3D9RasterCreate(m_nShadowSize, m_nShadowSize, D3DFMT_R32F, rwRASTERTYPECAMERATEXTURE);
	mDepthRaster = RwRasterCreate(m_nShadowSize, m_nShadowSize, 32, rwRASTERTYPEZBUFFER);
}

void SpotlightShadow::AddObject(CEntity* entity)
{
	if (entity->m_pRwObject == nullptr)
		return;

	CColModel* col = entity->GetColModel();
	if (col == nullptr)
		return;

	CVector position = entity->GetPosition();
	if (entity->m_pLod)
		position = entity->m_pLod->GetPosition();

	float distance = (position - CRenderer::ms_vecCameraPosition).Magnitude();

	if (distance < 80.0f)
		m_renderableList.push_back(entity);
}

void SpotlightShadow::Update()
{
	gLightManager.SortSpotLights();
	//RwD3D9SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED);

	RWSRCGLOBAL(curCamera) = Scene.m_pRwCamera;

	uint32_t maxLights = min((size_t)20, gLightManager.GetSpotLightCount());

	for (size_t i = 0; i < maxLights; i++)
	{
		auto light = gLightManager.GetSpotLightAt(i);

		auto position = light->GetPosition();
		auto radius = light->GetRadius();

		CVector dx = CVector(position.x, position.y, position.z) - TheCamera.GetPosition();

		if (dx.Magnitude() > 20.0f || !gLightManager.mSpotLightList[i].mCastShadow)
		{
			gLightManager.mSpotLightList[i].mDrawShadow = false;
			continue;
		}

		auto viewMatrix = light->GetViewMatrix();
		auto projectionMatrix = light->GetProjection();

		gRenderState = stageCascadeShadow;
		//gRenderState = stagePointShadow;

		_rwD3D9SetPixelShaderConstant(1, &position, 1);
		_rwD3D9SetPixelShaderConstant(2, &radius, 1);

		RwD3D9SetRenderTarget(0, mColorRaster[i]);
		rwD3D9SetDepthStencil(mDepthRaster);
		gLightManager.mSpotLightList[i].mColorRaster = mColorRaster[i];

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
			RwD3D9SetTransform(D3DTS_VIEW, &viewMatrix);
			RwD3D9SetTransform(D3DTS_PROJECTION, &projectionMatrix);

			_rwD3D9SetVertexShaderConstant(4, &viewMatrix, 4);
			_rwD3D9SetVertexShaderConstant(8, &projectionMatrix, 4);

			RenderEntities(light, i);
		}
	}	
	RWSRCGLOBAL(curCamera) = NULL;

	// Restore all channels colors
	RwD3D9SetRenderState(D3DRS_COLORWRITEENABLE,
		D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED |
		D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
}

#include "MeshCulling.h"
void SpotlightShadow::RenderEntities(SpotLight*light, int i)
{
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)3);
	if (!CGame::currArea)
		RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)140);

	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);

	for (auto entity : m_renderableList)
	{
		//if (MeshCulling::InFrustum(entity, light->GetFrustum()))
		//{
			if (entity == nullptr || entity->m_pRwObject == nullptr)
				continue;

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
		//}
	}
	CVisibilityPlugins::RenderWeaponPedsForPC();
}

