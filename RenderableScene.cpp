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

void RenderableScene::Render()
{
	RwCamera* camera = Scene.m_pRwCamera;
	RwCameraSetViewWindow(m_camera, &camera->viewWindow);
	RwFrameTransform(m_frame, &RwCameraGetFrame(camera)->ltm, rwCOMBINEREPLACE);

	TranslateCameraZ(m_camera, -1600);
	RwFrameUpdateObjects(m_frame);
	RwRGBA ambient = {CTimeCycle::m_CurrentColours.m_nSkyTopRed, CTimeCycle::m_CurrentColours.m_nSkyTopGreen, CTimeCycle::m_CurrentColours.m_nSkyTopBlue, 255};

	RwCameraClear(m_camera, &ambient, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
	RwCameraBeginUpdate(m_camera);
	ShaderContext->SetViewProjectionMatrix(4, true);
	//m_renderCallback();
	DefinedState();
	for(auto& entity : m_list)
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
	m_frustumRenderable->RenderFrustum(true);
	RwCameraEndUpdate(m_camera);
	
}

void RenderableScene::SetRenderList(std::vector<CEntity*> list)
{}
