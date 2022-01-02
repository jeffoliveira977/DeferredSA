#include "plugin.h"
#include "GTADef.h"
#include "DeferredRenderer.h"
#include "EnvironmentMapping.h"
#include "BuldingMeshPipeline.h"
#include "CommonD.h"
#include "CCamera.h"
#include "CTimeCycle.h"
#include "PBSMaterial.h"
#include "DualParaboloidReflection.h"
#include "CScene.h"
#include "ShaderManager.h"


using namespace plugin;

BuldingMeshPipeline* BuldingMeshPipe;

RxPipeline* CCustomBuildingDNPipeline__CreateCustomObjPipe()
{
	return BuldingMeshPipe->Initialize();
}

void CCustomBuildingDNPipeline__Render(RwResEntry* repEntry, void* object,
									   RwUInt8 type, RwUInt32 flags)
{
	BuldingMeshPipe->RenderCallBack(repEntry, object, type, flags);
}

void BuldingMeshPipeline::Hook()
{
	plugin::patch::RedirectJump(0x5D6750, CCustomBuildingDNPipeline__CreateCustomObjPipe);
}

 RwBool reinstance(void* object, RwResEntry* resEntry, RxD3D9AllInOneInstanceCallBack instanceCallback)
{
	return (instanceCallback == NULL ||
			instanceCallback(object, (RxD3D9ResEntryHeader*)(resEntry + 1), true) != 0);
}

RxPipeline* BuldingMeshPipeline::Initialize()
{
	RxPipeline* pipeline;

	pipeline = RxPipelineCreate();
	if(pipeline)
	{
		RxLockedPipe* lockedpipe;

		lockedpipe = RxPipelineLock(pipeline);
		if(lockedpipe)
		{
			RxPipelineNode* node;
			RxNodeDefinition* instanceNode;

			instanceNode = RxNodeDefinitionGetD3D9AtomicAllInOne();

			RxLockedPipeAddFragment(lockedpipe, NULL, instanceNode);

			pipeline->pluginId = 0x53F20098;
			pipeline->pluginData = 0x53F20098;

			RxLockedPipeUnlock(lockedpipe);

			node = RxPipelineFindNodeByName(pipeline, instanceNode->name, NULL, NULL);

			RxD3D9AllInOneInstanceCallBack instanceCB;
			RxD3D9AllInOneReinstanceCallBack reinstanceCB;

			instanceCB = RxD3D9AllInOneGetInstanceCallBack(node);
			RxD3D9AllInOneSetInstanceCallBack(node, instanceCB);
			reinstanceCB = RxD3D9AllInOneGetReinstanceCallBack(node);
			RxD3D9AllInOneSetReinstanceCallBack(node, reinstance);

			RxD3D9AllInOneSetRenderCallBack(node, CCustomBuildingDNPipeline__Render);
			
			mColorVertexShader = new VertexShader();
			mColorVertexShader->CreateFromBinary("ColorVS");

			mColorPixelShader = new PixelShader();
			mColorPixelShader->CreateFromBinary("ColorPS");

			VS_forward = new VertexShader();
			VS_forward->CreateFromBinary("ForwardBuldingVS");

			PS_forward = new PixelShader();
			PS_forward->CreateFromBinary("ForwardBuldingPS");

			VS_deferred = new VertexShader();
			VS_deferred->CreateFromBinary("DeferredBuldingVS");

			PS_deferred = new PixelShader();
			PS_deferred->CreateFromBinary("DeferredBuldingPS");

			VS_dualParaboloidMap = new VertexShader();
			VS_dualParaboloidMap->CreateFromBinary("DualParaboloidVS");

			PS_dualParaboloidMap = new PixelShader();
			PS_dualParaboloidMap->CreateFromBinary("DualParaboloidPS");

			VS_simple = new VertexShader();
			VS_simple->CreateFromBinary("SimpleVS");

			PS_simple = new PixelShader();
			PS_simple->CreateFromBinary("SimplePS");

			VS_sphereMap = new VertexShader();
			VS_sphereMap->CreateFromBinary("SphereMap");

			VS_waterReflection = new VertexShader();
			VS_waterReflection->CreateFromBinary("WaterReflectionVS");

			PS_waterReflection = new PixelShader();
			PS_waterReflection->CreateFromBinary("WaterReflectionPS");

			/*VS_forward           = RwCreateCompiledVertexShader("ForwardBuldingVS");
			PS_forward           = RwCreateCompiledPixelShader("ForwardBuldingPS");
			VS_deferred          = RwCreateCompiledVertexShader("DeferredBuldingVS");
			PS_deferred          = RwCreateCompiledPixelShader("DeferredBuldingPS");*/
		/*	VS_dualParaboloidMap = RwCreateCompiledVertexShader("DualParaboloidVS");
			PS_dualParaboloidMap = RwCreateCompiledPixelShader("DualParaboloidPS");*/
			//VS_simple            = RwCreateCompiledVertexShader("SimpleVS");
			//PS_simple            = RwCreateCompiledPixelShader("SimplePS");
			//VS_sphereMap         = RwCreateCompiledVertexShader("SphereMap");
			//VS_waterReflection = RwCreateCompiledVertexShader("WaterReflectionVS");
			//PS_waterReflection = RwCreateCompiledPixelShader("WaterReflectionPS");
			createShaders();

			return pipeline;
		}
		_rxPipelineDestroy(pipeline);
	}

	return NULL;
}

void BuldingMeshPipeline::ReflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{
	RxD3D9ResEntryHeader* header;
	RxD3D9InstanceData* instance;

	header = (RxD3D9ResEntryHeader*)(entry + 1);
	instance = (RxD3D9InstanceData*)(header + 1);

	if(gRenderState == RenderingStage::stageReflection)
		RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLFRONT);

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	if(gRenderState == RenderingStage::stageSphereMap)
	{
		float clipPlane[2];
		clipPlane[0] = CTimeCycle::m_CurrentColours.m_fFogStart;
		clipPlane[1] = CTimeCycle::m_CurrentColours.m_fFarClip;

		_rwD3D9SetVertexShaderConstant(12, clipPlane, 1);
		_rwD3D9SetVertexShaderConstant(13, &TheCamera.GetPosition(), 1);
		VS_sphereMap->Apply();
		PS_simple->Apply();
	}
	else if(gRenderState == RenderingStage::stageDualParaboloidMap)
	{
		float clipPlane[2];
		clipPlane[0] = 0.1;
		clipPlane[1] = 300.0;
		_rwD3D9SetVertexShaderConstant(12, clipPlane, 1);
		VS_dualParaboloidMap->Apply();
		PS_dualParaboloidMap->Apply();
	}
	else if(gRenderState == RenderingStage::stageReflection)
	{
		CVector playerPosition = FindPlayerCoors(0);
		float waterDept, waterLevel, waterGround;
		CWaterLevel__GetWaterDepth(playerPosition, &waterDept, &waterLevel, &waterGround);
		float params[2];
		params[0] = waterLevel;
		params[1] = 1.0;
		_rwD3D9SetVertexShaderConstant(12, &params, 1);
		_rwD3D9SetPixelShaderConstant(1, &waterLevel, 1);
		VS_waterReflection->Apply();
		PS_waterReflection->Apply();
	}
	else
	{
		VS_simple->Apply();
		PS_simple->Apply();
	}

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

		BOOL hasTexture = texture ? TRUE : FALSE;
		RwD3D9SetPixelShaderConstantB(0, &hasTexture, 1);

		D3D9Render(header, instance, texture, flags);
		instance++;
	}
}

#include "CWeather.h"
#include "CascadedShadowRendering.h"
#include "CGameIdle.h"

void BuldingMeshPipeline::DeferredRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{
	RxD3D9ResEntryHeader* header;
	RxD3D9InstanceData* instance;

	header = (RxD3D9ResEntryHeader*)(entry + 1);
	instance = (RxD3D9InstanceData*)(header + 1);

	VS_deferred->Apply();
	PS_deferred->Apply();

	MeshRenderingMode::DeferredRendering(entry, object, flags);

	int numMeshes = header->numMeshes;
	while(numMeshes--)
	{
		RwRGBA* matcolor;
		RpMaterial* material;
		RwTexture* texture;

		material = instance->material;
		matcolor = &material->color;
		texture = material->texture;

		bool alphaMesh = false;
		if (instance->vertexAlpha ||
			matcolor->alpha != 0xFF ||
			(texture && RwD3D9TextureHasAlpha(texture)))
		{
			alphaMesh = true;
		}

		if(alphaMesh == false)
		{
			float fSpec = max(CWeather::WetRoads, CCustomCarEnvMapPipeline__GetFxSpecSpecularity(material));

			float fGlossiness = RpMaterialGetFxEnvShininess(material);
			RwV4d materialProps;
			materialProps.x = fSpec;
			materialProps.y = fGlossiness;
			materialProps.z = 1.0f - (CGame::currArea == 0 ? CGameIdle::m_fShadowDNBalance : 1.0f);
			materialProps.w = 2.2;
			_rwD3D9SetPixelShaderConstant(2, &materialProps, 1);

			RwRGBAReal colorValue = { 1.0, 1.0, 1.0, 1.0 };
			if((flags & rpGEOMETRYLIGHT) && 
				(flags & rpGEOMETRYMODULATEMATERIALCOLOR))
			{
				if (material->surfaceProps.ambient > 1.0)
				{
					colorValue = {(float)matcolor->red / 255.0f * 16.0f,
								 (float)matcolor->green / 255.0f * 16.0f,
								 (float)matcolor->blue / 255.0f * 16.0f, (float)matcolor->alpha / 255.0f };
				}
				else
				{
					colorValue = {(float)matcolor->red / 255.0f,
								 (float)matcolor->green / 255.0f,
								 (float)matcolor->blue / 255.0f, (float)matcolor->alpha / 255.0f };
				}
				_rwD3D9SetPixelShaderConstant(1, &colorValue, 1);
			}
			else
			{
				_rwD3D9SetPixelShaderConstant(1, &colorValue, 1);
			}
			
			bool ForceBumpMap = false;
			bool hasNormalMap = false;
			bool hasSpecularMap = false;

			if(texture && texture->raster)
			{
				PBSMaterial* mat = PBSMaterialMgr::materials[texture->name];
				if(mat != nullptr)
				{
					if(mat->m_tSpecRoughness)
					{
						RwD3D9SetTexture(mat->m_tSpecRoughness, 1);
						hasSpecularMap = true;
					}
					if(mat->m_tNormals)
					{
						RwD3D9SetTexture(mat->m_tNormals, 2);
						hasNormalMap = true;
					}
				}
			}

			BOOL info[4];
			info[0] = texture ? TRUE : FALSE;
			info[1] = ForceBumpMap;
			info[2] = hasNormalMap;
			info[3] = hasSpecularMap;

			RwD3DDevice->SetPixelShaderConstantB(0, info, sizeof(info)/4);
			D3D9Render(header, instance, texture, flags);
		}
		instance++;
	}
}

void BuldingMeshPipeline::ForwardRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{
	RxD3D9ResEntryHeader* header;
	RxD3D9InstanceData* instance;

	header = (RxD3D9ResEntryHeader*)(entry + 1);
	instance = (RxD3D9InstanceData*)(header + 1);

	ShaderContext->SetTimecyProps(8);
	ShaderContext->SetInverseViewMatrix(4);

	_rwD3D9SetPixelShaderConstant(13, &TheCamera.GetPosition(), 1);

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	for(size_t i = 0; i < CascadedShadowManagement->CascadeCount; i++)
	{
		rwD3D9SetSamplerState(i + 4, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i + 4, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i + 4, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i + 4, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		rwD3D9SetSamplerState(i + 4, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		rwD3D9SetSamplerState(i + 4, D3DSAMP_BORDERCOLOR, 0xFFFFFFFF);

		rwD3D9RWSetRasterStage(CascadedShadowManagement->mColorRaster[i], i + 4);
	}

	_rwD3D9SetPixelShaderConstant(16, &CascadedShadowManagement->mConstantBuffer,
								  sizeof(CascadedShadowManagement->mConstantBuffer) / sizeof(XMVECTOR));

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

		if(texture)
		{
			VS_forward->Apply();
			PS_forward->Apply();
		}
		else
		{
			mColorVertexShader->Apply();
			mColorPixelShader->Apply();
		}

		hasAlpha = material->texture && RwD3D9TextureHasAlpha(material->texture);

		if(hasAlpha)
			RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)100);
		else
			RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)10);

		if(hasAlpha || instance->vertexAlpha || matcolor->alpha != 255)
		{

			float fSpec = max(CWeather::WetRoads,
				CCustomCarEnvMapPipeline__GetFxSpecSpecularity(material));

			float fGlossiness = RpMaterialGetFxEnvShininess(material);

			RwV4d materialProps;
			materialProps.x = fSpec;
			materialProps.y = fGlossiness;
			materialProps.z = 1.0f - (CGame::currArea == 0 ? CGameIdle::m_fShadowDNBalance : 1.0f);
			materialProps.w = 2.2;
			_rwD3D9SetPixelShaderConstant(15, &materialProps, 1);

			RwRGBAReal colorValue = { 1.0, 1.0, 1.0, 1.0 };
			if ((flags & rpGEOMETRYLIGHT) &&
				(flags & rpGEOMETRYMODULATEMATERIALCOLOR))
			{
				if (material->surfaceProps.ambient > 1.0)
				{
					colorValue = { (float)matcolor->red / 255.0f * 16.0f,
								 (float)matcolor->green / 255.0f * 16.0f,
								 (float)matcolor->blue / 255.0f * 16.0f, (float)matcolor->alpha / 255.0f };
				}
				else
				{
					colorValue = { (float)matcolor->red / 255.0f,
								 (float)matcolor->green / 255.0f,
								 (float)matcolor->blue / 255.0f, (float)matcolor->alpha / 255.0f };
				}
				_rwD3D9SetPixelShaderConstant(14, &colorValue, 1);
			}
			else
			{
				_rwD3D9SetPixelShaderConstant(14, &colorValue, 1);
			}

			bool ForceBumpMap = false;
			bool hasNormalMap = false;
			bool hasSpecularMap = false;

			if (texture && texture->raster)
			{
				PBSMaterial* mat = PBSMaterialMgr::materials[texture->name];
				if (mat != nullptr)
				{
					if (mat->m_tSpecRoughness)
					{
						RwD3D9SetTexture(mat->m_tSpecRoughness, 1);
						hasSpecularMap = true;
					}
					if (mat->m_tNormals)
					{
						RwD3D9SetTexture(mat->m_tNormals, 2);
						hasNormalMap = true;
					}
				}
			}

			BOOL info[4];
			info[0] = texture ? TRUE : FALSE;
			info[1] = ForceBumpMap;
			info[2] = hasNormalMap;
			info[3] = hasSpecularMap;

			RwD3DDevice->SetPixelShaderConstantB(0, info, 4);

			D3D9Render(header, instance, texture, flags);
		}
		instance++;
	}
}