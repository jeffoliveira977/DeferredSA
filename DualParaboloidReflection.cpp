#include "DualParaboloidReflection.h"

#include "CubemapReflection.h"
#include "CWorld.h"
#include "CRenderer.h"
#include "Renderer.h"
#include "DeferredRenderer.h"
#include "ShaderManager.h"
#include "CTimeCycle.h"
#include "GTADef.h"

std::vector<CEntity*> DualParaboloidReflection::m_renderableList[2];
int DualParaboloidReflection::m_size;
XMMATRIX DualParaboloidReflection::m_projectionMatrix;
XMMATRIX DualParaboloidReflection::m_viewMatrix[2];
Math::Frustum DualParaboloidReflection::m_frustum[2];
RwCamera* DualParaboloidReflection::m_camera;
RwRaster* DualParaboloidReflection::m_raster[2];
RwRaster* DualParaboloidReflection::m_depthRaster;
RwFrame* DualParaboloidReflection::m_frame;
float DualParaboloidReflection::m_direction;

void DualParaboloidReflection::Initialize()
{
	m_size = 512;
	m_raster[0] = RwRasterCreate(m_size, m_size, 32, rwRASTERTYPECAMERATEXTURE);
	m_raster[1] = RwRasterCreate(m_size, m_size, 32, rwRASTERTYPECAMERATEXTURE);
	m_depthRaster = RwRasterCreate(m_size, m_size, 32, rwRASTERTYPEZBUFFER);

	m_camera = RwCameraCreate();
	RwCameraSetZRaster(m_camera, m_depthRaster);
	RwCameraSetFrame(m_camera, RwFrameCreate());
}

void DualParaboloidReflection::Release()
{
	RwRasterDestroy(m_raster[0]);
	RwRasterDestroy(m_raster[1]);
	RwRasterDestroy(m_depthRaster);
	RwFrameDestroy(m_frame);
	RwCameraDestroy(m_camera);
}

void DualParaboloidReflection::AddObject(int i, CEntity* entity, float distance)
{
	if(distance > 400)
		return;

	if(distance < 100)
		m_renderableList[i].push_back(entity);
	else if(entity->m_pLod != NULL)
		m_renderableList[i].push_back(entity->m_pLod);
}

void DualParaboloidReflection::ScanSectorList(int sectorX, int sectorY)
{
	if(sectorX >= 0 && sectorY >= 0 && sectorX < MAX_SECTORS_X && sectorY < MAX_SECTORS_Y)
	{
		CSector* sector = GetSector(sectorX, sectorY);
		CRepeatSector* repeatSector = GetRepeatSector(sectorX, sectorY);

		SectorList(sector->m_buildings);
		/*SectorList(sector->m_dummies);
		 SectorList(repeatSector->m_lists[REPEATSECTOR_VEHICLES]);
		 SectorList(repeatSector->m_lists[REPEATSECTOR_PEDS]);*/
		SectorList(repeatSector->m_lists[REPEATSECTOR_OBJECTS]);
	}
}

void DualParaboloidReflection::Update()
{
	CVector pos = FindPlayerCoors(0);
	XMMATRIX translation = XMMatrixTranslation(pos.x, pos.y, pos.z);


	XMMATRIX XRot = XMMatrixRotationY(180.0f);
	XMMATRIX YRot = XMMatrixRotationX(90.0f);
	m_viewMatrix[0] = XRot * YRot;

	// front face
	//m_viewMatrix[0] = XMMatrixLookAtRH(g_XMIdentityR1, g_XMIdentityR0, g_XMIdentityR1);
	m_viewMatrix[0] = XMMatrixInverse(nullptr, m_viewMatrix[0] * translation);
	m_frustum[0].SetMatrix(m_viewMatrix[0] * m_projectionMatrix);
	
	// back face
	m_viewMatrix[1] = XMMatrixLookAtRH(XMVectorZero(), -g_XMIdentityR0, g_XMIdentityR1);
	m_viewMatrix[1] = XMMatrixInverse(nullptr, m_viewMatrix[1] * translation);
	m_frustum[1].SetMatrix(m_viewMatrix[1] * m_projectionMatrix);

	m_projectionMatrix = XMMatrixPerspectiveFovRH(tanf(3.14f / 4.0f), 1.0f, 0.01f, 3000.0f);

	for(size_t i = 0; i < 2; i++)
		m_renderableList[i].clear();

	// Scan entity list
	SetNextScanCode();

	int x = GetSectorX(CRenderer::ms_vecCameraPosition.x);
	int y = GetSectorY(CRenderer::ms_vecCameraPosition.y);

	int sectorCount = 10;
	for(int j = -sectorCount; j < sectorCount; j++)
	{
		for(int i = -sectorCount; i < sectorCount; i++)
		{
			ScanSectorList(x + i, y + j);
		}
	}
}

void DualParaboloidReflection::SectorList(CPtrList& ptrList)
{
	for(auto node = ptrList.GetNode(); node; node = node->pNext)
	{
		CEntity* entity = reinterpret_cast<CEntity*>(node->pItem);
		if(entity->m_nScanCode != CWorld::ms_nCurrentScanCode)
		{
			entity->m_nScanCode = CWorld::ms_nCurrentScanCode;

			CColModel* col = entity->GetColModel();
			if(col == nullptr)
				continue;

			CVector position = entity->GetPosition();
			if(entity->m_pLod)
				position = entity->m_pLod->GetPosition();

			float distance = (position - CRenderer::ms_vecCameraPosition).Magnitude();
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

			for(size_t i = 0; i < 2; i++)
			{
				if(m_frustum[i].Intersects(aabb))
				{
					AddObject(i, entity, distance);
				}
			}
		}
	}

	PrintMessage("Renderable list: %i %i", m_renderableList[0].size(), m_renderableList[1].size());
}

void DualParaboloidReflection::RenderScene()
{
	RwRGBA ambient = {CTimeCycle::m_CurrentColours.m_nSkyTopRed, CTimeCycle::m_CurrentColours.m_nSkyTopGreen, CTimeCycle::m_CurrentColours.m_nSkyTopBlue, 255};

	gRenderState = stageDualParaboloidMap;

	m_direction = 1.0;
	RwCameraSetRaster(m_camera, m_raster[0]);
	RwCameraClear(m_camera, &ambient, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
	RwCameraBeginUpdate(m_camera);
	RwD3D9SetTransform(D3DTS_VIEW, &m_viewMatrix[0]);
	RwD3D9SetTransform(D3DTS_PROJECTION, &m_projectionMatrix);
	ShaderContext->SetViewProjectionMatrix(4, true);
	RenderEntities(0);	
	RwCameraEndUpdate(m_camera);

	m_direction = -1.0;
	RwCameraSetRaster(m_camera, m_raster[1]);
	RwCameraClear(m_camera, &ambient, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
	RwCameraBeginUpdate(m_camera);
	RwD3D9SetTransform(D3DTS_VIEW, &m_viewMatrix[0]);
	RwD3D9SetTransform(D3DTS_PROJECTION, &m_projectionMatrix);
	ShaderContext->SetViewProjectionMatrix(4, true);
	RenderEntities(1);
	RwCameraEndUpdate(m_camera);
}

void DualParaboloidReflection::RenderEntities(int i)
{
	for(auto& entity : m_renderableList[i])
	{
		if(entity->m_pRwObject == nullptr)
			continue;

		entity->m_bImBeingRendered = true;
		entity->Render();
		entity->m_bImBeingRendered = false;
	}
}

