#include "VertexShader.h"

VertexShader::VertexShader()
{
    mVertexShader = nullptr;
    mParams[0] = mParams[1] =  mParams[2] = mParams[3] = 0.0f;
}

VertexShader::~VertexShader()
{
    RwD3D9DeletePixelShader(mVertexShader);
    mVertexShader = nullptr;
}

void VertexShader::CreateFromBinary(string file)
{
    mVertexShader = RwCreateCompiledVertexShader(file);
}

void VertexShader::CreateFromFile(string file, string profile)
{
    mVertexShader = CreateVertexShader(file, profile);
}

void VertexShader::SetFloat(RwUInt32 index, float value)
{
    mParams[0] = value;
    _rwD3D9SetVertexShaderConstant(index, mParams, 1);
}

void VertexShader::SetInt(RwUInt32 index, int value)
{
    mParams[0] = static_cast<float>(value);
    _rwD3D9SetVertexShaderConstant(index, mParams, 1);
}

void VertexShader::SetVector(RwUInt32 index, void* value)
{
    _rwD3D9SetVertexShaderConstant(index, value, 1);
}

void VertexShader::SetData(RwUInt32 index, void* value, size_t size)
{
    _rwD3D9SetVertexShaderConstant(index, value, size);
}

void VertexShader::SetMatrix(RwUInt32 index, void* value)
{
    _rwD3D9SetVertexShaderConstant(index, value, 4);
}

void VertexShader::SetMatrixArray(RwUInt32 index, void* value, size_t size)
{
    _rwD3D9SetVertexShaderConstant(index, value, size / sizeof(float[4]));
}

void* VertexShader::GetShader()
{
    return mVertexShader;
}
