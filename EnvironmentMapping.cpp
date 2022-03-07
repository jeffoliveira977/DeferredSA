#include "EnvironmentMapping.h"
#include "GTADef.h"
#include "DeferredRenderer.h"
#include "CScene.h"
#include "CTimeCycle.h"
#include "common.h"

#include "Renderer.h"
#include "CPed.h"
#include "CCamera.h"
#include "ShaderManager.h"
RenderCallback EnvironmentMapping::m_renderCallback;

RwRaster* EnvironmentMapping::m_cubeRaster;
RwRaster* EnvironmentMapping::m_paraboloidRaster[2];
RwRaster* EnvironmentMapping::m_sphericalRaster;
RwRaster* EnvironmentMapping::m_depthRaster;
RwFrame* EnvironmentMapping::m_envFrame;
RwCamera* EnvironmentMapping::m_envCamera;

float EnvironmentMapping::m_direction;
D3DXMATRIX EnvironmentMapping::m_paraboloidBasis;

int EnvironmentMapping::m_weight;

void EnvironmentMapping::SetRenderCallback(RenderCallback callback)
{
	m_renderCallback = callback;
}

void EnvironmentMapping::InitializeGraphics()
{
	m_weight = 1024;
	m_paraboloidRaster[0] = RwRasterCreate(m_weight, m_weight, 32, rwRASTERTYPECAMERATEXTURE);
	m_paraboloidRaster[1] = RwRasterCreate(m_weight, m_weight, 32, rwRASTERTYPECAMERATEXTURE);
	m_sphericalRaster = RwRasterCreate(m_weight, m_weight, 32, rwRASTERTYPECAMERATEXTURE);

	m_cubeRaster = RwRasterCreate(m_weight, m_weight, 32, rwRASTERTYPECAMERATEXTURE | rwRASTERDONTALLOCATE);
	_rwD3D9CubeRasterCreate(m_cubeRaster, D3DFMT_A8R8G8B8, 1);

	m_depthRaster = RwRasterCreate(m_weight, m_weight, 32, rwRASTERTYPEZBUFFER);

	m_envCamera = RwCameraCreate();
	RwCameraSetProjection(m_envCamera, RwCameraProjection::rwPERSPECTIVE);
	RwCameraSetNearClipPlane(m_envCamera, 0.1f);
	RwCameraSetFarClipPlane(m_envCamera, 3000);

	m_envFrame = RwFrameCreate();
	RwCameraSetFrame(m_envCamera, m_envFrame);
	RwCameraSetRaster(m_envCamera, m_paraboloidRaster[0]);
	RwCameraSetZRaster(m_envCamera, m_depthRaster);
	CameraSize(m_envCamera, nullptr, tanf(3.14f / 4), 1.0f);

	RpWorldAddCamera(Scene.m_pRpWorld, m_envCamera);
}

void EnvironmentMapping::PlaneMap()
{
	gRenderState = stageReflection;

	//RwCameraEndUpdate(Scene.m_pRwCamera);
	//RwCameraSetViewWindow(m_envCamera, &Scene.m_pRwCamera->viewWindow);

	RwRGBA ambient = {CTimeCycle::m_CurrentColours.m_nSkyTopRed, CTimeCycle::m_CurrentColours.m_nSkyTopGreen, CTimeCycle::m_CurrentColours.m_nSkyTopBlue, 255};

	//auto ltm = RwCameraGetFrame(Scene.m_pRwCamera)->ltm;
	//RwFrameTransform(m_envFrame, &ltm, rwCOMBINEREPLACE);

	CVector campos = FindPlayerCoors(0);
	float waterDept, waterLevel, waterGround;
	CWaterLevel__GetWaterDepth(campos, &waterDept, &waterLevel, &waterGround);
	auto oldz = Scene.m_pRwCamera->zBuffer;
	auto oldb = Scene.m_pRwCamera->frameBuffer;
	RwCameraSetRaster(Scene.m_pRwCamera, m_paraboloidRaster[0]);
	RwCameraSetZRaster(Scene.m_pRwCamera, m_envCamera->zBuffer);

	RwCameraClear(Scene.m_pRwCamera, &ambient, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);

	RwCameraBeginUpdate(Scene.m_pRwCamera);

	DirectX::XMMATRIX viewMat, projectionMat;
	RwD3D9GetTransform(D3DTS_PROJECTION, &projectionMat);
	RwD3D9GetTransform(D3DTS_VIEW, &viewMat);
	DirectX::XMVECTOR plane = {0, 0, -1, waterLevel};
	plane = DirectX::XMPlaneNormalize(plane);

	DirectX::XMMATRIX viewProjectionMat = viewMat * projectionMat;
	viewProjectionMat = DirectX::XMMatrixInverse(0, viewProjectionMat);
	viewProjectionMat = DirectX::XMMatrixTranspose(viewProjectionMat);
	plane = DirectX::XMPlaneTransform(plane, viewProjectionMat);

	RwD3D9SetRenderState(D3DRS_CLIPPLANEENABLE, TRUE);
	RwD3D9SetClipPlane(0, (RwV4d*)&plane);
	m_renderCallback();
	RwCameraEndUpdate(Scene. m_pRwCamera);

	RwCameraSetRaster(Scene.m_pRwCamera, oldb);
	RwCameraSetZRaster(Scene.m_pRwCamera, oldz);
	//CRenderer__ConstructRenderList();
	//RwCameraSetRaster(m_envCamera, m_paraboloidRaster[1]);
	//RwCameraClear(m_envCamera, &ambient, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
	//RwCameraBeginUpdate(m_envCamera);

	//DirectX::XMMATRIX viewMat, projectionMat;
	//RwD3D9GetTransform(D3DTS_PROJECTION, &projectionMat);
	//RwD3D9GetTransform(D3DTS_VIEW, &viewMat);

	//plane = {0, 0, 1, waterLevel};
	//plane = DirectX::XMPlaneNormalize(plane);

	//viewProjectionMat = viewMat * projectionMat;
	//viewProjectionMat = DirectX::XMMatrixInverse(0, viewProjectionMat);
	//viewProjectionMat = DirectX::XMMatrixTranspose(viewProjectionMat);
	//plane = DirectX::XMPlaneTransform(plane, viewProjectionMat);

	//rwD3D9SetRenderState(D3DRS_CLIPPLANEENABLE, TRUE);
	//RwD3D9SetClipPlane(0, (RwV4d*)&plane);
	//m_renderCallback();
	//RwCameraEndUpdate(m_envCamera);

	RwD3D9SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
//	RwCameraBeginUpdate(Scene.m_pRwCamera);
}

#include "CascadedShadowRendering.h"
#include "CCamera.h"
#include "CCam.h"
#include "GTADef.h"

void RenderBoundingBox(RwBBox* bbox, RwMatrix* ltm, RwRGBA color)
{
	RwInt32 i;
	RwIm3DVertex imVertex[8];
	RwImVertexIndex indices[24] =
	{
		0, 1, 1, 3, 3, 2, 2, 0, 4, 5, 5, 7,
		7, 6, 6, 4, 0, 4, 1, 5, 2, 6, 3, 7
	};

	for(i = 0; i < 8; i++)
	{
		RwIm3DVertexSetPos(&imVertex[i],
						   i & 1 ? bbox->sup.x : bbox->inf.x,
						   i & 2 ? bbox->sup.y : bbox->inf.y,
						   i & 4 ? bbox->sup.z : bbox->inf.z
		);

		RwIm3DVertexSetRGBA(&imVertex[i], color.red, color.green , color.blue, color.alpha);
	}

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEFLAT);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);

	if(RwIm3DTransform(imVertex, 8, ltm, rwIM3D_ALLOPAQUE))
	{
		RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indices, 24);
		RwIm3DEnd();
	}

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEGOURAUD);

	return;
}

void GenerateBoundingSphereVertices(RwUInt32 slices, RwUInt32 stacks, std::vector<RwIm3DVertex>& vertices, std::vector<RwImVertexIndex>& indices, RwRGBA *color = 0)
{
	int numVertices = 2 + slices * (stacks - 1);
	int numIndices = 2 * slices + (stacks - 2) * (2 * slices);

	vertices.resize(numVertices);
	indices.resize(numIndices * 3);

	float phiStep, phiStart;
	float thetaStep, theta;
	int vertexCount, indicesCount;

	phiStep = -2 * PI / slices;
	phiStart = PI / 2;

	thetaStep = PI / stacks;
	theta = thetaStep;

	vertexCount = 0;
	indicesCount = 0;

	RwRGBA defaultColor = {0, 128, 255, 255};
	if(color == NULL)
		color = &defaultColor;

	RwIm3DVertexSetPos(&vertices[vertexCount], 0.0f, 0.0f, 1.0f);
	RwIm3DVertexSetRGBA(&vertices[vertexCount], 
						color->red, color->green, 
						color->blue, color->alpha);
	vertexCount++;

	int i, j;
	for(i = 0; i < stacks - 1; i++)
	{
		float sinTheta = sinf(theta);
		float cosTheta = cosf(theta);

		for(j = 0; j < slices; j++)
		{
			float x = sinTheta * cosf(phiStart);
			float y = sinTheta * sinf(phiStart);

			RwIm3DVertexSetPos(&vertices[vertexCount], x, y, cosTheta);
			RwIm3DVertexSetRGBA(&vertices[vertexCount], 
								color->red, color->green, 
								color->blue, color->alpha);
			vertexCount++;

			phiStart += phiStep;

			if(j > 0)
			{
				if(i == 0)
				{
					indices[indicesCount++] = 0;
					indices[indicesCount++] = j + 1;
					indices[indicesCount++] = j;
				}
				else
				{
					indices[indicesCount++] = (i - 1) * slices + j;
					indices[indicesCount++] = (i - 1) * slices + j + 1;
					indices[indicesCount++] = i * slices + j;

					indices[indicesCount++] = (i - 1) * slices + j + 1;
					indices[indicesCount++] = i * slices + j + 1;
					indices[indicesCount++] = i * slices + j;
				}
			}
		}

		theta += thetaStep;

		if(i == 0)
		{
			indices[indicesCount++] = 0;
			indices[indicesCount++] = 1;
			indices[indicesCount++] = j;
		}
		else
		{
			indices[indicesCount++] = (i - 1) * slices + j;
			indices[indicesCount++] = (i - 1) * slices + 1;
			indices[indicesCount++] = i * slices + j;

			indices[indicesCount++] = (i - 1) * slices + 1;
			indices[indicesCount++] = i * slices + 1;
			indices[indicesCount++] = i * slices + j;
		}
	}

	RwIm3DVertexSetPos(&vertices[vertexCount], 0.0f, 0.0f, -1.0f);
	RwIm3DVertexSetRGBA(&vertices[vertexCount], 
						color->red, color->green, 
						color->blue, color->alpha);

	for(j = 1; j < slices; j++)
	{
		indices[indicesCount++] = (i - 1) * slices + j;
		indices[indicesCount++] = (i - 1) * slices + j + 1;
		indices[indicesCount++] = vertexCount;
	}

	indices[indicesCount++] = (i - 1) * slices + j;
	indices[indicesCount++] = (i - 1) * slices + 1;
	indices[indicesCount++] = vertexCount;
}

void RenderBoudingSphereCollision(CCollisionData* colData, RwMatrix ltm, RwRGBA *color)
{
	static std::vector<RwIm3DVertex> vertices;
	static std::vector<RwImVertexIndex> indices;

	static bool generateVertices = true;

	if(generateVertices)
	{
		GenerateBoundingSphereVertices(20, 20, vertices, indices, color);
		generateVertices = false;
	}

	for(int i = 0; i < colData->m_nNumSpheres; i++)
	{
		RwV3d c = colData->m_pSpheres[i].m_vecCenter.ToRwV3d();
		float r = colData->m_pSpheres[i].m_fRadius;

		RwMatrix trans = ltm;
		RwMatrixTranslate(&trans, &c, rwCOMBINEPRECONCAT);
		RwV3d scale = {r, r, r};
		RwMatrixScale(&trans, &scale, rwCOMBINEPRECONCAT);

		if(RwIm3DTransform(vertices.data(), vertices.size(), &trans, rwIM3D_ALLOPAQUE))
		{
			RwIm3DRenderIndexedPrimitive(rwPRIMTYPEPOLYLINE, indices.data(), indices.size());
			RwIm3DEnd();
		}
	}

	return;
}

void RenderTriangleCollision(CCollisionData* colData, RwMatrix ltm)
{
	for(int i = 0; i < colData->m_nNumTriangles; i++)
	{
		CVector points[3];
		colData->GetTrianglePoint(points[0], colData->m_pTriangles[i].m_nVertA);
		colData->GetTrianglePoint(points[1], colData->m_pTriangles[i].m_nVertB);
		colData->GetTrianglePoint(points[2], colData->m_pTriangles[i].m_nVertC);

		RwIm3DVertex imVertex[3];
		for(size_t j = 0; j < 3; j++)
		{
			RwIm3DVertexSetPos(&imVertex[j], points[j].x, points[j].y, points[j].z);
			RwIm3DVertexSetRGBA(&imVertex[j], 0, 255, 0, 255);
		}

		if(RwIm3DTransform(imVertex, 3, &ltm, rwIM3D_ALLOPAQUE))
		{
			RwIm3DRenderPrimitive(rwPRIMTYPEPOLYLINE);
			RwIm3DEnd();
		}
	}
}

void RenderLinesCollision(CCollisionData* colData, RwMatrix ltm)
{
	for(int i = 0; i < colData->m_nNumLines; i++)
	{	
		CVector points[2];
		points[0] = colData->m_pLines[i].m_vecStart;
		points[1] = colData->m_pLines[i].m_vecEnd;

		RwIm3DVertex imVertex[2];
		RwIm3DVertexSetPos(&imVertex[0], points[0].x, points[0].y, points[0].z);
		RwIm3DVertexSetRGBA(&imVertex[0], 128, 128, 128, 255);
		RwIm3DVertexSetPos(&imVertex[1], points[1].x, points[1].y, points[1].z);
		RwIm3DVertexSetRGBA(&imVertex[1], 128, 128, 128, 255);

		if(RwIm3DTransform(imVertex, 2, &ltm, rwIM3D_ALLOPAQUE))
		{
			RwIm3DRenderLine(0, 1);
			RwIm3DEnd();
		}
	}
}

void RenderBoundingBoxCollision(CCollisionData* colData, RwMatrix ltm, RwRGBA color)
{
	for(auto i = 0; i < colData->m_nNumBoxes; i++)
	{
		RwBBox bb;
		bb.inf = colData->m_pBoxes[i].m_vecMin.ToRwV3d();
		bb.sup = colData->m_pBoxes[i].m_vecMax.ToRwV3d();
		RenderBoundingBox(&bb, &ltm, color);
	}
}


struct CActiveOccluderLine
{
	CVector2D m_vecOrigin;
	CVector2D m_vecDirection;
	float m_fLength;
};

struct CActiveOccluder
{
	CActiveOccluderLine m_aLines[6];
	short m_wDepth;
	char m_cLinesCount;
	char m_cUnkn;
	CVector m_aVectors[3];
	float m_fRadiuses[3];
};

struct COccluder
{
	unsigned short m_fMidX;
	unsigned short m_fMidY;
	unsigned short m_fMidZ;
	unsigned short m_fWidthY;
	unsigned short m_fWidthX;
	unsigned short m_fHeight;
	char m_cRotX;
	char m_cRotY;
	char m_cRotZ;
	char m_cPad;
	unsigned short m_nFlags;
};

class COcclusion
{
public:
	static constexpr int MAX_INTERIOR_OCCLUDERS = 40;
	static constexpr int MAX_MAP_OCCLUDERS = 1000;
	static constexpr int MAX_ACTIVE_OCCLUDERS = 28;

	static COccluder(&aInteriorOccluders)[MAX_INTERIOR_OCCLUDERS];
	static COccluder(&aOccluders)[MAX_MAP_OCCLUDERS];
	static CActiveOccluder(&aActiveOccluders)[MAX_ACTIVE_OCCLUDERS];

	static int& NumInteriorOcculdersOnMap;
	static int& NumOccludersOnMap;
	static int& NumActiveOccluders;
	static short& FarAwayList;
	static short& NearbyList;
	static short& ListWalkThroughFA;
	static short& PreviousListWalkThroughFA;

	static float& gMinXInOccluder;
	static float& gMaxXInOccluder;
	static float& gMinYInOccluder;
	static float& gMaxYInOccluder;

	static bool(&gOccluderCoorsValid)[8];
	static CVector(&gOccluderCoors)[8];
	static CVector(&gOccluderCoorsOnScreen)[8];
	static CVector& gCenterOnScreen;
};

RwIm2DVertex vertexbufferT[2];

COccluder(&COcclusion::aInteriorOccluders)[MAX_INTERIOR_OCCLUDERS] = *(COccluder(*)[MAX_INTERIOR_OCCLUDERS])0xC73CC8;
COccluder(&COcclusion::aOccluders)[MAX_MAP_OCCLUDERS] = *(COccluder(*)[MAX_MAP_OCCLUDERS])0xC73FA0;
CActiveOccluder(&COcclusion::aActiveOccluders)[MAX_ACTIVE_OCCLUDERS] = *(CActiveOccluder(*)[MAX_ACTIVE_OCCLUDERS])0xC78610;
int& COcclusion::NumActiveOccluders = *(int*)0xC73CC0;

RwReal RwIm2DGetNearScreenZ(void)
{
	return (RWSRCGLOBAL(dOpenDevice).zBufferNear);
}

#include "CCollision.h"


void COcclusion__Render()
{
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, FALSE);

	float recipz = 1.0f / RwCameraGetNearClipPlane(Scene.m_pRwCamera);
	for(int i = 0; i < COcclusion::NumActiveOccluders; i++)
	{
		for(int j = 0; j < COcclusion::aActiveOccluders[i].m_cLinesCount; j++)
		{
			RwIm2DVertexSetScreenX(&vertexbufferT[0], COcclusion::aActiveOccluders[i].m_aLines[j].m_vecOrigin.x);
			RwIm2DVertexSetScreenY(&vertexbufferT[0], COcclusion::aActiveOccluders[i].m_aLines[j].m_vecOrigin.y);
			RwIm2DVertexSetScreenZ(&vertexbufferT[0], RwIm2DGetNearScreenZ());
			RwIm2DVertexSetCameraZ(&vertexbufferT[0], RwCameraGetNearClipPlane(Scene.camera));
			RwIm2DVertexSetRecipCameraZ(&vertexbufferT[0], recipz);

			RwIm2DVertexSetScreenX(&vertexbufferT[1], COcclusion::aActiveOccluders[i].m_aLines[j].m_vecOrigin.x + 
								   COcclusion::aActiveOccluders[i].m_aLines[j].m_vecDirection.x * COcclusion::aActiveOccluders[i].m_aLines[j].m_fLength);
			RwIm2DVertexSetScreenY(&vertexbufferT[1], COcclusion::aActiveOccluders[i].m_aLines[j].m_vecOrigin.y + 
								   COcclusion::aActiveOccluders[i].m_aLines[j].m_vecDirection.y * COcclusion::aActiveOccluders[i].m_aLines[j].m_fLength);

			RwIm2DVertexSetScreenZ(&vertexbufferT[1], RwIm2DGetNearScreenZ());
			RwIm2DVertexSetCameraZ(&vertexbufferT[1], RwCameraGetNearClipPlane(Scene.camera));
			RwIm2DVertexSetRecipCameraZ(&vertexbufferT[1], recipz);

			RwIm2DVertexSetIntRGBA(&vertexbufferT[0], 255, 255, 0, 255);
			RwIm2DVertexSetIntRGBA(&vertexbufferT[1], 255, 255, 0, 255);
			RwIm2DRenderLine(vertexbufferT, 2, 0, 1);
		}
	}

	DefinedState();
}

#include "GTADef.h"
#define MAXNUMBONES         64*4
RwUInt32             NumIm3DVerts = 0;
 RwIm3DVertex         Im3DVerts[MAXNUMBONES];
 RwImVertexIndex      Im3DIndices[MAXNUMBONES << 1];

 void CLines__RenderLineWithClipping(float x1, float y1, float z1, float x2, float y2, float z2, uint32_t c1, uint32_t c2)
 {
	 RwIm3DVertex         v[2];

	 RwRGBA col1;
	 col1.red = c1 >> 24;
	 col1.green = c1 >> 16;
	 col1.blue = c1 >> 8;
	 col1.alpha = c1;
	 RwRGBA col2;
	 col2.red = c2 >> 24;
	 col2.green = c2 >> 16;
	 col2.blue = c2 >> 8;
	 col2.alpha = c2;

	 RwIm3DVertexSetRGBA(&v[0], col1.red, col1.green, col1.blue, col1.alpha);
	 RwIm3DVertexSetRGBA(&v[1], col2.red, col2.green, col2.blue, col2.alpha);
	 RwIm3DVertexSetPos(&v[0], x1, y1, z1);
	 RwIm3DVertexSetPos(&v[1], x2, y2, z2);
	 if(RwIm3DTransform(v, 2, 0, 0))
	 {
		 RwIm3DRenderLine(0, 1);
		 RwIm3DEnd();
	 }
 }

 void SkeletonRender(RwMatrix* LTM, RpHAnimHierarchy* hier)
 {
	 if(hier)
	 {
		 static std::vector<RwIm3DVertex> vertices;
		 static std::vector<RwImVertexIndex> indices;

		 static bool generateVertices = true;

		 RwRGBA color = {255, 255, 0, 255};
		 if(generateVertices)
		 {
			 GenerateBoundingSphereVertices(5, 5, vertices, indices, &color);
			 generateVertices = false;
		 }

		 RwMatrix* matrices = RpHAnimHierarchyGetMatrixArray(hier);
		 RwV3d* rootPos;

		 RwUInt32 i, j;

		 for(i = 0, j = 0; i < MAXNUMBONES; i++)
		 {
			 Im3DIndices[j++] = 0;
			 Im3DIndices[j++] = (RwImVertexIndex)i + 1;
			 RwIm3DVertexSetRGBA(&Im3DVerts[i], color.red, color.green, color.blue, color.alpha);
		 }

		 for(size_t i = 0; i < hier->numNodes; i++)
		 {
			 rootPos = RwMatrixGetPos(&matrices[i]);
			 RwIm3DVertexSetPos(&Im3DVerts[0], rootPos->x, rootPos->y, rootPos->z);
			 NumIm3DVerts = 1;

			 if(hier->pNodeInfo[i].flags & rpHANIMPOPPARENTMATRIX)
			 {
				 RwIm3DVertexSetPos(&Im3DVerts[NumIm3DVerts], rootPos->x, rootPos->y, rootPos->z);
				 NumIm3DVerts++;
			 }
			 else
			 {
				 RwInt32     parentsOnStack = 0;
				 RwUInt32    currentIndex = i;

				 while(parentsOnStack > -1)
				 {
					 RwUInt32    flags;

					 currentIndex++;
					 flags = hier->pNodeInfo[currentIndex].flags;

					 if(parentsOnStack == 0)
					 {
						 RwV3d* pos = RwMatrixGetPos(matrices + currentIndex);

						 RwReal off = 0.015;
						 RwIm3DVertexSetPos(&Im3DVerts[NumIm3DVerts], pos->x, pos->y + off, pos->z);
						 NumIm3DVerts++;
						 RwIm3DVertexSetPos(&Im3DVerts[NumIm3DVerts], pos->x + off, pos->y, pos->z);
						 NumIm3DVerts++;
						 RwIm3DVertexSetPos(&Im3DVerts[NumIm3DVerts], pos->x, pos->y - off, pos->z);
						 NumIm3DVerts++;
						 RwIm3DVertexSetPos(&Im3DVerts[NumIm3DVerts], pos->x - off, pos->y, pos->z);
						 NumIm3DVerts++;

						 RwMatrix trans = *(matrices + currentIndex);
						 RwV3d scale = {0.02, 0.02, 0.02};
						 RwMatrixScale(&trans, &scale, rwCOMBINEPRECONCAT);

						 if(RwIm3DTransform(vertices.data(), vertices.size(), &trans, rwIM3D_ALLOPAQUE))
						 {
							 RwIm3DRenderIndexedPrimitive(rwPRIMTYPEPOLYLINE, indices.data(), indices.size());
							 RwIm3DEnd();
						 }

						 if(!(flags & rpHANIMPUSHPARENTMATRIX))
						 {
							 break;
						 }
					 }

					 if(flags & rpHANIMPUSHPARENTMATRIX)
					 {
						 parentsOnStack++;
					 }

					 if(flags & rpHANIMPOPPARENTMATRIX)
					 {
						 parentsOnStack--;
					 }
				 }
			 }

			 if(RwIm3DTransform(Im3DVerts, NumIm3DVerts, NULL, rwIM3D_ALLOPAQUE | rwIM3D_VERTEXXYZ))
			 {
				 RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
				 RwIm3DRenderIndexedPrimitive(rwPRIMTYPEPOLYLINE, Im3DIndices, (NumIm3DVerts * 2 - 1));
				 RwIm3DEnd();
				 RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
			 }
		 }
	 }
 }

#define RpHAnimGetHierarchy(frame) RpHAnimFrameGetHierarchy(frame)

RwRGBA Color = {255, 0, 0, 255};
RwFrame* HierarchyRender(RwFrame* frame,
				void* data __RWUNUSED__)
{

	 SkeletonRender(RwFrameGetLTM(frame), RpHAnimGetHierarchy(frame));

	return RwFrameForAllChildren(frame, HierarchyRender, NULL);

}

RpAtomic* AtomicRenderSkeleton(RpAtomic* atomic,
							   void* data __RWUNUSED__)
{
	RpGeometry* geometry;

	geometry = RpAtomicGetGeometry(atomic);

	if(geometry)
	{
		RwFrame* frame = RwFrameGetRoot(RpAtomicGetFrame(atomic));

		if(frame != NULL)
		{
			RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
			RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)FALSE);
			RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);

			RwFrameForAllChildren(frame, HierarchyRender, NULL);
		}
	}

	return atomic;
}

#include "CRenderer.h"
#include "CModelInfo.h"

#include "Mesh.h"
void EnvironmentMapping::SphericalMap()
{
	if(m_renderCallback == nullptr)
		return;

	RwCamera* camera = Scene.m_pRwCamera;
	RwCameraSetViewWindow(m_envCamera, &camera->viewWindow);
	RwFrameTransform(m_envFrame, &RwCameraGetFrame(camera)->ltm, rwCOMBINEREPLACE);


	RwV2d view;
	view.x = view.y = 0.4;
	RwCameraSetViewWindow(m_envCamera, &view);

	auto ped = FindPlayerPed();
	CVector pos = FindPlayerCoors(0);
	//RwFrameRotate(m_envFrame, &Yaxis, 90, rwCOMBINEREPLACE);
	//RwFrameRotate(m_envFrame, &Xaxis, 90, rwCOMBINEPOSTCONCAT);
	//RwFrameTranslate(m_envFrame, (RwV3d*)&pos, rwCOMBINEREPLACE);
	//RwFrameUpdateObjects(m_envFrame);

	//CameraTilt(m_envCamera, 0, 110.0);

	//CameraPan(m_envCamera, 0, (ped->m_fCurrentRotation* (180 / PI)));
	TranslateCameraZ(m_envCamera, -1600);


	RwFrameUpdateObjects(m_envFrame);


	std::vector<CMesh*>meshList;
	gRenderState = stageSphereMap;
	RwRGBA ambient = {CTimeCycle::m_CurrentColours.m_nSkyTopRed, CTimeCycle::m_CurrentColours.m_nSkyTopGreen, CTimeCycle::m_CurrentColours.m_nSkyTopBlue, 255};

	RwCameraSetRaster(m_envCamera, m_sphericalRaster);
	RwCameraClear(m_envCamera, &ambient, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
	RwCameraBeginUpdate(m_envCamera);
	ShaderContext->SetViewProjectionMatrix(4, true);
	m_renderCallback();
	// Renderer::RenderRoads();
	//Renderer::RenderEverythingBarRoads();
	DefinedState();

	//COcclusion__Render();

	for(int i = 0; i < CRenderer::ms_nNoOfVisibleEntities; ++i)
	{
		CEntity* entity = CRenderer::ms_aVisibleEntityPtrs[i];
		int id = entity->m_nModelIndex;

		CBaseModelInfo* modelInfo = CModelInfo::ms_modelInfoPtrs[id];
		CBoundingBox boudingbox = modelInfo->m_pColModel->m_boundBox;

		RwMatrix mat = *(RwMatrix*)entity->GetMatrix();


		RwRGBA SolidWhite = {255, 255, 255, 255};
		RwRGBA SolidBlack = {0,   0,   0, 255};
		RwRGBA SolidRed = {200,  64,  64, 255};
		RwRGBA SolidGreen = {64, 200,  64, 255};
		RwRGBA SolidBlue = {64,  64, 200, 255};
		RwRGBA SolidYellow = {200, 200,  64, 255};
		RwRGBA SolidPurple = {200,  64, 200, 255};
		RwRGBA SolidCyan = {64, 200, 200, 255};

		auto clump = entity->m_pRwClump;

		/*std::vector<RwRGBA> randoColor;

		randoColor.push_back(SolidWhite);
		randoColor.push_back(SolidBlack);
		randoColor.push_back(SolidRed);
		randoColor.push_back(SolidGreen);
		randoColor.push_back(SolidBlue);
		randoColor.push_back(SolidYellow);
		randoColor.push_back(SolidPurple);
		randoColor.push_back(SolidCyan);*/
		//if(entity->m_pRwObject->type == rpCLUMP && entity->m_nType == ENTITY_TYPE_PED)
		//RpClumpForAllAtomics(clump, AtomicRenderSkeleton, NULL);
		auto value = plugin::Random(0, 7);
		RwBBox bb;
		bb.inf = boudingbox.m_vecMin.ToRwV3d();
		bb.sup = boudingbox.m_vecMax.ToRwV3d();

		RwRGBA color;

		if(ObjectInFrustum[0][entity])
			color = SolidGreen;
		else
			color = SolidRed;
			
		//RenderBoundingBox(&bb, &mat, color);
			

	
		/*static std::vector<RwIm3DVertex> vertices;
		static std::vector<RwImVertexIndex> indices;

		static bool generateVertices = true;


		if(generateVertices)
		{
			GenerateBoundingSphereVertices(20, 20, vertices, indices, &color);
			generateVertices = false;
		}

		for(size_t j = 0; j < vertices.size(); j++)
		{
			RwIm3DVertexSetRGBA(&vertices[j],
								color.red, color.green,
								color.blue, color.alpha);
		}

		auto c = 0.5*(boudingbox.m_vecMax - boudingbox.m_vecMin);
		auto r = 0.5 * (boudingbox.m_vecMax - boudingbox.m_vecMin).Magnitude();

		
		RwV3d scale = {r, r, r};
		RwMatrixTranslate(&mat, &c.ToRwV3d(), rwCOMBINEPRECONCAT);
		RwMatrixScale(&mat, &scale, rwCOMBINEPRECONCAT);

		if(RwIm3DTransform(vertices.data(), vertices.size(), &mat, rwIM3D_ALLOPAQUE))
		{
			RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, indices.data(), indices.size());
			RwIm3DEnd();
		}*/

		auto colData = modelInfo->m_pColModel->m_pColData;
		if(colData == NULL)
			continue;

		//RenderBoudingSphereCollision(colData, mat, &color);
		//RenderBoundingBoxCollision(colData, mat, SolidCyan);
		//RenderTriangleCollision(colData, mat);
		//RenderLinesCollision(colData, mat);

	}

	Math::AABB box = CascadedShadowManagement->Desc[0].mFrustumCulling.GetBoundingBox();
	RwBBox bb;
	bb.inf = {box.Min.x,box.Min.y,box.Min.z};
	bb.sup = {box.Max.x,box.Max.y,box.Max.z};
	RwRGBA SolidBlue = {64,  64, 200, 64};
	RenderBoundingBox(&bb, 0, SolidBlue);

	SolidBlue.alpha = 64;
	RenderBoundingBox(&bb, 0, SolidBlue);

	RwCameraEndUpdate(m_envCamera);
}


void EnvironmentMapping::DualParaboloidMap()
{
	RwV2d view;
	view.x = view.y = 2.4;
	RwCameraSetViewWindow(m_envCamera, &view);

	RwV3d vAxisA = {0, 1, 0};
	RwV3d vAxisB = {1, 0, 0};
	CVector pos= FindPlayerCoors(0);

	gRenderState = stageDualParaboloidMap;

	// Calculate paraboloid frame
	RwFrameRotate(m_envFrame, &Yaxis, 180, rwCOMBINEREPLACE);
	RwFrameRotate(m_envFrame, &Xaxis, 90, rwCOMBINEPOSTCONCAT);
	RwFrameTranslate(m_envFrame, (RwV3d*)&pos, rwCOMBINEPOSTCONCAT);
	RwFrameUpdateObjects(m_envFrame);
	
	RwRGBA color = LerpSkyColor();

	// Render front mesh
	m_direction = 1.0f;
	RwCameraSetRaster(m_envCamera, m_paraboloidRaster[0]);
	RwCameraClear(m_envCamera, &color, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
	RwCameraBeginUpdate(m_envCamera);
	ShaderContext->SetViewProjectionMatrix(4, true);
	m_renderCallback();
	RwCameraEndUpdate(m_envCamera);

	// Render back mesh
	m_direction = -1.0;
	RwCameraSetRaster(m_envCamera, m_paraboloidRaster[1]);
	RwCameraClear(m_envCamera, &color, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
	RwCameraBeginUpdate(m_envCamera);
	ShaderContext->SetViewProjectionMatrix(4, true);
	m_renderCallback();
	RwCameraEndUpdate(m_envCamera);

	// Prepare paraboloid basis	
	RwD3D9GetTransform(D3DTS_VIEW, &m_paraboloidBasis);
}

std::vector<CEntity*> EnvironmentMapping::m_renderableList[6];

void EnvironmentMapping::AddObject(int i, CEntity* entity, float distance)
{
	if(distance > 400)
		return;

	if(distance < 100)
		m_renderableList[i].push_back(entity);
	else if(entity->m_pLod != NULL)
		m_renderableList[i].push_back(entity->m_pLod);
}

void EnvironmentMapping::SectorList(CPtrList& ptrList)
{
	int count[6] = {0,0,0,0,0};
	for(size_t i = 0; i < 6; i++)
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

				if(EnvironmentMapping::m_frustum[i].Intersects(aabb))
				{
					AddObject(i, entity, distance);
					count[i]++;
				}
			}
		}
	}
	PrintMessage("%i %i %i %i %i %i", count[0], count[1], count[2], count[3], count[4], count[5]);
}
 Math::Frustum EnvironmentMapping::m_frustum[6];
 RwMatrix EnvironmentMapping::m_ltm[6];
#include "RenderableReflectionObjects.h"
#include "Frustum.h"
#include "AABB.h"
  XMMATRIX EnvironmentMapping::m_projectionMatrix;
  XMMATRIX EnvironmentMapping::m_viewMatrix[6];

 void EnvironmentMapping::UpdateCubeMap()
 {
	 m_projectionMatrix = XMMatrixPerspectiveFovRH(tanf(3.14f / 4.0f), 1.0f, 0.01f, 3000.0f);
	 CVector pos = FindPlayerCoors(0);
	 for(int i = 0; i < 6; i++)
	 {
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
 }

 void EnvironmentMapping::CubeMap()
 {
	 //if(m_renderCallback == nullptr)
		// return;

	 //gRenderState = stageReflectionCubemap;
	 //RwRGBA ambient = {CTimeCycle::m_CurrentColours.m_nSkyTopRed, CTimeCycle::m_CurrentColours.m_nSkyTopGreen, CTimeCycle::m_CurrentColours.m_nSkyTopBlue, 255};

	 //for(int i = 0; i < 6; i++)
	 //{
		// _rwD3D9CubeRasterSelectFace(m_cubeRaster, i);
		// RwCameraSetRaster(m_envCamera, m_cubeRaster);
		// RwCameraClear(m_envCamera, &ambient, rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
		// RwCameraBeginUpdate(m_envCamera);

		// RwD3D9SetTransform(D3DTS_VIEW, &m_viewMatrix[i]);
		// RwD3D9SetTransform(D3DTS_PROJECTION, &m_projectionMatrix);

		// ShaderContext->SetViewProjectionMatrix(4, true);

		// auto renderList = RenderableReflectionObjects::GetRenderList();
		// for(auto& entity : renderList)
		// {
		//	 if(entity->m_pRwObject == nullptr)
		//		 continue;

		//	 entity->m_bImBeingRendered = true;
		//	 if(entity->m_pRwObject->type == rpATOMIC)
		//		 RpAtomicRender(entity->m_pRwAtomic);
		//	 else
		//		 RpClumpRender(entity->m_pRwClump);
		//	 entity->m_bImBeingRendered = false;

		// }
		// RwCameraEndUpdate(m_envCamera);
	 //}
 }
