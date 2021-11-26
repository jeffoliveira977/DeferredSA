#include "plugin.h"
#include "GTADef.h"
#include "DeferredRenderer.h"
#include "DefaultMeshPipeline.h"
#include "CommonD.h"

using namespace plugin;
DefaultMeshPipeline* DefaultMeshPipe;

#include "DefaultPipeline.h"

RwBool _rpCreatePlatformAtomicPipelines()
{
	return DefaultMeshPipe->initGraphics();
}

void DefaultMeshPipeline::hook()
{
	patch::RedirectJump(0x74EB80, _rpCreatePlatformAtomicPipelines);
	patch::RedirectJump(0x757890, D3D9AtomicAllInOnePipelineInit);
}

RwBool DefaultMeshPipeline::initGraphics()
{
	RxPipeline* pipe;

	pipe = RxPipelineCreate();
	if(pipe)
	{
		RxLockedPipe* lpipe;

		pipe->pluginId = rwID_WORLDPLUGIN;
		if(NULL != (lpipe = RxPipelineLock(pipe)))
		{
			lpipe = RxLockedPipeAddFragment(lpipe, NULL, RxNodeDefinitionGetD3D9AtomicAllInOne(), NULL);
			lpipe = RxLockedPipeUnlock(lpipe);

			if(NULL != lpipe)
			{
				VS_deferred = RwCreateCompiledVertexShader("DefaultMeshVS");
				PS_deferred = RwCreateCompiledPixelShader("DefaultMeshPS");
				VS_simple = RwCreateCompiledVertexShader("SimpleVS");
				PS_simple = RwCreateCompiledPixelShader("SimplePS");
				createShaders();

				RXPIPELINEGLOBAL(platformAtomicPipeline) = pipe;
				RpAtomicSetDefaultPipeline(pipe);

				_rpD3D9VertexShaderCacheOpen();

				return TRUE;			
			}
		}

		RxPipelineDestroy(pipe);
	}
}

void DefaultMeshPipeline::reflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags)
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

void DefaultMeshPipeline::deferredRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{
	RxD3D9ResEntryHeader* header;
	RxD3D9InstanceData* instance;

	header = (RxD3D9ResEntryHeader*)(entry + 1);
	instance = (RxD3D9InstanceData*)(header + 1);

	_rwD3D9SetVertexShader(VS_deferred);
	_rwD3D9SetPixelShader(PS_deferred);

	CDrawable::deferredRendering(entry, object, flags);

	RwMatrix* LTM = RwFrameGetLTM(RpAtomicGetFrame(object));
	XMMATRIX worldMatrix = RwMatrixToXMMATRIX(LTM);
	_rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);

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
		RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)120);

		RwRGBAReal colorValue = {1.0, 1.0, 1.0, 1.0};

		float fSpec = max(CWeather__WetRoads, CCustomCarEnvMapPipeline__GetFxSpecSpecularity(material));
		float shininess = CCustomCarEnvMapPipeline__GetFxEnvShininess(material);

		RwV4d materialProps;
		materialProps.x = fSpec;
		materialProps.y = shininess;
		materialProps.z = CCustomBuildingDNPipeline__m_fDNBalanceParam;
		materialProps.w = 2.2;

		if(flags & rpGEOMETRYLIGHT)
		{
			if(flags & rpGEOMETRYMODULATEMATERIALCOLOR)
			{
				RwRGBARealFromRwRGBA(&colorValue, matcolor);
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

void DefaultMeshPipeline::forwardRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{}