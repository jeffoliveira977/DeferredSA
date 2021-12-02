#include "CubemapReflection.h"
#include "CWorld.h"
#include "CRenderer.h"
#include "Renderer.h"
#include "DeferredRenderer.h"
#include "ShaderManager.h"
#include "CTimeCycle.h"
#include "GTADef.h"

std::vector<CEntity*> CubemapReflection::m_renderableList[6];
 int CubemapReflection::m_size;
 XMMATRIX CubemapReflection::m_projectionMatrix;
 XMMATRIX CubemapReflection::m_viewMatrix[6];
 Math::Frustum CubemapReflection::m_frustum[6];
 RwCamera* CubemapReflection::m_camera;
 RwRaster* CubemapReflection::m_raster;
 RwRaster* CubemapReflection::m_cubeRaster;
 RwRaster* CubemapReflection::m_depthRaster;
 RwFrame* CubemapReflection::m_frame;

void CubemapReflection::Initialize()
{
	m_size = 512;
	m_cubeRaster = RwRasterCreate(m_size, m_size, 32, rwRASTERTYPECAMERATEXTURE);
	rwD3D9CubeRasterCreate(m_cubeRaster, D3DFMT_A8R8G8B8, 1);
	m_depthRaster = RwRasterCreate(m_size, m_size, 32, rwRASTERTYPEZBUFFER);

	m_camera = RwCameraCreate(); 
	RwCameraSetZRaster(m_camera, m_depthRaster);
	RwCameraSetFrame(m_camera, RwFrameCreate());
}

void CubemapReflection::Release()
{
	RwRasterDestroy(m_cubeRaster);
	RwRasterDestroy(m_depthRaster);
	RwFrameDestroy(m_frame);
	RwCameraDestroy(m_camera);
}

void CubemapReflection::AddObject(int i, CEntity* entity, float distance)
{
	if(distance > 400)
		return;

	if(distance < 100)
		m_renderableList[i].push_back(entity);
	else if(entity->m_pLod != NULL)
		m_renderableList[i].push_back(entity->m_pLod);
}

void CubemapReflection::ScanSectorList(int sectorX, int sectorY)
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

void CubemapReflection::Update()
{

	// Update matrices
	m_projectionMatrix = XMMatrixPerspectiveFovRH(tanf(3.14f / 4.0f), 1.0f, 0.01f, 3000.0f);
	CVector pos = FindPlayerCoors(0);

	for(size_t i = 0; i < 6; i++)
	{
		m_renderableList[i].clear();

		XMVECTOR lookAt;
		XMVECTOR up;

		switch((D3DCUBEMAP_FACES)i)
		{
			case D3DCUBEMAP_FACE_POSITIVE_X:
				lookAt = g_XMIdentityR0;
				up = g_XMIdentityR1;
				break;
			case D3DCUBEMAP_FACE_NEGATIVE_X:
				lookAt = -g_XMIdentityR0;
				up = g_XMIdentityR1;
				break;
			case D3DCUBEMAP_FACE_POSITIVE_Y:
				lookAt = g_XMIdentityR1;
				up = -g_XMIdentityR2;
				break;
			case D3DCUBEMAP_FACE_NEGATIVE_Y:
				lookAt = -g_XMIdentityR1;
				up = g_XMIdentityR2;
				break;
			case D3DCUBEMAP_FACE_POSITIVE_Z:
				lookAt = g_XMIdentityR2;
				up = g_XMIdentityR1;
				break;
			case D3DCUBEMAP_FACE_NEGATIVE_Z:
				lookAt = -g_XMIdentityR2;
				up = g_XMIdentityR1;
				break;
		}

		XMMATRIX translation = XMMatrixTranslation(pos.x, pos.y, pos.z);
		m_viewMatrix[i] = XMMatrixLookAtRH(XMVectorZero(), lookAt, up);
		m_viewMatrix[i] = XMMatrixInverse(nullptr, m_viewMatrix[i] * translation);
		m_frustum[i].SetMatrix(m_viewMatrix[i] * m_projectionMatrix);
	}

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

void CubemapReflection::SectorList(CPtrList& ptrList)
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

			for(size_t i = 0; i < 6; i++)
			{
				if(m_frustum[i].Intersects(aabb))
				{
					AddObject(i, entity, distance);
				}
			}
		}
	}

	PrintMessage("Renderable list: %i %i %i %i %i %i", m_renderableList[0].size(), m_renderableList[1].size(), m_renderableList[2].size(), 
													   m_renderableList[3].size(), m_renderableList[4].size(), m_renderableList[5].size());
}

void CubemapReflection::RenderScene()
{
	gRenderState = stageReflectionCubemap;
	RwRGBA ambient = {CTimeCycle::m_CurrentColours.m_nSkyTopRed, CTimeCycle::m_CurrentColours.m_nSkyTopGreen, CTimeCycle::m_CurrentColours.m_nSkyTopBlue, 255};

	for(int i = 0; i < 6; i++)
	{
		_rwD3D9CubeRasterSelectFace(m_cubeRaster, i);
		RwCameraSetRaster(m_camera, m_cubeRaster);
		RwCameraClear(m_camera, &ambient, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
		RwCameraBeginUpdate(m_camera);

		RwD3D9SetTransform(D3DTS_VIEW, &m_viewMatrix[i]);
		RwD3D9SetTransform(D3DTS_PROJECTION, &m_projectionMatrix);

		ShaderContext->SetViewProjectionMatrix(4, true);

		for(auto& entity : m_renderableList[i])
		{
			if(entity->m_pRwObject == nullptr)
				continue;

			entity->m_bImBeingRendered = true;
			entity->Render();
			entity->m_bImBeingRendered = false;
		}

		RwCameraEndUpdate(m_camera);
	}
}
