#pragma once

#include "CommonD.h"


class VertexShaderState
{
public:
	void addFloatConstant(int index, float value);
	void addIntConstant(int index, int value);
	void addFloatArrayConstant(int index, float *value, size_t size);
	void addntArrayConstant(int index, int* value, size_t size);
	void addMatrixConstant(int index, void* matrix);
	void addVectorConstant(int index, float* vector);

	void push();
	void pop();
private:
	struct ConstantData
	{
		void* valueArray;
		int index;
		int size;
	};
	void* VertexShader;
	std::vector<ConstantData> ConstantsList;
	float VertexShaderConstantsTable[256*4];
};

