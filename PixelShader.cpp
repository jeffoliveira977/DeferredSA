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
	string path = DEFERREDSHADERPATH + file + ".cso";
	
	ifstream f(file);
	if(!f.good())
	{
		f.close();
		string message = "Failed to load shader: " + file;
		MessageBox(0, &message[0], "Error", MB_OK);
	}

	auto bytes = readFile(path);
	RwD3D9CreateVertexShader((RwUInt32*)bytes.data(), &mPixelShader);
}

void PixelShader::CreateFromFile(string file, string profile)
{
    mPixelShader = CreatePixelShader(file, profile);
}

void* PixelShader::GetShader()
{
    return mPixelShader;
}
