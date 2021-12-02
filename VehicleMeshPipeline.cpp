#include "plugin.h"
#include "GTADef.h"
#include "DeferredRenderer.h"
#include "EnvironmentMapping.h"
#include "VehicleMeshPipeline.h"
#include "CubemapReflection.h"
#include "CommonD.h"
#include "CCamera.h"
#include "CTimeCycle.h"
using namespace plugin;

VehicleMeshPipeline* VehicleMeshPipe;

RxPipeline* CCustomCarEnvMapPipeline__CreateCustomOpenGLObjPipe()
{
	return VehicleMeshPipe->initGraphics();
}

void CCustomCarEnvMapPipeline__CustomPipeRenderCB(RwResEntry* repEntry, void* object, RwUInt8 type, RwUInt32 flags)
{
	VehicleMeshPipe->RenderCallBack(repEntry, object, type, flags);
}

RwBool CCustomCarEnvMapPipeline__CustomPipeReInstanceCB(void* object, RwResEntry* resEntry, RxD3D9AllInOneInstanceCallBack instanceCallback)
{
	if(!instanceCallback)
	{
		return true;
	}
	return instanceCallback(object, (RxD3D9ResEntryHeader*)(resEntry + 1), true);
}

RxPipeline* VehicleMeshPipeline::initGraphics()
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

			//instanceCB = RxD3D9AllInOneGetInstanceCallBack(node);
			//RxD3D9AllInOneSetInstanceCallBack(node, instanceCB);
			//reinstanceCB = RxD3D9AllInOneGetReinstanceCallBack(node);
			//RxD3D9AllInOneSetReinstanceCallBack(node, CCustomCarEnvMapPipeline__CustomPipeReInstanceCB);
			RxD3D9AllInOneSetRenderCallBack(node, CCustomCarEnvMapPipeline__CustomPipeRenderCB);

			deferredVS = RwCreateCompiledVertexShader("DeferredVehicleVS");
			deferredPS = RwCreateCompiledPixelShader("DeferredVehiclePS");
			VS_forward = RwCreateCompiledVertexShader("ForwardVehicleVS");
			PS_forward = RwCreateCompiledPixelShader("ForwardVehiclePS");
			createShaders();
			return pipeline;
		}
		_rxPipelineDestroy(pipeline);
	}
	return 0;
}

void VehicleMeshPipeline::hook()
{
	patch::RedirectJump(0x005D9F80, CCustomCarEnvMapPipeline__CreateCustomOpenGLObjPipe);
}

void VehicleMeshPipeline::ReflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{

	//CVehicleDrawable::DeferredRendering(entry, object, flags);
}

void VehicleMeshPipeline::DeferredRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{
	RxD3D9ResEntryHeader* header;
	RxD3D9InstanceData* instance;

	header = (RxD3D9ResEntryHeader*)(entry + 1);
	instance = (RxD3D9InstanceData*)(header + 1);

	MeshRenderingMode::DeferredRendering(entry, object, flags);

	RwMatrix* LTM = RwFrameGetLTM(RpAtomicGetFrame(object));
	XMMATRIX worldMatrix = RwMatrixToXMMATRIX(LTM);
	_rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);

	RwMatrix view;
	RwD3D9GetTransform(D3DTS_VIEW, &view);
	_rwD3D9SetPixelShaderConstant(4, &view, 4);

	_rwD3D9SetPixelShaderConstant(0, &GetSkyTopColor(), 1);
	CVector skyBottom = GetSkyBottomColor();
	_rwD3D9SetPixelShaderConstant(1, &D3DXVECTOR4(skyBottom.x, skyBottom.y, skyBottom.z, CTimeCycle::m_CurrentColours.m_fFarClip), 1);

	_rwD3D9SetVertexShader(deferredVS);
	_rwD3D9SetPixelShader(deferredPS);

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
			RwRGBARealFromRwRGBA(&colorValue, matcolor);

			UINT colorHEX = (*(UINT*)matcolor) & 0xFFFFFF;

			if(colorHEX < 0xAF00FF)
			{
				if(colorHEX < 0xFF3C || colorHEX == 0xFFB9 || colorHEX == 0xFF3C)
				{
					colorValue.blue = 0;
					colorValue.green = 0;
					colorValue.red = 0;
				}
			}

			if(colorHEX == 0xC8FF00 || colorHEX == 0xFF00FF || colorHEX == 0xFFFF00 || colorHEX == 0xAF00FF)
			{
				colorValue.blue = 0;
				colorValue.green = 0;
				colorValue.red = 0;
			}

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
			_rwD3D9SetPixelShaderConstant(2, &colorValue, 1);
			float materialProps[4];
			materialProps[0] = CCustomCarEnvMapPipeline__GetFxSpecSpecularity(material);
			materialProps[1] = 1.0f - RpMaterialGetFxEnvShininess(material);
			materialProps[2] = 0.0;
			materialProps[3] = 1.0;
			_rwD3D9SetPixelShaderConstant(3, materialProps, 1);

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

void VehicleMeshPipeline::ForwardRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{
	RxD3D9ResEntryHeader* header;
	RxD3D9InstanceData* instance;

	header = (RxD3D9ResEntryHeader*)(entry + 1);
	instance = (RxD3D9InstanceData*)(header + 1);

	MeshRenderingMode::ForwardRendering(entry, object, flags);

	RwMatrix* LTM = RwFrameGetLTM(RpAtomicGetFrame(object));
	XMMATRIX worldMatrix = RwMatrixToXMMATRIX(LTM);
	_rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);

	_rwD3D9SetPixelShaderConstant(8, &GetSkyTopColor(), 1);
	_rwD3D9SetPixelShaderConstant(9, &GetSkyBottomColor(), 1);
	_rwD3D9SetPixelShaderConstant(10, &GetSunColor(), 1);
	_rwD3D9SetPixelShaderConstant(11, &GetSunDirection(), 1);

	float fog[2];
	fog[0] = CTimeCycle::m_CurrentColours.m_fFogStart;
	fog[1] = CTimeCycle::m_CurrentColours.m_fFarClip;
	_rwD3D9SetPixelShaderConstant(12, fog, 1);
	_rwD3D9SetPixelShaderConstant(13, &TheCamera.GetPosition(), 1);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

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
			RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)40);
		else
			RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0);

		if(hasAlpha || instance->vertexAlpha || matcolor->alpha != 255)
		{
			RwRGBAReal colorValue = {1.0, 1.0, 1.0, 1.0};
			if(flags & rpGEOMETRYLIGHT)
			{
				if(flags & rpGEOMETRYMODULATEMATERIALCOLOR)
				{
					RwRGBARealFromRwRGBA(&colorValue, matcolor);

					UINT colorHEX = (*(UINT*)matcolor) & 0xFFFFFF;

					if(colorHEX < 0xAF00FF)
					{
						if(colorHEX < 0xFF3C || colorHEX == 0xFFB9 || colorHEX == 0xFF3C)
						{
							colorValue.blue = 0;
							colorValue.green = 0;
							colorValue.red = 0;
						}
					}

					if(colorHEX == 0xC8FF00 || colorHEX == 0xFF00FF || colorHEX == 0xFFFF00 || colorHEX == 0xAF00FF)
					{
						colorValue.blue = 0;
						colorValue.green = 0;
						colorValue.red = 0;
					}

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

			float materialProps[4];
			materialProps[0] = CCustomCarEnvMapPipeline__GetFxSpecSpecularity(material);
			materialProps[1] = 1.0f - RpMaterialGetFxEnvShininess(material);
			materialProps[2] = 0.0;
			materialProps[3] = 1.0;
			_rwD3D9SetPixelShaderConstant(15, materialProps, 1);

			BOOL hasTexture = texture ? TRUE : FALSE;
			RwD3D9SetPixelShaderConstantB(0, &hasTexture, 1);

			RwD3D9SetSamplerState(1, D3DSAMP_BORDERCOLOR, 0x00000000);
			RwD3D9SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			RwD3D9SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
			_rwD3D9RWSetRasterStage(EnvironmentMapping::m_sphericalRaster, 1);

			RwD3D9SetSamplerState(2, D3DSAMP_BORDERCOLOR, 0x00000000);
			RwD3D9SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			RwD3D9SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
			_rwD3D9RWSetRasterStage(EnvironmentMapping::m_paraboloidRaster[1], 2);

			RwD3D9SetSamplerState(3, D3DSAMP_BORDERCOLOR, 0x00000000);
			RwD3D9SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(3, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			RwD3D9SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			RwD3D9SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
			_rwD3D9RWSetRasterStage(CubemapReflection::m_cubeRaster, 3);

			D3D9Render(header, instance, texture, flags);
		}
		instance++;
	}
}