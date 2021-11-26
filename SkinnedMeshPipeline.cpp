#include "GTADef.h"
#include "DeferredRenderer.h"
#include "EnvironmentMapping.h"
#include "CommonD.h"
#include "CCamera.h"
#include "CTimeCycle.h"
#include "SkinnedMeshPipeline.h"
#include "SkinnedMeshInstance.h"
SkinnedMeshPipeline* SkinnedMeshPipe;

RxPipeline* _rpSkinD3D9CreatePlainPipe()
{
	return SkinnedMeshPipe->initGraphics();
}

void SkinnedMeshPipeline::hook()
{
	plugin::patch::RedirectJump(0x7CB2B0, _rpSkinD3D9CreatePlainPipe);
	plugin::patch::RedirectJump(0x7C7B90, _rwSkinD3D9AtomicAllInOneNode);
}

RxPipeline* SkinnedMeshPipeline::initGraphics()
{
	RxPipeline* pipeline;

	pipeline = RxPipelineCreate();
	if(pipeline)
	{
		RxLockedPipe* lockedpipe;
		lockedpipe = RxPipelineLock(pipeline);
		if(lockedpipe)
		{
			RxNodeDefinition* instanceNode;

			instanceNode = RxNodeDefinitionGetD3D9SkinAtomicAllInOne();
			auto lock = RxLockedPipeAddFragment(lockedpipe, NULL, instanceNode);

			lockedpipe->pluginId = 278;
			lockedpipe->pluginData = 1;

			RxLockedPipeUnlock(lock);
			VS_deferred = RwCreateCompiledVertexShader("SkinnedMeshVS");
			PS_deferred = RwCreateCompiledPixelShader("SkinnedMeshPS");
			VS_shadow = RwCreateCompiledVertexShader("SkinnedMeshShadowVS");
			PS_shadow = RwCreateCompiledPixelShader("SkinnedMeshShadowPS");
			VS_waterReflection = RwCreateCompiledVertexShader("WaterSkinnedMeshReflectionVS");
			PS_waterReflection = RwCreateCompiledPixelShader("WaterReflectionPS");
			return pipeline;
		}
		_rxPipelineDestroy(pipeline);
	}

	return NULL;
}


void _rpSkinMatrixUpdating(XMFLOAT4X3* boneMat, RpAtomic* atomic, RpSkin* skin)
{
	RwFrame* frame;
	RpHAnimHierarchy* hierarchy;
	RwInt32 numNodes;

	hierarchy = RpSkinAtomicGetHAnimHierarchy(atomic);

	frame = RpAtomicGetFrame(atomic);
	XMMATRIX LTM = RwMatrixToXMMATRIX(RwFrameGetLTM(frame));
	XMMATRIX inverseLTM = XMMatrixInverse(0, LTM);

	RpHAnimNodeInfo* frameInfo;

	/* Get the hierarchy's frame information. */
	frameInfo = hierarchy->pNodeInfo;
	for(RwInt32 i = 0; i < hierarchy->numNodes; i++)
	{
		XMMATRIX skinToBones = RwMatrixToXMMATRIX(&rpSkinGetSkinToBoneMatrices(skin)[i]);
		XMMATRIX matrixArray = RwMatrixToXMMATRIX(&hierarchy->pMatrixArray[i]);
		XMMATRIX temp;

		if(hierarchy->flags & rpHANIMHIERARCHYNOMATRICES)
		{
			LTM = RwMatrixToXMMATRIX(RwFrameGetLTM(frameInfo[i].pFrame));
			temp = skinToBones * LTM * inverseLTM;
		}
		else if(hierarchy->flags & rpHANIMHIERARCHYLOCALSPACEMATRICES)
		{
			temp = skinToBones * matrixArray;
		}
		else
		{
			temp = skinToBones * matrixArray * inverseLTM;
		}

		temp = XMMatrixTranspose(temp);
		XMStoreFloat4x4(reinterpret_cast<XMFLOAT4X4*>(&(reinterpret_cast<XMFLOAT4*>(boneMat))[i * 3]), temp);
	}
}

void SkinnedMeshPipeline::cascadeShadowRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{
	XMMATRIX  worldMatrix, viewMatrix, projectionMatrix;

	RwMatrix* LTM;
	RxD3D9ResEntryHeader* header;
	RxD3D9InstanceData* instance;

	RpSkin* skin;
	RpGeometry* geometry;
	RwMatrix* boneMatrices;
	RwUInt32  numBones;

	header = (RxD3D9ResEntryHeader*)(entry + 1);
	instance = (RxD3D9InstanceData*)(header + 1);

	LTM = RwFrameGetLTM(RpAtomicGetFrame(object));
	worldMatrix = RwMatrixToXMMATRIX(LTM);
	_rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);

	geometry = RpAtomicGetGeometry((RpAtomic*)object);
	skin = rpSkinGeometryGetSkin(geometry);
	numBones = rpSkinGetNumBones(skin);
	boneMatrices = rpSkinGetAlignedMatrixCache();

	XMFLOAT4X3 bones[64];
	_rpSkinMatrixUpdating(bones, (RpAtomic*)object, skin);
	_rwD3D9SetVertexShaderConstant(12, bones, sizeof(bones) / sizeof(XMVECTOR));

	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, FALSE);
	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);

	_rwD3D9SetVertexShader(VS_shadow);
	_rwD3D9SetPixelShader(PS_shadow);

	int numMeshes = header->numMeshes;
	while(numMeshes--)
	{
		RwRGBA* matcolor;
		RpMaterial* material;
		RwTexture* texture;
		bool hasAlpha;

		material = instance->material;
		matcolor = &material->color;
		texture = material->texture;

		RwRGBAReal colorValue;
		RwRGBARealFromRwRGBA(&colorValue, matcolor);
		_rwD3D9SetPixelShaderConstant(0, &colorValue, 1);

		hasAlpha = instance->vertexAlpha || matcolor->alpha != 255;
		//RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)hasAlpha);
		_rwD3D9SetPixelShaderConstant(1, &CTimeCycle::m_CurrentColours.m_fFarClip, 1);

		D3D9Render(header, instance, texture, flags);
		instance++;
	}
}

void SkinnedMeshPipeline::reflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{
	XMMATRIX				worldMatrix;
	RwMatrix* LTM;
	RxD3D9ResEntryHeader* header;
	RxD3D9InstanceData* instance;

	RpSkin* skin;
	RpGeometry* geometry;
	RwMatrix* boneMatrices;
	RwUInt32  numBones;

	header = (RxD3D9ResEntryHeader*)(entry + 1);
	instance = (RxD3D9InstanceData*)(header + 1);

	LTM = RwFrameGetLTM(RpAtomicGetFrame(object));
	worldMatrix = RwMatrixToXMMATRIX(LTM);
	_rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);

	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)2);
	if(gRenderState == RenderingStage::stageReflection)
		RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLFRONT);

	
		CVector playerPosition = FindPlayerCoors(0);
		float waterDept, waterLevel, waterGround;
		CWaterLevel__GetWaterDepth(playerPosition, &waterDept, &waterLevel, &waterGround);
		float params[2];
		params[0] = waterLevel;
		params[1] = gRenderState == RenderingStage::stageReflection;
		_rwD3D9SetVertexShaderConstant(12, params, 1);
		_rwD3D9SetPixelShaderConstant(1, params, 1);
	

	geometry = RpAtomicGetGeometry((RpAtomic*)object);
	skin = rpSkinGeometryGetSkin(geometry);
	numBones = rpSkinGetNumBones(skin);
	boneMatrices = rpSkinGetAlignedMatrixCache();
	XMFLOAT4X3 bones[64];
	_rpSkinMatrixUpdating(bones, (RpAtomic*)object, skin);
	_rwD3D9SetVertexShaderConstant(13, bones, sizeof(bones) / sizeof(XMVECTOR));
	_rwD3D9SetVertexShader(VS_waterReflection);
	_rwD3D9SetPixelShader(PS_waterReflection);
	int numMeshes = header->numMeshes;
	while(numMeshes--)
	{
		RwRGBA* matcolor;
		RpMaterial* material;
		RwTexture* texture;
		bool hasAlpha;

		material = instance->material;
		matcolor = &material->color;
		texture = material->texture;

		RwRGBAReal colorValue;
		RwRGBARealFromRwRGBA(&colorValue, matcolor);
		_rwD3D9SetPixelShaderConstant(0, &colorValue, 1);

		hasAlpha = instance->vertexAlpha || matcolor->alpha != 255;
		RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)hasAlpha);

		D3D9Render(header, instance, texture, flags);
		instance++;
	}
}

#include "CScene.h"
void SkinnedMeshPipeline::deferredRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{
	RxD3D9ResEntryHeader* header;
	RxD3D9InstanceData* instance;

	RpSkin* skin;
	RpGeometry* geometry;
	RwMatrix* boneMatrices;
	RwUInt32  numBones;
	RpAtomic* atomic = (RpAtomic*)object;
	header = (RxD3D9ResEntryHeader*)(entry + 1);
	instance = (RxD3D9InstanceData*)(header + 1);

	_rwD3D9SetVertexShader(VS_deferred);
	_rwD3D9SetPixelShader(PS_deferred);

	RwMatrix* LTM = RwFrameGetLTM(RpAtomicGetFrame(object));
	XMMATRIX worldMatrix = RwMatrixToXMMATRIX(LTM);
	_rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);

	geometry = RpAtomicGetGeometry((RpAtomic*)object);
	skin = rpSkinGeometryGetSkin(geometry);
	numBones = rpSkinGetNumBones(skin);
	boneMatrices = rpSkinGetAlignedMatrixCache();
	float numWeights = skin->maxNumWeightsForVertex;
	XMFLOAT4X3 bones[64];
	_rpSkinMatrixUpdating(bones, (RpAtomic*)object, skin);
	_rwD3D9SetVertexShaderConstant(12, bones, sizeof(bones) / sizeof(XMVECTOR));

	//float fog[2];
	//fog[0] = CTimeCycle::m_CurrentColours.m_fFogStart;
	//fog[1] = Scene.m_pRwCamera->farPlane;
	//_rwD3D9SetPixelShaderConstant(0, fog, 1);

	_rwD3D9SetPixelShaderConstant(0, &GetSkyTopColor(), 1);
	CVector skyBottom = GetSkyBottomColor();
	_rwD3D9SetPixelShaderConstant(1, &D3DXVECTOR4(skyBottom.x, skyBottom.y, skyBottom.z, CTimeCycle::m_CurrentColours.m_fFarClip), 1);

	int numMeshes = header->numMeshes;
	while(numMeshes--)
	{
		RwRGBA* matcolor;
		RpMaterial* material;
		RwTexture* texture;

		material = instance->material;
		matcolor = &material->color;
		texture = material->texture;
		bool hasAlpha;

		hasAlpha = instance->vertexAlpha || matcolor->alpha != 255;
		RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)hasAlpha);
		RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, 0);

		RwRGBAReal colorValue = {1.0, 1.0, 1.0, 1.0};

		float fSpec = max(CWeather__WetRoads, CCustomCarEnvMapPipeline__GetFxSpecSpecularity(material));
		float shininess = CCustomCarEnvMapPipeline__GetFxEnvShininess(material);

		RwV4d materialProps;
		materialProps.x = 0.01;
		materialProps.y = 0.1;
		materialProps.z = CCustomBuildingDNPipeline__m_fDNBalanceParam;
		materialProps.w = 1.0;

		if(material->surfaceProps.ambient > 1.0)
		{
			colorValue = {(float)matcolor->red / 255.0f * 16.0f,
						 (float)matcolor->green / 255.0f * 16.0f,
						 (float)matcolor->blue / 255.0f * 16.0f, (float)matcolor->alpha / 255.0f};
		}
		else
		{
			colorValue = {(float)matcolor->red / 255.0f,
						 (float)matcolor->green / 255.0f,
						 (float)matcolor->blue / 255.0f, (float)matcolor->alpha / 255.0f};
		}
		if(flags & rpGEOMETRYLIGHT)
		{
			if(flags & rpGEOMETRYMODULATEMATERIALCOLOR)
			{
			//	RwRGBARealFromRwRGBA(&colorValue, matcolor);
				_rwD3D9SetPixelShaderConstant(2, &colorValue, 1);
			}
			else
			{
				_rwD3D9SetPixelShaderConstant(2, &colorValue, 1);
			}
		}
		else
		{
			_rwD3D9SetPixelShaderConstant(2, &colorValue, 1);
		}

		_rwD3D9SetPixelShaderConstant(3, &materialProps, 1);

		bool ForceBumpMap = true;
		bool hasNormalMap = false;

		BOOL info[4];
		info[0] = texture ? TRUE : FALSE;
		info[1] = ForceBumpMap;
		info[2] = hasNormalMap;
		RwD3DDevice->SetPixelShaderConstantB(0, info, 4);

		D3D9Render(header, instance, texture, flags);

		instance++;
	}
}

void SkinnedMeshPipeline::forwardRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{}