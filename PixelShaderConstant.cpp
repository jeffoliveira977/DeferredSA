#include "PixelShaderConstant.h"

void PixelShaderConstant::SetFloat(RwUInt32 index, float value)
{
    float params[] = {0.0f, 0.0f, 0.0f, 0.0f};

    params[0] = value;
    _rwD3D9SetPixelShaderConstant(index, params, 1);
}

void PixelShaderConstant::SetInt(RwUInt32 index, int value)
{
    float params[] = {0.0f, 0.0f, 0.0f, 0.0f};

    params[0] = static_cast<float>(value);
    _rwD3D9SetPixelShaderConstant(index, params, 1);
}

void PixelShaderConstant::SetVector(RwUInt32 index, void* value)
{
    _rwD3D9SetPixelShaderConstant(index, value, 1);
}

void PixelShaderConstant::SetData(RwUInt32 index, void* value, size_t size)
{
    _rwD3D9SetPixelShaderConstant(index, value, size);
}

void PixelShaderConstant::SetMatrix(RwUInt32 index, void* value)
{
    _rwD3D9SetPixelShaderConstant(index, value, 4);
}

void PixelShaderConstant::SetMatrixArray(RwUInt32 index, void* value, size_t size)
{
    _rwD3D9SetPixelShaderConstant(index, value, size / sizeof(float[4]));
}