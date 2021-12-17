#include "SpotlightShadow.h"
#include "GTADef.h"
#include "ShaderManager.h"
#include "Renderer.h"
#include "Lights.h"

SpotlightShadow* SpotShadow;

SpotlightShadow::SpotlightShadow()
{
	m_nShadowSize = 0.0;
	mCamera = nullptr;
	for(size_t i = 0; i < 30; i++)
	mColorRaster[i] = nullptr;

	mDepthRaster = nullptr;
}

SpotlightShadow::~SpotlightShadow()
{
	for(size_t i = 0; i < 30; i++)
	RwRasterDestroy(mColorRaster[i]);

	RwRasterDestroy(mDepthRaster);
	RwCameraDestroy(mCamera);
}

void SpotlightShadow::Initialize()
{
	/*mCamera = RwCameraCreate();
	RwCameraSetProjection(mCamera, rwPARALLEL);
	RwCameraSetProjection(mCamera, RwCameraProjection::rwPERSPECTIVE);
	RwCameraSetNearClipPlane(mCamera, 0.1f);
	RwCameraSetFarClipPlane(mCamera, 1000);

	RwCameraSetFrame(mCamera, RwFrameCreate());
	m_nShadowSize = 512.0;
	for(size_t i = 0; i < 30; i++)
	mColorRaster[i]= RwRasterCreate(m_nShadowSize, m_nShadowSize, 32, rwRASTERTYPECAMERATEXTURE);;

	mDepthRaster = RwD3D9RasterCreate(m_nShadowSize, m_nShadowSize, D3DFMT_D24S8, rwRASTERTYPEZBUFFER);*/
}
#include "CascadedShadowRendering.h"
#include "CScene.h"
#include "DeferredRenderer.h"
void SpotlightShadow::Update()
{

	//RwCameraEndUpdate(Scene.m_pRwCamera);
	static int SpotLightCount = 0;

	for(int i = 0; i < Lights::LightList.size(); i++)
	{

		LightData* data = &Lights::LightList[i];
		//PrintMessage("%f", data->type);
		//if(data && data->type)
		//{
			if(SpotLightCount > 28)
			{
				SpotLightCount = 0;
				return;
			}

			PrintMessage("%d", i);


			m_spotPos = XMVectorSet(data->pos.x, data->pos.y, data->pos.z, 1.0);
			m_spotDir = XMVectorSet(data->dir.x, data->dir.y, data->dir.z, 1.0);

			float angle = 40.0;
			float minCos = DegreesToRadians(angle);
			m_spotAngle = minCos;
			m_spotRadius = data->radius;

			XMMATRIX view = XMMatrixLookAtLH(m_spotDir, m_spotPos, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			XMMATRIX projection = XMMatrixPerspectiveFovLH(m_spotAngle, 0.3, 0.01f * m_spotRadius, m_spotRadius);

			m_shadowMatrix[SpotLightCount] = view * projection;
			gRenderState = stageSphereMap;
			RwCameraSetRaster(mCamera, mColorRaster[SpotLightCount]);
			RwCameraSetZRaster(mCamera, mDepthRaster);
			RwCameraClear(mCamera, gColourTop, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
			RwCameraBeginUpdate(mCamera);

			RwD3D9SetTransform(D3DTS_VIEW, &view);
			RwD3D9SetTransform(D3DTS_PROJECTION, &projection);

			ShaderContext->SetViewProjectionMatrix(4, true);

			Renderer::RenderRoads();
			Renderer::RenderEverythingBarRoads();
			RwCameraEndUpdate(mCamera);
			SpotLightCount++;


		//}
	}
//RwCameraBeginUpdate(Scene.m_pRwCamera);
	//Lights::LightList.clear();
	// PrintMessage("%d", Lights::LightList.size());
}
