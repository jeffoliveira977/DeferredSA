#include "CDrawable.h"
#include "CommonD.h"
#include "GTADef.h"
#include "DeferredRenderer.h"
#include "CTimeCycle.h"
#include "DeferredSA.h"
#include "CScene.h"

void MeshRenderingMode::createShaders()
{
	VS_shadow = new VertexShader();
	VS_shadow->CreateFromBinary("ShadowMappingVS");

	PS_shadow = new PixelShader();
	PS_shadow->CreateFromBinary("ShadowMappingPS");

	VS_Distance = new VertexShader();
	VS_Distance->CreateFromBinary("DistanceVS");

	PS_Distance = new PixelShader();
	PS_Distance->CreateFromBinary("DistancePS");

}

void MeshRenderingMode::RenderCallBack(RwResEntry* entry, void* object, RwUInt8 type, RwUInt32 flags)
{
	XMMATRIX worldMatrix;

	RxD3D9ResEntryHeader* header;
	RxD3D9InstanceData* instance;

	header = (RxD3D9ResEntryHeader*)(entry + 1);
	instance = (RxD3D9InstanceData*)(header + 1);

	// set indices and vertex buffer
	_rwD3D9SetIndices(header->indexBuffer);
	_rwD3D9SetStreams(header->vertexStream, header->useOffsets);
	_rwD3D9SetVertexDeclaration(header->vertexDeclaration);

	RwMatrix* LTM = RwFrameGetLTM(RpAtomicGetFrame(object));
	worldMatrix = RwMatrixToXMMATRIX(LTM);
	_rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);

	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);

	switch(gRenderState)
	{
		case stageDeferred:
			DeferredRendering(entry, object, flags);
			break;
		case stageForward:
			ForwardRendering(entry, object, flags);
			break;
		case stageDualParaboloidMap:
		case stageSphereMap:
		case stageReflectionCubemap:
		case stageReflection:
		case stageRefraction:
			ReflectionRendering(entry, object, flags);
			break;
		case stageCascadeShadow:	
		case stagePointShadow:
			ShadowRendering(entry, object, flags);
			break;
	}
}

void MeshRenderingMode::ShadowRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{
	XMMATRIX worldMatrix;

	RwMatrix* LTM;
	RxD3D9ResEntryHeader* header;
	RxD3D9InstanceData* instance;

	header = (RxD3D9ResEntryHeader*)(entry + 1);
	instance = (RxD3D9InstanceData*)(header + 1);

	LTM = RwFrameGetLTM(RpAtomicGetFrame(object));
	worldMatrix = RwMatrixToXMMATRIX(LTM);

	_rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);
	
	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	if (gRenderState == stagePointShadow)
	{
		RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)255);
		VS_Distance->Apply();
		PS_Distance->Apply();
	}
	else
	{
		RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)255);
		/*VS_shadow->Apply();
		PS_shadow->Apply();*/
		VS_Distance->Apply();
		PS_Distance->Apply();
	}

	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)3);

	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)1u);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTUREPERSPECTIVE, (void*)1u);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)1u);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)1u);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATESHADEMODE, (void*)2u);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)2u);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATESRCBLEND, (void*)5u);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)6u);

	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATECULLMODE, (void*)2u);
//	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, (void*)rwALPHATESTFUNCTIONGREATEREQUAL);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)2u);
//	RwD3D9SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	//RwRenderStateSet(rwRENDERSTATEFOGENABLE, FALSE);
	//RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	 RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)2);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);

	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)255);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);


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

		RwRGBAReal colorValue;
		RwRGBARealFromRwRGBA(&colorValue, matcolor);
		_rwD3D9SetPixelShaderConstant(0, &colorValue, 1);
		
		hasAlpha = hasAlpha || instance->vertexAlpha || matcolor->alpha != 255;
		RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)hasAlpha);
		D3D9Render(header, instance, texture, flags);
		instance++;
	}
}

void MeshRenderingMode::ReflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{
	D3DXMATRIX           viewMatrix;
	RwD3D9GetTransform(D3DTS_VIEW, &viewMatrix);

	D3DXMatrixInverse(&viewMatrix, 0, &viewMatrix);
	D3DXVECTOR3 vecCamPos(viewMatrix.m[3][0], viewMatrix.m[3][1], viewMatrix.m[3][2]);
	_rwD3D9SetVertexShaderConstant(8, &vecCamPos, 1);

	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)100);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	_rwD3D9SetPixelShaderConstant(0, &GetSkyTopColor(), 1);
	_rwD3D9SetPixelShaderConstant(1, &GetSkyBottomColor(), 1);
	_rwD3D9SetPixelShaderConstant(2, &GetSunColor(), 1);
	_rwD3D9SetPixelShaderConstant(3, &GetSunDirection(), 1);

	if(gRenderState == RenderingStage::stageRefraction)
		RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK);

	float fog[2];
	fog[0] = CTimeCycle::m_CurrentColours.m_fFogStart;
	fog[1] = Scene.m_pRwCamera->farPlane;
	_rwD3D9SetPixelShaderConstant(4, fog, 1);

	VS_forward->Apply();
	PS_forward->Apply();
}

void MeshRenderingMode::DeferredRendering(RwResEntry * entry, void* object, RwUInt32 flags)
{
	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	_rwD3D9SetPixelShaderConstant(0, &Scene.m_pRwCamera->farPlane, 1);
}

void MeshRenderingMode::ForwardRendering(RwResEntry * entry, void* object, RwUInt32 flags)
{
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	_rwD3D9SetPixelShaderConstant(0, &GetSkyTopColor(), 1);
	_rwD3D9SetPixelShaderConstant(1, &GetSkyBottomColor(), 1);
	_rwD3D9SetPixelShaderConstant(2, &GetSunColor(), 1);
	_rwD3D9SetPixelShaderConstant(3, &GetSunDirection(), 1);

	float fog[2];
	fog[0] = CTimeCycle::m_CurrentColours.m_fFogStart;
	fog[1] = Scene.m_pRwCamera->farPlane;
	_rwD3D9SetPixelShaderConstant(4, fog, 1);
	VS_forward->Apply();
	PS_forward->Apply();
}

void MeshRenderingMode::D3D9Render(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* instance, RwTexture* texture, RwUInt32 flags)
{
	if(flags & (rxGEOMETRY_TEXTURED2 | rxGEOMETRY_TEXTURED))
		RwD3D9SetTexture(instance->material->texture, 0);
	else
		RwD3D9SetTexture(NULL, 0);

	if(header->indexBuffer)
		_rwD3D9DrawIndexedPrimitive(header->primType, instance->baseIndex,
								   0, instance->numVertices,
								   instance->startIndex, instance->numPrimitives);
	else
		_rwD3D9DrawPrimitive(header->primType, instance->baseIndex, instance->numPrimitives);
}