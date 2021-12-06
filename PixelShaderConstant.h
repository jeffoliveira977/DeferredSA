#pragma once
#include "commonD.h"

class PixelShaderConstant
{
public:
	static void SetFloat(RwUInt32 index, float value);
	static void SetInt(RwUInt32 index, int value);
	static void SetVector(RwUInt32 index, void* value);
	static void SetData(RwUInt32 index, void* value, size_t size);
	static void SetMatrix(RwUInt32 index, void* value);
	static void SetMatrixArray(RwUInt32 index, void* value, size_t size);
};

