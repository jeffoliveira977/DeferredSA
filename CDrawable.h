#pragma once
#include "game_sa\RenderWare.h"
#include "PixelShader.h"
#include "VertexShader.h"

class MeshRenderingMode
{
public:
	virtual void RenderCallBack     (RwResEntry* entry, void* object, RwUInt8 type, RwUInt32 flags);
protected:
	void createShaders();

	virtual void ShadowRendering (RwResEntry* entry, void* object, RwUInt32 flags);
	virtual void ReflectionRendering	(RwResEntry* entry, void* object, RwUInt32 flags);
	virtual void DeferredRendering		(RwResEntry* entry, void* object, RwUInt32 flags);
	virtual void ForwardRendering		(RwResEntry* entry, void* object, RwUInt32 flags);
	static	void D3D9Render				(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* instance, RwTexture* texture, RwUInt32 flags);

	VertexShader* VS_forward;
	PixelShader* PS_forward;

	VertexShader* VS_shadow;
	PixelShader* PS_shadow;

	VertexShader* VS_Distance;
	PixelShader* PS_Distance;
};