#pragma once

#include "CommonD.h"

class VertexShader
{
public:
	VertexShader();
	~VertexShader();

	void CreateFromBinary(string file);
	void CreateFromFile(string file, string profile);

	void SetFloat(RwUInt32 index, float value);
	void SetInt(RwUInt32 index, int value);
	void SetVector(RwUInt32 index, void* value);
	void SetData(RwUInt32 index, void* value, size_t size);
	void SetMatrix(RwUInt32 index, void* value);
	void SetMatrixArray(RwUInt32 index, void* value, size_t size);

	void* GetShader();
private:
	void* mVertexShader;
	float mParams[4];
};

