#include "RenderableScene.h"
#include "CScene.h"
#include "GTADef.h"
#include "ShaderManager.h"
#include "CTimeCycle.h"
#include "DualParaboloidReflection.h"
#include "CubemapReflection.h"
#include "Frustum.h"
#include "AABB.h"
#include "CRenderer.h"
#include "RenderableAABB.h"
#include "ShadowCaster.h"
#include "CascadedShadowRendering.h"
RwFrame* RenderableScene::m_frame;
RwRaster* RenderableScene::m_raster;
RwRaster* RenderableScene::m_depthRaster;
RwCamera* RenderableScene::m_camera;
std::vector<CEntity*> RenderableScene::m_list;
RenderableFrustum* RenderableScene::m_frustumRenderable;
RenderCallback RenderableScene::m_renderCallback;
RenderableAABB *RenderableScene::mRenderableAABB;
RenderableSphere* RenderableScene::mRenderableSphere;
int RenderableScene::mRenderStage;

void RenderableScene::InitGraphicsBuffer()
{
	mRenderableAABB = new RenderableAABB();
	mRenderableAABB->Initialize();

	mRenderableSphere = new RenderableSphere();
	mRenderableSphere->Initialize(10, 10);

	m_frustumRenderable = new RenderableFrustum();
	m_frustumRenderable->InitGraphicsBuffer();

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
	mRenderStage = 0;
}
#include "LightManager.h"
void RenderableScene::Render()
{
	RwCamera* camera = Scene.m_pRwCamera;
	RwCameraSetViewWindow(m_camera, &camera->viewWindow);
	RwFrameTransform(m_frame, &RwCameraGetFrame(camera)->ltm, rwCOMBINEREPLACE);


	//RwV2d view;
	//view.x = view.y = 0.4;
	//RwCameraSetViewWindow(m_camera, &view);
	//RwFrameRotate(m_frame, &Yaxis, 90, rwCOMBINEREPLACE);
	//RwFrameRotate(m_frame, &Xaxis, 90, rwCOMBINEPOSTCONCAT);
	//CVector pos = FindPlayerCoors(0);
	//RwFrameTranslate(m_frame, (RwV3d*)&pos, rwCOMBINEREPLACE);
	//RwFrameUpdateObjects(m_frame);

	//CameraTilt(m_camera, 0, 110.0);
	TranslateCameraZ(m_camera, -340);
	RwFrameUpdateObjects(m_frame);

	RwRGBA ambient = {CTimeCycle::m_CurrentColours.m_nSkyTopRed, CTimeCycle::m_CurrentColours.m_nSkyTopGreen, CTimeCycle::m_CurrentColours.m_nSkyTopBlue, 255};

	RwCameraClear(m_camera, &ambient, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
	RwCameraBeginUpdate(m_camera);
	ShaderContext->SetViewProjectionMatrix(4, true);
	//m_renderCallback();
	DefinedState();

	XMINT4 SolidGreen = {64, 200,  64, 255};
	XMINT4 SolidRed = {200,  64,  64, 255};
	XMINT4 SolidWhite = {255, 255, 255, 255};
	XMINT4 SolidBlack = {0,   0,   0, 255};
	XMINT4 SolidBlue = {64,  64, 200, 95};
	XMINT4 SolidYellow = {200, 200,  64, 255};


	for(int i = 0; i < CRenderer::ms_nNoOfVisibleEntities; ++i)
	{
		CEntity* entity = CRenderer::ms_aVisibleEntityPtrs[i];
		CColModel* col = entity->GetColModel();
		if(col == nullptr)
			continue;

		XMMATRIX world = RwMatrixToXMMATRIX(reinterpret_cast<RwMatrix*>(entity->GetMatrix()));
		CBoundingBox modelAABB = col->m_boundBox;

		XMFLOAT3 min, max;
		min = *reinterpret_cast<XMFLOAT3*>(&modelAABB.m_vecMin);
		max = *reinterpret_cast<XMFLOAT3*>(&modelAABB.m_vecMax);

		Math::AABB aabb(min, max);
		aabb.Transform(world);
		mRenderableAABB->SetAABB(aabb);
		mRenderableAABB->SetWorldMatrix(world);

		Math::BoundingSphere sphere;

		sphere.Center = *reinterpret_cast<XMFLOAT3*>(&col->m_boundSphere.m_vecCenter);
		sphere.Radius = col->m_boundSphere.m_fRadius;

		mRenderableSphere->SetSphere(sphere);
		mRenderableSphere->SetWorldMatrix(world);
		
		/*if(mRenderStage == 0)
		{*/
			if(ShadowCasterEntity->mCulled[1][entity])
			{
				XMINT4 saturate = {0, 60,  0, 255};
				mRenderableAABB->SetColor(SolidGreen, saturate, saturate);
				//mRenderableSphere->SetColor(SolidGreen);
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
			else
			{
				mRenderableAABB->SetColor(SolidRed, SolidBlack, SolidBlack);
				//mRenderableSphere->SetColor(SolidRed);
			}
		//}

		mRenderableAABB->Render();
		//mRenderableSphere->Render();
	}

	m_frustumRenderable->SetColor(SolidBlue);
	m_frustumRenderable->Render(false);
	RwCameraEndUpdate(m_camera);
}

void RenderableScene::SetRenderList(std::vector<CEntity*> list)
{}
