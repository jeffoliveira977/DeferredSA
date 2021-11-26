#include "CDrawable.h"
#include "CommonD.h"
#include "GTADef.h"
#include "DeferredRenderer.h"
#include "CTimeCycle.h"
#include "DeferredSA.h"

void CDrawable::createShaders()
{
	//VS_shadow = CreateVertexShader("DeferredSA/shaders/ShadowMapping.hlsl", "VS_Shadow");
	//PS_shadow = CreatePixelShader("DeferredSA/shaders/ShadowMappingPS.hlsl", "main");

	std::vector<BYTE> bytes;
	bytes = readFile("DeferredSA/shaders/binary/ShadowMappingVS.cso");
	RwD3D9CreateVertexShader((RwUInt32*)bytes.data(), &VS_shadow);

	bytes = readFile("DeferredSA/shaders/binary/ShadowMappingPS.cso");
	RwD3D9CreatePixelShader((RwUInt32*)bytes.data(), &PS_shadow);
}

void CDrawable::RenderCallBack(RwResEntry* entry, void* object, RwUInt8 type, RwUInt32 flags)
{
	//RwD3D9EnableClippingIfNeeded(object, type);

	RxD3D9ResEntryHeader* header;
	RxD3D9InstanceData* instance;

	header = (RxD3D9ResEntryHeader*)(entry + 1);
	instance = (RxD3D9InstanceData*)(header + 1);

	// set indices and vertex buffer
	_rwD3D9SetIndices(header->indexBuffer);
	_rwD3D9SetStreams(header->vertexStream, header->useOffsets);
	_rwD3D9SetVertexDeclaration(header->vertexDeclaration);


	if(gRenderState == stageDeferred)
		deferredRendering(entry, object, flags);
	else if(gRenderState == stageForward)
	{
		forwardRendering(entry, object, flags);
	}
	else if(gRenderState == stageCascadeShadow)
		cascadeShadowRendering(entry, object, flags);
	else
		reflectionRendering(entry, object, flags);
}

void CDrawable::cascadeShadowRendering(RwResEntry* entry, void* object, RwUInt32 flags)
{
	XMMATRIX  worldMatrix, viewMatrix, projectionMatrix;

	RwMatrix* LTM;
	RxD3D9ResEntryHeader* header;
	RxD3D9InstanceData* instance;

	header = (RxD3D9ResEntryHeader*)(entry + 1);
	instance = (RxD3D9InstanceData*)(header + 1);

	LTM = RwFrameGetLTM(RpAtomicGetFrame(object));
	worldMatrix = RwMatrixToXMMATRIX(LTM);


	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)1u);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTUREPERSPECTIVE, (void*)1u);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)1u);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)1u);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATESHADEMODE, (void*)2u);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)2u);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATESRCBLEND, (void*)5u);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)6u);

	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATECULLMODE, (void*)1u);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, (void*)rwALPHATESTFUNCTIONGREATEREQUAL);
	RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)2u);

	_rwD3D9SetVertexShaderConstant(0, &worldMatrix, 4);
	
	//RwRenderStateSet(rwRENDERSTATEFOGENABLE, FALSE);
	//RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	// RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)TRUE);
	_rwD3D9SetVertexShader(VS_shadow);
	_rwD3D9SetPixelShader(PS_shadow);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)20);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	//rwD3D9SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//rwD3D9SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//rwD3D9SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	//rwD3D9SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	//rwD3D9SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	//rwD3D9SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);

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

		//if(hasAlpha)
		//	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)170);
		//else
		//	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0);

		//hasAlpha = hasAlpha || instance->vertexAlpha || matcolor->alpha != 255;
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

void CDrawable::reflectionRendering(RwResEntry* entry, void* object, RwUInt32 flags)
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
	fog[1] = CTimeCycle::m_CurrentColours.m_fFarClip;
	_rwD3D9SetPixelShaderConstant(4, fog, 1);

	_rwD3D9SetVertexShader(VS_forward);
	_rwD3D9SetPixelShader(PS_forward);
}

void CDrawable::deferredRendering(RwResEntry * entry, void* object, RwUInt32 flags)
{
	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	RwD3D9SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	_rwD3D9SetPixelShaderConstant(0, &CTimeCycle::m_CurrentColours.m_fFarClip, 1);
}

void CDrawable::forwardRendering(RwResEntry * entry, void* object, RwUInt32 flags)
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
	fog[1] = CTimeCycle::m_CurrentColours.m_fFarClip;
	_rwD3D9SetPixelShaderConstant(4, fog, 1);

	_rwD3D9SetVertexShader(VS_forward);
	_rwD3D9SetPixelShader(PS_forward);
}

void CDrawable::D3D9Render(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* instance, RwTexture* texture, RwUInt32 flags)
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