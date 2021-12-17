#include "VertexShaderConstant.h"

void VertexShaderConstant::SetFloat(RwUInt32 index, float value)
{
    float params[] = {0.0f, 0.0f, 0.0f, 0.0f};

    params[0] = value;
    _rwD3D9SetVertexShaderConstant(index, params, 1);
}

void VertexShaderConstant::SetInt(RwUInt32 index, int value)
{
    float params[] = {0.0f, 0.0f, 0.0f, 0.0f};

    params[0] = static_cast<float>(value);
    _rwD3D9SetVertexShaderConstant(index, params, 1);
}

void VertexShaderConstant::SetVector(RwUInt32 index, void* value)
{
    _rwD3D9SetVertexShaderConstant(index, value, 1);
}

void VertexShaderConstant::SetData(RwUInt32 index, void* value, size_t size)
{
    _rwD3D9SetVertexShaderConstant(index, value, size);
}

void VertexShaderConstant::SetMatrix(RwUInt32 index, void* value)
{
    _rwD3D9SetVertexShaderConstant(index, value, 4);
}

void VertexShaderConstant::SetMatrix(RwUInt32 index, XMMATRIX value)
{
    _rwD3D9SetVertexShaderConstant(index, &value, 4);
}

void VertexShaderConstant::SetMatrixArray(RwUInt32 index, void* value, size_t size)
{
    _rwD3D9SetVertexShaderConstant(index, value, size / sizeof(float[4]));
}