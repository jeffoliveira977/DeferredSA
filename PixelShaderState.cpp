#include "PixelShaderState.h"
#include "ShaderState.h"

void PixelShaderState::createFromBinary(string file)
{
    PixelShader = RwCreateCompiledPixelShader(file);
}

void PixelShaderState::createFromFile(string file, string profile)
{
    PixelShader = CreatePixelShader(file, profile);
}

void PixelShaderState::addFloatConstant(int index, float value)
{
    float f[4];
    f[0] = value;
    f[1] = f[2] = f[3] = 0.0;

    ConstantData cd;
    cd.index = index;
    cd.valueArray = f;
    cd.size = 1;
    ConstantsList.push_back(cd);
}

void PixelShaderState::addIntConstant(int index, int value)
{
    float f[4];
    f[0] = static_cast<float>(value);
    f[1] = f[2] = f[3] = 0.0;

    ConstantData cd;
    cd.index = index;
    cd.valueArray = f;
    cd.size = 1;
    ConstantsList.push_back(cd);
}

void PixelShaderState::addFloatArrayConstant(int index, float* value, size_t size)
{
    ConstantData cd;
    cd.index = index;
    cd.valueArray = value;
    cd.size = size;
    ConstantsList.push_back(cd);
}

void PixelShaderState::addntArrayConstant(int index, int* value, size_t size)
{}

void PixelShaderState::addMatrixConstant(int index, void* matrix)
{
    ConstantData cd;
    cd.index = index;
    cd.valueArray = matrix;
    cd.size = 4;
    ConstantsList.push_back(cd);
}

void PixelShaderState::addVectorConstant(int index, float* vector)
{
    ConstantData cd;
    cd.index = index;
    cd.valueArray = vector;
    cd.size = 1;
    ConstantsList.push_back(cd);
}

void PixelShaderState::push()
{
    _rwD3D9SetVertexShader(PixelShader);

    int i = 0;
    for(auto cd : ConstantsList)
    {
        _rwD3D9GetPixelShaderConstant(cd.index, &PixelShaderConstantsTable[cd.index *4], cd.size);

        _rwD3D9SetPixelShaderConstant(cd.index, cd.valueArray, cd.size);
        i++;
    }
}

void PixelShaderState::pop()
{
    int i = 0;
    for(auto cd : ConstantsList)
    {
        _rwD3D9SetPixelShaderConstant(cd.index, &PixelShaderConstantsTable[cd.index *4], cd.size);
        i++;
    }

    ConstantsList.clear();
}
