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

#include "CPointLights.h"
#include "CCamera.h"

#include "MeshCulling.h"

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
		RwRasterDestroy(mColorRaster[i]);
	}
	RwRasterDestroy(mDepthRaster);
}

void PointLightShadow::Initialize()
{
	m_nShadowSize = 512;

	for (size_t i = 0; i < 30; i++)
	{
		mColorRaster[i] = RwD3D9RasterCreate(m_nShadowSize, m_nShadowSize, D3DFMT_R32F, rwRASTERTYPECAMERATEXTURE);
		rwD3D9CubeRasterCreate(mColorRaster[i], D3DFMT_R32F, 1);

		//mColorCube[i] = RwD3D9RasterCreate(m_nShadowSize*6, m_nShadowSize, D3DFMT_G32R32F, rwRASTERTYPECAMERATEXTURE);
	}

	mDepthRaster = RwRasterCreate(m_nShadowSize * 6, m_nShadowSize, 32, rwRASTERTYPEZBUFFER);
}

void PointLightShadow::AddObject(CEntity* entity)
{
	if (entity->m_pRwObject == nullptr)
		return;

	CColModel* col = entity->GetColModel();
	if (col == nullptr)
		return;

	CVector position = entity->GetPosition();
	if (entity->m_pLod)
		position = entity->m_pLod->GetPosition();

	float distance = (position - TheCamera.GetPosition()).Magnitude();

	if (distance < 100.0f)
		m_renderableList.push_back(entity);

}
float rwV3D_Dist2(CVector& a, CVector& b)
{
	float
		dX = b.x - a.x,
		dY = b.y - a.y,
		dZ = b.z - a.z;

	return sqrt(dX * dX + dY * dY + dZ * dZ);
}
void PointLightShadow::Update()
{
	gLightManager.SortPointLights();

	/*std::sort(m_renderableList.begin(), m_renderableList.end(), [&](CEntity* a, CEntity* b)
		{
			return (rwV3D_Dist2(a->GetPosition(), TheCamera.GetPosition()) < rwV3D_Dist2(b->GetPosition(), TheCamera.GetPosition()));
		});*/

	uint32_t maxLights = min((size_t)16, gLightManager.GetPointLightCount());

	//RwD3D9SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED);
	auto coors = FindPlayerCoors(-1);
	RWSRCGLOBAL(curCamera) = Scene.m_pRwCamera;
	for (size_t i = 0; i < maxLights; i++)
	{
		PointLight* light = gLightManager.GetPointLightAt(i);
		auto position = light->GetPosition();
		auto radius = light->GetRadius();
		float visibleRadius = radius + 15.0f;
		CVector dx = CVector(position.x, position.y, position.z) - TheCamera.GetPosition();

	/*	if (dx.x >= visibleRadius || dx.x <= -visibleRadius)
		{
			PrintMessage("%f %f %f", dx.x, dx.y, dx.z);
			continue;
		}
		if (dx.y >= visibleRadius || dx.y <= -visibleRadius)
		{
			PrintMessage("%f %f %f", dx.x, dx.y, dx.z);
			continue;
		}*/

		if (dx.Magnitude() > 30.0 || !gLightManager.mPointLightList[i].mCastShadow)
		{
			gLightManager.mPointLightList[i].mDrawShadow = false;
			continue;
		}

		_rwD3D9SetPixelShaderConstant(1, &position, 1);
		_rwD3D9SetPixelShaderConstant(2, &radius, 1);

		gLightManager.mPointLightList[i].mShadowRaster = mColorRaster[i];

		gRenderState = stagePointShadow;

		for (size_t j = 0; j < 6; j++)
		{
			auto viewMatrix = light->GetViewMatrix(j);
			auto projectionMatrix = light->GetProjection();

			_rwD3D9CubeRasterSelectFace(mColorRaster[i], j);
			RwD3D9SetRenderTarget(0, mColorRaster[i]);
			rwD3D9SetDepthStencil(mDepthRaster);

			D3DVIEWPORT9 viewport;
			viewport.X = 0;
			viewport.Y = 0;
			viewport.Width = m_nShadowSize;
			viewport.Height = m_nShadowSize;
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

				RenderEntities(light, i, j);
			}
		}
	}
	RWSRCGLOBAL(curCamera) = NULL;

	// Restore all channels colors
	RwD3D9SetRenderState(D3DRS_COLORWRITEENABLE,
		D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED |
		D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
}

bool CheckLamps2(CEntity* entity)
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
void RenderEntity2(CEntity* entity)
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
#include "CModelInfo.h"
void PointLightShadow::RenderEntities(PointLight* light, int i, int j)
{
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)0);
	if (!CGame::currArea)
		RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)140);

	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);

	//MeshCulling::Render(m_renderableList, light->GetFrustum(j));

	for (auto entity : m_renderableList)
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

		//if( castEntity /*&& CheckLamps2(entity)*/)
		//	RenderEntity2(entity);
		//else
		if (MeshCulling::InFrustum(entity, light->GetFrustum(j)))
		{
			RenderEntity2(entity);
		}
	}

	CVisibilityPlugins::RenderWeaponPedsForPC();
}