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

using namespace plugin;

BuldingMeshPipeline* BuldingMeshPipe;


RxPipeline* CCustomBuildingDNPipeline__CreateCustomObjPipe()
{
	return BuldingMeshPipe->initGraphics();
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

static RwBool
reinstance(void* object, RwResEntry* resEntry, RxD3D9AllInOneInstanceCallBack instanceCallback)
{
	return (instanceCallback == NULL ||
			instanceCallback(object, (RxD3D9ResEntryHeader*)(resEntry + 1), true) != 0);
}

RxPipeline* BuldingMeshPipeline::initGraphics()
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
			

			mColorVertexShader = RwCreateCompiledVertexShader("ColorVS");
			mColorPixelShader = RwCreateCompiledPixelShader("ColorPS");

			VS_forward           = RwCreateCompiledVertexShader("ForwardBuldingVS");
			PS_forward           = RwCreateCompiledPixelShader("ForwardBuldingPS");
			VS_deferred          = RwCreateCompiledVertexShader("DeferredBuldingVS");
			PS_deferred          = RwCreateCompiledPixelShader("DeferredBuldingPS");
			VS_dualParaboloidMap = RwCreateCompiledVertexShader("DualParaboloidVS");
			PS_dualParaboloidMap = RwCreateCompiledPixelShader("DualParaboloidPS");
			VS_simple            = RwCreateCompiledVertexShader("SimpleVS");
			PS_simple            = RwCreateCompiledPixelShader("SimplePS");
			VS_sphereMap         = RwCreateCompiledVertexShader("SphereMap");
			VS_waterReflection = RwCreateCompiledVertexShader("WaterReflectionVS");
			PS_waterReflection = RwCreateCompiledPixelShader("WaterReflectionPS");
			createShaders();

			return pipeline;
		}
		_rxPipelineDestroy(pipeline);
	}

	return NULL;
}

void BuldingMeshPipeline::ReflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{
	XMMATRIX				worldMatrix;
	RwMatrix* LTM;
	RxD3D9ResEntryHeader* header;
	RxD3D9InstanceData* instance;

	header = (RxD3D9ResEntryHeader*)(entry + 1);
	instance = (RxD3D9InstanceData*)(header + 1);

	LTM = RwFrameGetLTM(RpAtomicGetFrame(object));
	worldMatrix = RwMatrixToXMMATRIX(LTM);
	_rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);

	if(gRenderState == RenderingStage::stageReflection)
		RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLFRONT);

	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	if(gRenderState == RenderingStage::stageSphereMap)
	{
		float fog[2];
		fog[0] = CTimeCycle::m_CurrentColours.m_fFogStart;
		fog[1] = CTimeCycle::m_CurrentColours.m_fFarClip;
		_rwD3D9SetVertexShaderConstant(12, fog, 1);
		_rwD3D9SetVertexShaderConstant(13, &TheCamera.GetPosition(), 1);

		_rwD3D9SetVertexShader(VS_sphereMap);
		_rwD3D9SetPixelShader(PS_simple);
	}
	else if(gRenderState == RenderingStage::stageDualParaboloidMap)
	{
		float fog[4];
		fog[0] = 0.1;
		fog[1] = 300.0;
		_rwD3D9SetVertexShaderConstant(12, fog, 1);

		_rwD3D9SetVertexShader(VS_dualParaboloidMap);
		_rwD3D9SetPixelShader(PS_dualParaboloidMap);
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
		_rwD3D9SetVertexShader(VS_waterReflection);
		_rwD3D9SetPixelShader(PS_waterReflection);
	}
	else
	{
		_rwD3D9SetVertexShader(VS_simple);
		_rwD3D9SetPixelShader(PS_simple);
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

	_rwD3D9SetVertexShader(VS_deferred);
	_rwD3D9SetPixelShader(PS_deferred);

	MeshRenderingMode::DeferredRendering(entry, object, flags);

	RwMatrix* LTM= RwFrameGetLTM(RpAtomicGetFrame(object));
	XMMATRIX worldMatrix = RwMatrixToXMMATRIX(LTM);
	_rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);
	
	RwMatrix view;
	RwD3D9GetTransform(D3DTS_VIEW, &view);
	_rwD3D9SetPixelShaderConstant(4, &view, 4);

	int numMeshes = header->numMeshes;
	while(numMeshes--)
	{
		RwRGBA* matcolor;
		RpMaterial* material;
		RwTexture* texture;

		material = instance->material;
		matcolor = &material->color;
		texture = material->texture;

		if((instance->vertexAlpha ||
			matcolor->alpha != 0xFF ||
			(texture && RwD3D9TextureHasAlpha(texture))) == false)
		{
			RwRGBAReal colorValue = {1.0, 1.0, 1.0, 1.0};
			float fSpec = max(CWeather::WetRoads,
							  CCustomCarEnvMapPipeline__GetFxSpecSpecularity(
								  material));
			float fGlossiness = RpMaterialGetFxEnvShininess(material);
			RwV4d materialProps;
			materialProps.x = fSpec;
			materialProps.y = fGlossiness;
			materialProps.z = 1.0f - (CGame::currArea == 0 ? CGameIdle::m_fShadowDNBalance : 1.0f);
			materialProps.w = 2.2;

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
					//RwRGBARealFromRwRGBA(&colorValue, matcolor);
					
					_rwD3D9SetPixelShaderConstant(1, &colorValue, 1);
				}
				else
				{
					_rwD3D9SetPixelShaderConstant(1, &colorValue, 1);
				}
			}
			else
			{
				_rwD3D9SetPixelShaderConstant(1, &colorValue, 1);
			}
			_rwD3D9SetPixelShaderConstant(2, &materialProps, 1);
			
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

	RwMatrix* LTM = RwFrameGetLTM(RpAtomicGetFrame(object));
	XMMATRIX worldMatrix = RwMatrixToXMMATRIX(LTM);
	_rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);
	_rwD3D9SetVertexShaderConstant(12, &EnvironmentMapping::m_paraboloidBasis, 4);

	_rwD3D9SetPixelShaderConstant(8, &GetSkyTopColor(), 1);
	_rwD3D9SetPixelShaderConstant(9, &GetSkyBottomColor(), 1);
	_rwD3D9SetPixelShaderConstant(10, &GetSunColor(), 1);
	_rwD3D9SetPixelShaderConstant(11, &GetSunDirection(), 1);

	float fog[3];
	fog[0] = CTimeCycle::m_CurrentColours.m_fFogStart;
	fog[1] = CTimeCycle::m_CurrentColours.m_fFarClip;
	fog[2] = CTimeCycle::m_CurrentColours.m_fFarClip - CTimeCycle::m_CurrentColours.m_fFogStart;
	_rwD3D9SetPixelShaderConstant(12, fog, 1);
	_rwD3D9SetPixelShaderConstant(13, &TheCamera.GetPosition(), 1);


	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	for(size_t i = 0; i < CascadedShadowManagement->CascadeCount; i++)
	{
		rwD3D9SetSamplerState(i + 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i + 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i + 2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i + 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		rwD3D9SetSamplerState(i + 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		rwD3D9SetSamplerState(i + 2, D3DSAMP_BORDERCOLOR, 0xFFFFFFFF);

		rwD3D9RWSetRasterStage(CascadedShadowManagement->m_shadowColorRaster[i], i + 2);
	}

	_rwD3D9SetPixelShaderConstant(16, &CascadedShadowManagement->m_shadowBuffer,
								  sizeof(CascadedShadowManagement->m_shadowBuffer) / sizeof(XMVECTOR));
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
			_rwD3D9SetVertexShader(VS_forward);
			_rwD3D9SetPixelShader(PS_forward);
		}
		else
		{
			_rwD3D9SetVertexShader(mColorVertexShader);
			_rwD3D9SetPixelShader(mColorPixelShader);
		}

		hasAlpha = material->texture && RwD3D9TextureHasAlpha(material->texture);

		if(hasAlpha)
			RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)10);
		else
			RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0);

		if(hasAlpha || instance->vertexAlpha || matcolor->alpha != 255)
		{
			RwRGBAReal colorValue = {1.0, 1.0, 1.0, 1.0};

			float fSpec = max(CWeather::WetRoads,
							  CCustomCarEnvMapPipeline__GetFxSpecSpecularity(
								  material));
			float fGlossiness = RpMaterialGetFxEnvShininess(material);

			RwV4d materialProps;
			materialProps.x = fSpec;
			materialProps.y = fGlossiness;
			materialProps.z = 1.0f - (CGame::currArea == 0 ? CGameIdle::m_fShadowDNBalance : 1.0f);
			materialProps.w = 2.2;

			if(flags & rpGEOMETRYLIGHT)
			{
				if(flags & rpGEOMETRYMODULATEMATERIALCOLOR)
				{
					RwRGBARealFromRwRGBA(&colorValue, matcolor);
					_rwD3D9SetPixelShaderConstant(14, &colorValue, 1);
				}
				else
				{
					_rwD3D9SetPixelShaderConstant(14, &colorValue, 1);
				}
			}
			else
			{
				_rwD3D9SetPixelShaderConstant(14, &colorValue, 1);
			}
			_rwD3D9SetPixelShaderConstant(15, &materialProps, 1);
	/*		RwD3D9SetSamplerState(2, D3DSAMP_BORDERCOLOR, 0x00000000);
			RwD3D9SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			RwD3D9SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
			rwD3D9RWSetRasterStage(EnvironmentMapping::m_paraboloidRaster[0], 2);*/
			bool ForceBumpMap = true;
			bool hasNormalMap = false;

			BOOL info[4];
			info[0] = texture ? TRUE : FALSE;
			info[1] = ForceBumpMap;
			info[2] = hasNormalMap;
			RwD3DDevice->SetPixelShaderConstantB(0, info, 4);

			D3D9Render(header, instance, texture, flags);
		}
		instance++;
	}
}