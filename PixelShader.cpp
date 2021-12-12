#include "PixelShader.h"
#include <d3dcompiler.h>
#include <filesystem>

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
	string path = DEFERREDSHADERPATHBINARY + file + ".cso";
	if(!std::filesystem::exists(path))
	{
		string message = "Failed to load shader: " + file;
		MessageBox(0, &message[0], "Error", MB_OK);
	}

	auto bytes = readFile(path);
	RwD3D9CreatePixelShader((RwUInt32*)bytes.data(), &mPixelShader);
}

void PixelShader::CreateFromFile(string file, string profile)
{
	string path = DEFERREDSHADERPATH + file + ".hlsl";
	if(!std::filesystem::exists(path))
	{
		string message = "Failed to load shader: " + file;
		MessageBox(0, &message[0], "Error", MB_OK);
	}

	ID3DBlob* vsCodeBlob;
	ID3DBlob* vsErrorBlob;

	std::filesystem::path parentPath = std::filesystem::canonical(path);
	D3DCompileFromFile(parentPath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, profile.c_str(), "ps_3_0", NULL, 0, &vsCodeBlob, &vsErrorBlob);
	
	if(vsErrorBlob && vsErrorBlob->GetBufferPointer())
	{
		MessageBox(0, (char*)vsErrorBlob->GetBufferPointer(), "Error", MB_OK);
		vsErrorBlob->Release();
	}

	RwD3D9CreatePixelShader((RwUInt32*)vsCodeBlob->GetBufferPointer(), &mPixelShader);
	vsCodeBlob->Release();
}

void* PixelShader::GetObject()
{
    return mPixelShader;
}
