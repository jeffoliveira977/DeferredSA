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

	float distance = (position - CRenderer::ms_vecCameraPosition).Magnitude();

	if (distance < 180.0f)
		m_renderableList.push_back(entity);

}

void PointLightShadow::Update()
{
	gLightManager.SortPointLights();
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
		CVector dx2 = CVector(position.x, position.y, position.z) - coors;
		//PrintMessage("%f %f", dx.Magnitude(), dx2.Magnitude());

		if (dx.Magnitude() > 30.0 /*|| gLightManager.mPointLightList[i].mCastShadow*/)
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


void PointLightShadow::RenderEntities(PointLight* light, int i, int j)
{
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)0);
	if (!CGame::currArea)
		RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)140);

	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);

	MeshCulling::Render(m_renderableList, light->GetFrustum(j));

	CVisibilityPlugins::RenderWeaponPedsForPC();
}