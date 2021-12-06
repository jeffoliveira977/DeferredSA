#include "PixelShader.h"

PixelShader::PixelShader()
{
    mPixelShader = nullptr;
}

PixelShader::~PixelShader()
{
    RwD3D9DeletePixelShader(mPixelShader);
}

void PixelShader::CreateFromBinary(string file)
{
    mPixelShader = RwCreateCompiledPixelShader(file);
}

void PixelShader::CreateFromFile(string file, string profile)
{
    mPixelShader = CreatePixelShader(file, profile);
}

void* PixelShader::GetShader()
{
    return mPixelShader;
}
