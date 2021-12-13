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
inline DWORD* fileread(const char* filename)
{
	FILE* fp = nullptr;
	long size;
	fopen_s(&fp, filename, "rb");
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	DWORD* dat = new DWORD[size];
	fread_s(dat, size * sizeof(DWORD), size, 1, fp);
	fclose(fp);
	return dat;
}

void PixelShader::CreateFromBinary(string file)
{
	string path = DEFERREDSHADERPATHBINARY + file + ".cso";
	if(!std::filesystem::exists(path))
	{
		string message = "Failed to load shader: " + file;
		MessageBox(0, &message[0], "Error", MB_OK);
	}

	auto bytes = fileread(path.c_str());
	mBinary = bytes;
	RwD3D9CreatePixelShader((RwUInt32*)bytes, &mPixelShader);
	Initialize(bytes);

	delete[] bytes;
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
	Initialize(vsCodeBlob->GetBufferPointer());
	vsCodeBlob->Release();
}

void* PixelShader::GetObject()
{
    return mPixelShader;
}
