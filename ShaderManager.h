#pragma once
#include "CommonD.h"

class ShaderManager
{
	CVector mSkyTopColor;
	CVector mSkytBottomColor;
	CVector mSunColor;
	float mSettings[4];
	CVector* sunDirs;

public:
	void SetTimecyProps(RwUInt32 index);
	void SetSunColor(RwUInt32 index);
	void SetFogParams(RwUInt32 index);
	void SetSkyColor(RwUInt32 index);
	void SetSunDirection(RwUInt32 index);
	void SetViewMatrix(RwUInt32 index, bool vs = false);
	void SetInverseViewMatrix(RwUInt32 index, bool vs = false);
	void SetProjectionMatrix(RwUInt32 index, bool vs = false);
	void SetViewProjectionMatrix(RwUInt32 index, bool vs = false);
	void SetViewProjectionMatrix(RwUInt32 index, XMMATRIX view, XMMATRIX projection);
	void Update();
};

extern ShaderManager* ShaderContext;
