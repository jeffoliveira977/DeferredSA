#include "RenderableScene.h"
#include "CScene.h"
#include "GTADef.h"
#include "ShaderManager.h"
#include "CTimeCycle.h"

RwFrame* RenderableScene::m_frame;
RwRaster* RenderableScene::m_raster;
RwRaster* RenderableScene::m_depthRaster;
RwCamera* RenderableScene::m_camera;
std::vector<CEntity*> RenderableScene::m_list;
RenderableFrustum* RenderableScene::m_frustumRenderable;
RenderCallback RenderableScene::m_renderCallback;

void RenderableScene::InitGraphicsBuffer()
{
	m_frustumRenderable = new RenderableFrustum();
	m_raster = RwRasterCreate(1024, 1024, 32, rwRASTERTYPECAMERATEXTURE);
	m_depthRaster = RwRasterCreate(1024, 1024, 32, rwRASTERTYPEZBUFFER);

	m_camera = RwCameraCreate();
	RwCameraSetProjection(m_camera, RwCameraProjection::rwPERSPECTIVE);
	RwCameraSetNearClipPlane(m_camera, 0.1f);
	RwCameraSetFarClipPlane(m_camera, 3000);

	m_frame = RwFrameCreate();
	RwCameraSetFrame(m_camera, m_frame);
	RwCameraSetRaster(m_camera, m_raster);
	RwCameraSetZRaster(m_camera, m_depthRaster);
	CameraSize(m_camera, nullptr, tanf(3.14f / 4), 1.0f);

	RpWorldAddCamera(Scene.m_pRpWorld, m_camera);
}
#include "EnvironmentMapping.h"
#include "Frustum.h"
#include "AABB.h"

void RenderableScene::Render()
{
	RwCamera* camera = Scene.m_pRwCamera;
	RwCameraSetViewWindow(m_camera, &camera->viewWindow);
	RwFrameTransform(m_frame, &RwCameraGetFrame(camera)->ltm, rwCOMBINEREPLACE);


	RwV2d view;
	view.x = view.y = 0.4;
	RwCameraSetViewWindow(m_camera, &view);
	RwFrameRotate(m_frame, &Yaxis, 90, rwCOMBINEREPLACE);
	RwFrameRotate(m_frame, &Xaxis, 90, rwCOMBINEPOSTCONCAT);
	CVector pos = FindPlayerCoors(0);
	RwFrameTranslate(m_frame, (RwV3d*)&pos, rwCOMBINEREPLACE);
	RwFrameUpdateObjects(m_frame);

	CameraTilt(m_camera, 0, 110.0);
	TranslateCameraZ(m_camera, -300);
	RwFrameUpdateObjects(m_frame);

	RwRGBA ambient = {CTimeCycle::m_CurrentColours.m_nSkyTopRed, CTimeCycle::m_CurrentColours.m_nSkyTopGreen, CTimeCycle::m_CurrentColours.m_nSkyTopBlue, 255};

	RwCameraClear(m_camera, &ambient, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
	RwCameraBeginUpdate(m_camera);
	ShaderContext->SetViewProjectionMatrix(4, true);
	//m_renderCallback();
	DefinedState();
	for(auto& entity : m_list)
	{
		CColModel* col = entity->GetColModel();
		if(col == nullptr)
			continue;

		XMMATRIX world = RwMatrixToXMMATRIX(reinterpret_cast<RwMatrix*>(entity->GetMatrix()));
		CBoundingBox modelAABB = col->m_boundBox;

		XMVECTOR min, max;
		min = XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&modelAABB.m_vecMin));
		max = XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&modelAABB.m_vecMax));
		min = XMVector3Transform(min, world);
		max = XMVector3Transform(max, world);

		Math::AABB aabb;
		XMStoreFloat3(&aabb.Min, min);
		XMStoreFloat3(&aabb.Max, max);
		if(EnvironmentMapping::m_frustum[5].Intersects(aabb))
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
	m_frustumRenderable->RenderFrustum(false);
	RwCameraEndUpdate(m_camera);
}

void RenderableScene::SetRenderList(std::vector<CEntity*> list)
{}
