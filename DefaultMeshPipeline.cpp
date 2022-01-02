#include "plugin.h"
#include "GTADef.h"
#include "DeferredRenderer.h"
#include "DefaultMeshPipeline.h"
#include "CommonD.h"
#include "CScene.h"

using namespace plugin;
DefaultMeshPipeline* DefaultMeshPipe;

#include "DefaultPipeline.h"

RwBool _rpCreatePlatformAtomicPipelines()
{
	return DefaultMeshPipe->Initialize();
}

void DefaultMeshPipeline::Hook()
{
	patch::RedirectJump(0x74EB80, _rpCreatePlatformAtomicPipelines);
	patch::RedirectJump(0x757890, D3D9AtomicAllInOnePipelineInit);
}

RwBool DefaultMeshPipeline::Initialize()
{
	RxPipeline* pipe;

	pipe = RxPipelineCreate();
	if(pipe)
	{
		RxLockedPipe* lpipe;

		pipe->pluginId = rwID_WORLDPLUGIN;
		if(NULL != (lpipe = RxPipelineLock(pipe)))
		{
			lpipe = RxLockedPipeAddFragment(lpipe, NULL, RxNodeDefinitionGetD3D9AtomicAllInOne());
			lpipe = RxLockedPipeUnlock(lpipe);

			if(NULL != lpipe)
			{
				VS_simple = new VertexShader();
				VS_simple->CreateFromBinary("SimpleVS");

				PS_simple = new PixelShader();
				PS_simple->CreateFromBinary("SimplePS");

				VS_forward = new VertexShader();
				VS_forward->CreateFromBinary("ForwardBuldingVS");

				PS_forward = new PixelShader();
				PS_forward->CreateFromBinary("ForwardBuldingPS");

				VS_deferred = new VertexShader();
				VS_deferred->CreateFromBinary("DeferredBuldingVS");

				PS_deferred = new PixelShader();
				PS_deferred->CreateFromBinary("DeferredBuldingPS");
				createShaders();

				RXPIPELINEGLOBAL(platformAtomicPipeline) = pipe;
				RpAtomicSetDefaultPipeline(pipe);

				_rpD3D9VertexShaderCacheOpen();

				return TRUE;			
			}
		}

		RxPipelineDestroy(pipe);
	}

	return FALSE;
}

void DefaultMeshPipeline::ReflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags)
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

	/*if(gRenderState == RenderingStage::stageSphereMap)
	{
		RwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);
		RwD3D9SetVertexShaderConstant(4, &TheCamera->GetPosition(), 1);

		float fog[2];
		fog[0] = Timecycle->m_fCurrentFogStart;
		fog[1] = Timecycle->m_fCurrentFarClip;
		RwD3D9SetVertexShaderConstant(5, fog, 1);
		rwD3D9SetVertexShader(VS_sphereMap);
		rwD3D9SetPixelShader(PS_simple);
	}
	else if(gRenderState == RenderingStage::stageDualParaboloidMap)
	{
		float fog[3];
		fog[0] = 0.1;
		fog[1] = 310.0;
		fog[2] = EnvironmentMapping::m_direction;
		RwD3D9SetVertexShaderConstant(4, fog, 1);

		rwD3D9SetVertexShader(VS_dualParaboloidMap);
		rwD3D9SetPixelShader(PS_dualParaboloidMap);
	}
	else*/
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
#include "CommonD.h"
#include "CCamera.h"
#include "CTimeCycle.h"
#include "PBSMaterial.h"
#include "EnvironmentMapping.h"

void DefaultMeshPipeline::DeferredRendering(RwResEntry* entry, void* object, RwUInt32 flags)
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

		if((instance->vertexAlpha ||
			matcolor->alpha != 0xFF ||
			(texture && RwD3D9TextureHasAlpha(texture))) == false)
		{
			RwRGBAReal colorValue = {1.0f, 1.0f, 1.0f, 1.0f};
			float fSpec = max(CWeather::WetRoads,
							  CCustomCarEnvMapPipeline__GetFxSpecSpecularity(
								  material));
			float fGlossiness = RpMaterialGetFxEnvShininess(material);
			RwV4d materialProps;
			materialProps.x = fSpec;
			materialProps.y = fGlossiness;
			materialProps.z = 1.0f - (CGame::currArea == 0 ? CGameIdle::m_fShadowDNBalance : 1.0f);
			materialProps.w = 2.2f;
			_rwD3D9SetPixelShaderConstant(2, &materialProps, 1);

			if(material->surfaceProps.ambient > 1.0f)
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

			RwD3DDevice->SetPixelShaderConstantB(0, info, sizeof(info) / 4);
			D3D9Render(header, instance, texture, flags);
		}
		instance++;
	}
}

#include "ShaderManager.h"
void DefaultMeshPipeline::ForwardRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{

	RxD3D9ResEntryHeader* header;
	RxD3D9InstanceData* instance;
	
	header = (RxD3D9ResEntryHeader*)(entry + 1);
	instance = (RxD3D9InstanceData*)(header + 1);

	RwMatrix* LTM = RwFrameGetLTM(RpAtomicGetFrame(object));
	XMMATRIX worldMatrix = RwMatrixToXMMATRIX(LTM);
	_rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);
	ShaderContext->SetViewProjectionMatrix(4, true);
	_rwD3D9SetVertexShaderConstant(12, &EnvironmentMapping::m_paraboloidBasis, 4);

	_rwD3D9SetPixelShaderConstant(8, &GetSkyTopColor(), 1);
	_rwD3D9SetPixelShaderConstant(9, &GetSkyBottomColor(), 1);
	_rwD3D9SetPixelShaderConstant(10, &GetSunColor(), 1);
	_rwD3D9SetPixelShaderConstant(11, &GetSunDirection(), 1);

	float fog[3];
	fog[0] = CTimeCycle::m_CurrentColours.m_fFogStart;
	fog[1] = Scene.m_pRwCamera->farPlane;
	fog[2] = Scene.m_pRwCamera->farPlane - CTimeCycle::m_CurrentColours.m_fFogStart;
	_rwD3D9SetPixelShaderConstant(12, fog, 1);
	_rwD3D9SetPixelShaderConstant(13, &TheCamera.GetPosition(), 1);
	
	VS_forward->Apply();
	PS_forward->Apply();

	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSWRAP);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)5);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)6);
	

	for(size_t i = 0; i < (size_t)CascadedShadowManagement->CascadeCount; i++)
	{
		rwD3D9SetSamplerState(i + 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i + 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i + 2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		rwD3D9SetSamplerState(i + 2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		rwD3D9SetSamplerState(i + 2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		rwD3D9SetSamplerState(i + 2, D3DSAMP_BORDERCOLOR, 0xFFFFFFFF);

		rwD3D9RWSetRasterStage(CascadedShadowManagement->mColorRaster[i], i + 2);
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

		hasAlpha = material->texture && RwD3D9TextureHasAlpha(material->texture);

		if(hasAlpha)
			RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)50);
		else
			RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0);
		RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)hasAlpha);
		if(hasAlpha || instance->vertexAlpha || matcolor->alpha != 255)
		{
			RwRGBAReal colorValue = {1.0f, 1.0f, 1.0f, 1.0f};

			float fSpec = max(CWeather::WetRoads,
							  CCustomCarEnvMapPipeline__GetFxSpecSpecularity(
								  material));
			float fGlossiness = RpMaterialGetFxEnvShininess(material);

			RwV4d materialProps;
			materialProps.x = fSpec;
			materialProps.y = fGlossiness;
			materialProps.z = 1.0f - (CGame::currArea == 0 ? CGameIdle::m_fShadowDNBalance : 1.0f);
			materialProps.w = 2.2f;

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