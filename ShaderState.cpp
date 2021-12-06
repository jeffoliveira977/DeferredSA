#include "ShaderState.h"

void VertexShaderState::addFloatConstant(int index, float value)
{
    float f[4];
    f[0] = value;
    f[1] = 0.0;
    f[2] = 0.0;
    f[3] = 0.0;

    ConstantData cd;
    cd.index = index;
    cd.valueArray = f;
    cd.size = 1;
    ConstantsList.push_back(cd);
}

void VertexShaderState::addIntConstant(int index, int value)
{
    float f[4];
    f[0] = static_cast<float>(value);
    f[1] = 0.0;
    f[2] = 0.0;
    f[3] = 0.0;
    
    ConstantData cd;
    cd.index = index;
    cd.valueArray = f;
    cd.size = 1;
    ConstantsList.push_back(cd);
}

void VertexShaderState::addFloatArrayConstant(int index, float *value, size_t size)
{
    ConstantData cd;
    cd.index = index;
    cd.valueArray = value;
    cd.size = size;
    ConstantsList.push_back(cd);
}

void VertexShaderState::addntArrayConstant(int index, int *value, size_t size)
{}

void VertexShaderState::addMatrixConstant(int index, void* matrix)
{
    ConstantData cd;
    cd.index = index;
    cd.valueArray = matrix;
    cd.size = 4;
    ConstantsList.push_back(cd);
}

void VertexShaderState::addVectorConstant(int index, float* vector)
{
    ConstantData cd;
    cd.index = index;
    cd.valueArray = vector;
    cd.size = 1;
    ConstantsList.push_back(cd);
}

void VertexShaderState::push()
{
    int i = 0;
    for(auto cd : ConstantsList)
    {      
        _rwD3D9GetVertexShaderConstant(cd.index, &VertexShaderConstantsTable[i], cd.size);
        _rwD3D9SetVertexShaderConstant(cd.index, cd.valueArray, cd.size);
        i++;
    }
}

void VertexShaderState::pop()
{
    int i = 0;
    for(auto cd : ConstantsList)
    {
        _rwD3D9SetVertexShaderConstant(cd.index, &VertexShaderConstantsTable[i], cd.size);
        i++;
    }

    ConstantsList.clear();
}
