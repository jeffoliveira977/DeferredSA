#pragma once
#include "game_sa\RenderWare.h"
class CDrawable
{
public:
	virtual void RenderCallBack     (RwResEntry* entry, void* object, RwUInt8 type, RwUInt32 flags);
protected:
	void createShaders();

	virtual void cascadeShadowRendering (RwResEntry* entry, void* object, RwUInt32 flags);
	virtual void reflectionRendering	(RwResEntry* entry, void* object, RwUInt32 flags);
	virtual void deferredRendering		(RwResEntry* entry, void* object, RwUInt32 flags);
	virtual void forwardRendering		(RwResEntry* entry, void* object, RwUInt32 flags);
	static	void D3D9Render				(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* instance, RwTexture* texture, RwUInt32 flags);

	void* VS_forward;
	void* PS_forward;

	void* VS_shadow;
	void* PS_shadow;
};