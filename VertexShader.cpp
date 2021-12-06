#include "VertexShader.h"

VertexShader::VertexShader()
{
    mVertexShader = nullptr;
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

void* VertexShader::GetShader()
{
    return mVertexShader;
}
