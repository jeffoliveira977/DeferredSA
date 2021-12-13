#pragma once

#include "CommonD.h"
#include "ShaderConstant.h"

class PixelShader : public ConstantTable
{
public:
	PixelShader();
	~PixelShader();

	void CreateFromBinary(string file);
	void CreateFromFile(string file, string profile);

	void* GetObject();
	void* mBinary;
private:
	void* mPixelShader;
};

