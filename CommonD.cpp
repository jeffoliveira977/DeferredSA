#include "CommonD.h"

#include "CTimeCycle.h"
char(&PC_Scratch)[16384] = *(char(*)[16384])0xC8E0C8;

RwUInt32& CurrentBaseIndex3D = *(RwUInt32*)0xC9AB34;
LPDIRECT3DVERTEXBUFFER9& CurrentVertexBuffer3D = *(LPDIRECT3DVERTEXBUFFER9*)0xC9AB38;
RwUInt32& IB3DOffset = *(RwUInt32*)0xC9AB3C;
LPDIRECT3DINDEXBUFFER9& IndexBuffer3D = *(LPDIRECT3DINDEXBUFFER9*)0xC9AB40;
LPDIRECT3DVERTEXDECLARATION9& VertexDeclIm3D = *(LPDIRECT3DVERTEXDECLARATION9*)0xC9AB48;
LPDIRECT3DVERTEXDECLARATION9& VertexDeclIm3DNoTex = *(LPDIRECT3DVERTEXDECLARATION9*)0xC9AB4C;
LPDIRECT3DVERTEXDECLARATION9& VertexDeclIm3DOld = *(LPDIRECT3DVERTEXDECLARATION9*)0xC9AB44;
rwIm3DPool *& _rwD3D9ImmPool = *(rwIm3DPool**)0xC9AB30;
D3DPRIMITIVETYPE* _RwD3D9PrimConv = (D3DPRIMITIVETYPE*)0x88513C;

uint16_t& uiTempBufferIndicesStored = *(uint16_t*)0xC4B954;
uint16_t& uiTempBufferVerticesStored = *(uint16_t*)0xC4B950;
RxObjSpace3DVertex(&aTempBufferVertices)[TOTAL_TEMP_BUFFER_VERTICES] = *(RxObjSpace3DVertex(*)[TOTAL_TEMP_BUFFER_VERTICES])0xC4D958;
RxVertexIndex(&aTempBufferIndices)[TOTAL_TEMP_BUFFER_INDICES] = *(RxVertexIndex(*)[TOTAL_TEMP_BUFFER_INDICES])0xC4B958;

#include "dxerr.h"

HRESULT CheckError(HRESULT callResult, const std::string& errorMessage)
{
	if (IS_ERROR(callResult))
	{
		auto wstri = std::wstring(DXGetErrorString(callResult));

		std::string str;
		std::transform(wstri.begin(), wstri.end(), std::back_inserter(str), [](wchar_t c) { return (char)c;});

		Log::Error("ERROR_CODE: " + str + " : " + errorMessage );
		MessageBox(*WindowHandle, errorMessage.c_str(), "RUNTIME ERROR", 0);
	}
	return callResult;
}

inline bool exists_test0(const std::string& name)
{
	ifstream f(name.c_str());
	return f.good();
}

void rwD3D9SetSamplerState(RwUInt32 stage, RwUInt32 type, RwUInt32 value)
{
	// Renderware has 8 sample state limit
	if(stage < 8)
		RwD3D9SetSamplerState(stage, type, value);
	else
		RwD3DDevice->SetSamplerState(stage, (D3DSAMPLERSTATETYPE)type, value);
}

#include "D3D9BaseTexture.h"
void rwD3D9RWSetRasterStage(RwRaster *raster, RwUInt32 stage)
{
	// Renderware has 8 sample state limit
	if(stage < 8)
		_rwD3D9RWSetRasterStage(raster, stage);
	else
	{
		auto rasExt = RASTEREXTFROMRASTER(raster);
		RwD3DDevice->SetTexture(stage, rasExt->texture);
	}
}

void _rwD3D9GetVertexShaderConstant(RwUInt32 registerAddress, void* constantData, RwUInt32 constantCount)
{
	RwD3DDevice->GetVertexShaderConstantF(registerAddress, (float*)constantData, constantCount);
}

void _rwD3D9GetPixelShaderConstant(RwUInt32 registerAddress, void* constantData, RwUInt32 constantCount)
{
	RwD3DDevice->GetPixelShaderConstantF(registerAddress, (float*)constantData, constantCount);
}

void RwD3D9RestoreRenderTargets(int count)
{
	__rwD3D9SetRenderTarget(0, RwD3D9RenderSurface);

	for(size_t i = 1; i < count; i++)
	__rwD3D9SetRenderTarget(i, NULL);
}

void rwD3D9SetRenderTargets(RwRaster* rasters[], int rastersCount, RwUInt32 startIndex)
{
	for(size_t i = 0; i < rastersCount; i++)
	{
		RwD3D9SetRenderTarget(i + startIndex, rasters[i]);
	}

	// Keeps original surface
	CurrentRenderSurface[0] = RwD3D9RenderSurface;

	// Clear screen buffer
	RwD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0, 0);
}

void rwD3D9SetDepthStencil(RwRaster* depthStencil)
{
	auto zRaster = RASTEREXTFROMRASTER(depthStencil);
	_rwD3D9SetDepthStencilSurface((LPSURFACE)zRaster->texture);
}

void rwD3D9Clear(RwRGBA* color, RwInt32 clearFlags)
{
	D3DVIEWPORT9        viewport;
	RwUInt32            d3d9ClearFlags = 0;
	D3DCOLOR packedColor;

	/* Clear */
	if(rwCAMERACLEARIMAGE & clearFlags)
	{
		d3d9ClearFlags |= D3DCLEAR_TARGET;
	}

	if(rwCAMERACLEARZ & clearFlags)
	{
		d3d9ClearFlags |= D3DCLEAR_ZBUFFER;
	}

	if(rwCAMERACLEARSTENCIL & clearFlags)
	{
		d3d9ClearFlags |= D3DCLEAR_STENCIL;
	}

	if(color)
	{
		packedColor = (D3DCOLOR)(((color->alpha) << 24) | ((color->red) << 16) |
			((color->green) << 8) | (color->blue));
	}

	IDirect3DDevice9_Clear(RwD3DDevice, 0, NULL, d3d9ClearFlags, color? packedColor : 0, 1.0f, 0.0f);
}

#include <fstream>
#include <iterator>
#include <fstream>
#include <chrono>
#include <vector>
#include <cstdint>
#include <numeric>
#include <random>
#include <algorithm>
#include <iostream>
#include <cassert>

std::vector<uint64_t> GenerateData(std::size_t bytes)
{
	assert(bytes % sizeof(uint64_t) == 0);
	std::vector<uint64_t> data(bytes / sizeof(uint64_t));
	std::iota(data.begin(), data.end(), 0);
	std::shuffle(data.begin(), data.end(), std::mt19937{std::random_device{}()});
	return data;
}

long long option_2(string filename, std::size_t bytes)
{
	std::vector<uint64_t> data = GenerateData(bytes);

	auto startTime = std::chrono::high_resolution_clock::now();
	FILE* file = fopen(filename.c_str(), "wb");
	fwrite(&data[0], 1, bytes, file);
	fclose(file);
	auto endTime = std::chrono::high_resolution_clock::now();

	return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
}

std::vector<BYTE> readFile(string filename)
{
	std::ifstream file(filename, std::ios::binary);
	file.unsetf(std::ios::skipws);
	std::streampos fileSize;
	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<BYTE> vec;
	vec.reserve(fileSize);
	vec.insert(vec.begin(),
			   std::istream_iterator<BYTE>(file),
			   std::istream_iterator<BYTE>());

	return vec;
}

void* RwCreateCompiledVertexShader(string filename)
{
	void* shader;
	int length;
	char* buffer;

	string path = DEFERREDSHADERPATHBINARY + filename + ".cso";
	if(!exists_test0(path))
	{
		string message = "Failed to load shader: " + filename;
		MessageBox(0, &message[0], "Error", MB_OK);
	}

	auto bytes = readFile(path);
	RwD3D9CreateVertexShader((RwUInt32*)bytes.data(), &shader);

	return shader;
}

void* RwCreateCompiledPixelShader(string filename)
{
	void* shader;
	int length;
	char* buffer;
	
	string path = DEFERREDSHADERPATHBINARY + filename + ".cso";
	if(!exists_test0(path))
	{
		string message = "Failed to load shader: " + filename;
		MessageBox(0, &message[0], "Error", MB_OK);
	}

	auto bytes = readFile(path);
	RwD3D9CreatePixelShader((RwUInt32*)bytes.data(), &shader);

	return shader;
}

#include <filesystem>


#include <d3dcompiler.h>
IDirect3DVertexShader9* CreateVertexShader(string path, string profile)
{

	void* shader;
	ID3DBlob* vsCodeBlob;
	ID3DBlob* vsErrorBlob;

	string filepath = DEFERREDSHADERPATH + path + ".hlsl";
	std::filesystem::path parentPath = std::filesystem::canonical(filepath);

	D3DCompileFromFile(parentPath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, profile.c_str(), "vs_3_0", D3DCOMPILE_DEBUG, 0, &vsCodeBlob, &vsErrorBlob);

	if(vsErrorBlob && vsErrorBlob->GetBufferPointer())
	{
		MessageBox(0, (char*)vsErrorBlob->GetBufferPointer(), "Error", MB_OK);
	}

	RwD3D9CreateVertexShader((RwUInt32*)vsCodeBlob->GetBufferPointer(), &shader);

	return (IDirect3DVertexShader9*)shader;
}

IDirect3DPixelShader9* CreatePixelShader(string path, string profile)
{
	void* shader;
	ID3DBlob* vsCodeBlob;
	ID3DBlob* vsErrorBlob;

	string filepath = DEFERREDSHADERPATH + path + ".hlsl";
	std::filesystem::path parentPath = std::filesystem::canonical(filepath);
	D3DCompileFromFile(parentPath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, profile.c_str(), "ps_3_0", D3DCOMPILE_DEBUG, 0, &vsCodeBlob, &vsErrorBlob);

	RwD3D9CreatePixelShader((RwUInt32*)vsCodeBlob->GetBufferPointer(), &shader);

	return (IDirect3DPixelShader9*)shader;
}

void RwD3D9SetPixelShaderConstantB(RwUInt32 registerAddress, const void* constantData, RwUInt32 constantCount)
{
	RwD3DDevice->SetPixelShaderConstantB(registerAddress, (BOOL*)constantData, constantCount);
}

#define _daylightLightingState ((float*)0x8D12C0)
bool GetSunPosn(CVector* dst, float farClip)
{
	CVector* sunPos, * camPos;
	bool res;
	sunPos = (CVector*)(*(DWORD*)0xB79FD0 * 12 + 0xB7CA50);
	if(sunPos->z > -0.1f)
		res = TRUE;
	else
		res = FALSE;
	if(*(DWORD*)0xB6F03C)
	{
		camPos = (CVector*)(0xB6F03C + 0x30);
		dst->x = sunPos->x * farClip + camPos->x;
		dst->y = sunPos->y * farClip + camPos->y;
		dst->z = sunPos->z * farClip + camPos->z;
	}
	else
	{
		camPos = (CVector*)0xB6F02C;
		dst->x = sunPos->x * farClip + camPos->x;
		dst->y = sunPos->y * farClip + camPos->y;
		dst->z = sunPos->z * farClip + camPos->z;
	}
	dst->x *= (1 - (*_daylightLightingState)) ? 1 : -1;
	dst->y *= ((1 - (*_daylightLightingState))) ? 1 : -1;
	dst->z *= (1 - (*_daylightLightingState)) ? 1 : -1;
	return res;
}

XMMATRIX RwMatrixToXMMATRIX(const RwMatrix* mtx)
{
	if(mtx == nullptr)
		return XMMatrixIdentity();

	return {mtx->right.x, mtx->right.y, mtx->right.z, 0,
			mtx->up.x,	  mtx->up.y,    mtx->up.z,    0,
			mtx->at.x,    mtx->at.y,    mtx->at.z,    0,
			mtx->pos.x,   mtx->pos.y,   mtx->pos.z,   1};
}

RwMatrix XMMATRIXtoRwMatrix(XMMATRIX mtx)
{
	//if(mtx == nullptr)
	//	return RwMatrixi;

	return {XMVectorGetX(mtx.r[0]), XMVectorGetY(mtx.r[0]), XMVectorGetZ(mtx.r[0]), 0,
			XMVectorGetX(mtx.r[1]), XMVectorGetY(mtx.r[1]), XMVectorGetZ(mtx.r[1]), 0,
			XMVectorGetX(mtx.r[2]), XMVectorGetY(mtx.r[2]), XMVectorGetZ(mtx.r[2]), 0,
			XMVectorGetX(mtx.r[3]), XMVectorGetY(mtx.r[3]), XMVectorGetZ(mtx.r[3]), 1};
}

RwMatrix RwMatrixTranspose(RwMatrix* mtx)
{
	return {mtx->right.x, mtx->right.y, mtx->right.z, 0,
			mtx->up.x,	  mtx->up.y,    mtx->up.z,    0,
			mtx->at.x,    mtx->at.y,    mtx->at.z,    0,
			mtx->pos.x,   mtx->pos.y,   mtx->pos.z,   1};
}

RwRGBA LerpSkyColor()
{
	XMVECTOR outColor, skyTop, skyBottom;

	skyTop = XMVectorSet(static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyTopRed) / 255.0,
						 static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyTopGreen) / 255.0,
						 static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyTopBlue) / 255.0, 1.0);

	skyBottom = XMVectorSet(static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyBottomRed) / 255.0,
							static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyBottomGreen) / 255.0,
							static_cast<float>(CTimeCycle::m_CurrentColours.m_nSkyBottomBlue) / 255.0, 1.0);

	outColor = XMVectorLerp(skyTop, skyBottom, 0.2);

	RwRGBA color;
	color.red = static_cast<RwUInt16>(XMVectorGetX(outColor) * 255.0);
	color.green = static_cast<RwUInt16>(XMVectorGetY(outColor) * 255.0);
	color.blue = static_cast<RwUInt16>(XMVectorGetZ(outColor) * 255.0);
	color.alpha = 255;

	return color;
}

CVector GetSkyTopColor()
{
	return {CTimeCycle::m_CurrentColours.m_nSkyTopRed / 255.0f, CTimeCycle::m_CurrentColours.m_nSkyTopGreen / 255.0f, CTimeCycle::m_CurrentColours.m_nSkyTopBlue / 255.0f};
}
CVector GetSkyBottomColor()
{
	return {CTimeCycle::m_CurrentColours.m_nSkyBottomRed / 255.0f, CTimeCycle::m_CurrentColours.m_nSkyBottomGreen / 255.0f, CTimeCycle::m_CurrentColours.m_nSkyBottomBlue / 255.0f};
}

CVector GetObjectColor()
{
	return {CTimeCycle::m_CurrentColours.m_fAmbientRed_Obj, CTimeCycle::m_CurrentColours.m_fAmbientGreen_Obj, CTimeCycle::m_CurrentColours.m_fAmbientBlue_Obj};
}
CVector GetSunColor()
{
	return {CTimeCycle::m_CurrentColours.m_nSunCoreRed / 255.0f, CTimeCycle::m_CurrentColours.m_nSunCoreGreen / 255.0f, CTimeCycle::m_CurrentColours.m_nSunCoreBlue / 255.0f};
}

CVector GetSunDirection()
{
	CVector* sunDirs = (CVector*)0xB7CA50;
	int sunDirIndex = *(int*)0xB79FD0;
	return sunDirs[sunDirIndex];
}

RwTexture* LoadTextureFromFile(const RwChar* filename)
{
	RwInt32 width, height, depth, flags;
	RwImage* image = RtPNGImageRead(filename);
	RwImageFindRasterFormat(image, 4, &width, &height, &depth, &flags);
	RwRaster* raster = RwRasterCreate(width, height, depth, flags);
	RwRasterSetFromImage(raster, image);
	RwImageDestroy(image);
	return RwTextureCreate(raster);
}

RwTexture* LoadBMPTextureFromFile(const RwChar* filename)
{
	RwInt32 width, height, depth, flags;
	RwImage* image = RtBMPImageRead(filename);
	RwImageFindRasterFormat(image, 4, &width, &height, &depth, &flags);
	RwRaster* raster = RwRasterCreate(width, height, depth, flags);
	RwRasterSetFromImage(raster, image);
	RwImageDestroy(image);
	return RwTextureCreate(raster);
}

//
// Gaussian functions
//
float GetGaussianDistribution(float x, float y, float rho)
{
	float g = 1.0f / sqrt(2.0f * 3.141592654f * rho * rho);
	return g * exp(-(x * x + y * y) / (2 * rho * rho));
}

void GetGaussianOffsets(bool bHorizontal, float texels, RwV2d* vSampleOffsets, float* fSampleWeights)
{
	// Get the center texel offset and weight
	fSampleWeights[0] = 1.0f * GetGaussianDistribution(0, 0, 2.0f);
	vSampleOffsets[0] = {0.0f, 0.0f};

	// Get the offsets and weights for the remaining taps
	if(bHorizontal)
	{
		for(int i = 1; i < 15; i += 2)
		{
			vSampleOffsets[i + 0] = {i * texels, 0.0f};
			vSampleOffsets[i + 1] = {-i * texels, 0.0f};

			fSampleWeights[i + 0] = 2.0f * GetGaussianDistribution(float(i + 0), 0.0f, 3.0f);
			fSampleWeights[i + 1] = 2.0f * GetGaussianDistribution(float(i + 1), 0.0f, 3.0f);
		}
	}

	else
	{
		for(int i = 1; i < 15; i += 2)
		{
			vSampleOffsets[i + 0] = {0.0f, i * texels};
			vSampleOffsets[i + 1] = {0.0f, -i * texels};

			fSampleWeights[i + 0] = 2.0f * GetGaussianDistribution(0.0f, float(i + 0), 3.0f);
			fSampleWeights[i + 1] = 2.0f * GetGaussianDistribution(0.0f, float(i + 1), 3.0f);
		}
	}
}

#include "GTADef.h"

void CameraPan(RwCamera* camera, const RwV3d* pos, RwReal angle)
{
	RwV3d invCamPos;
	RwFrame* cameraFrame;
	RwMatrix* cameraMatrix;
	RwV3d camPos;

	cameraFrame = RwCameraGetFrame(camera);
	cameraMatrix = RwFrameGetMatrix(cameraFrame);

	camPos = (pos) ? *pos : *RwMatrixGetPos(cameraMatrix);

	RwV3dScale(&invCamPos, &camPos, -1.0f);

	/*
	 * Translate the camera back to the rotation origin...
	 */
	RwFrameTranslate(cameraFrame, &invCamPos, rwCOMBINEPOSTCONCAT);

	/*
	 * Get the camera's UP vector and use this as the axis of rotation...
	 */
	RwMatrixRotate(cameraMatrix,
				   RwMatrixGetUp(cameraMatrix), angle, rwCOMBINEPOSTCONCAT);

	/*
	 * Translate the camera back to its original position...
	 */
	RwFrameTranslate(cameraFrame, &camPos, rwCOMBINEPOSTCONCAT);

	return;
}

void CameraTilt(RwCamera* camera, const RwV3d* pos, RwReal angle)
{
	RwV3d invCamPos;
	RwFrame* cameraFrame;
	RwMatrix* cameraMatrix;
	RwV3d camPos;

	cameraFrame = RwCameraGetFrame(camera);
	cameraMatrix = RwFrameGetMatrix(cameraFrame);

	camPos = (pos) ? *pos : *RwMatrixGetPos(cameraMatrix);

	RwV3dScale(&invCamPos, &camPos, -1.0f);

	/*
	 * Translate the camera back to the rotation origin...
	 */
	RwFrameTranslate(cameraFrame, &invCamPos, rwCOMBINEPOSTCONCAT);

	/*
	 * Get the camera's RIGHT vector and use this as the axis of rotation...
	 */
	RwMatrixRotate(cameraMatrix,
				   RwMatrixGetRight(cameraMatrix), angle, rwCOMBINEPOSTCONCAT);

	/*
	 * Translate the camera back to its original position...
	 */
	RwFrameTranslate(cameraFrame, &camPos, rwCOMBINEPOSTCONCAT);

	return;
}

void CameraRotate(RwCamera* camera, const RwV3d* pos, RwReal angle)
{
	RwV3d invCamPos;
	RwFrame* cameraFrame;
	RwMatrix* cameraMatrix;
	RwV3d camPos;

	cameraFrame = RwCameraGetFrame(camera);
	cameraMatrix = RwFrameGetMatrix(cameraFrame);

	camPos = (pos) ? *pos : *RwMatrixGetPos(cameraMatrix);

	RwV3dScale(&invCamPos, &camPos, -1.0f);

	/*
	 * Translate the camera back to the rotation origin...
	 */
	RwFrameTranslate(cameraFrame, &invCamPos, rwCOMBINEPOSTCONCAT);

	/*
	 * Get the camera's AT vector and use this as the axis of rotation...
	 */
	RwMatrixRotate(cameraMatrix, RwMatrixGetAt(cameraMatrix),
				   angle, rwCOMBINEPOSTCONCAT);

	/*
	 * Translate the camera back to its original position...
	 */
	RwFrameTranslate(cameraFrame, &camPos, rwCOMBINEPOSTCONCAT);

	return;
}


RwReal TotalTilt = 0.0f;


#define LIMITTILT 80

void CameraPoint(RwCamera* camera, RwReal tilt, RwReal turn)
{
	RwFrame* frame;
	RwV3d delta, pos, * right, at;
	RwV3d yAxis = {0.0f, 1.0f, 0.0f};
	frame = (RwCameraGetFrame(camera));

	at = *RwMatrixGetAt(RwFrameGetMatrix(frame));
	/*
	 * Limit the camera's tilt so that it never quite reaches
	 * exactly +90 or -90 degrees...
	 */
	if(TotalTilt + tilt > 1)
	{
		tilt = 1 - TotalTilt;

	}
	else if(TotalTilt + tilt < -LIMITTILT)
	{
		tilt = -LIMITTILT - TotalTilt;
	}

	/*
	 * Use the base-frame to rotate the camera...
	 */
	frame = (RwCameraGetFrame(camera));

	/*
	 * Remember where the camera is...
	 */
	pos = *RwMatrixGetPos(RwFrameGetMatrix(frame));

	/*
	 * Remove the translation component...
	 */
	RwV3dScale(&delta, &pos, -1.0f);
	RwFrameTranslate(frame, &delta, rwCOMBINEPOSTCONCAT);

	/*
	 * Rotate to the new direction...
	 */
	right = RwMatrixGetRight(RwFrameGetMatrix(frame));
	RwFrameRotate(frame, right, -tilt, rwCOMBINEPOSTCONCAT);
	RwFrameRotate(frame, &yAxis, turn, rwCOMBINEPOSTCONCAT);

	/*
	 * Put the camera back to where it was...
	 */
	RwFrameTranslate(frame, &pos, rwCOMBINEPOSTCONCAT);

	return;
}

void TranslateCameraZ(RwCamera* camera, RwReal dist)
{
	RwFrame* frame;
	RwV3d at;

	/*
	 * Move the camera along it's look-at vector the given distance...
	 */

	 /*
	  * Use the base-frame to advance the camera...
	  */
	frame = (RwCameraGetFrame(camera));

	at = *RwMatrixGetAt(RwFrameGetMatrix(frame));

	RwV3dScale(&at, &at, dist);

	RwFrameTranslate(frame, &at, rwCOMBINEPOSTCONCAT);

	return;
}

RwBBox ClumpLocalBoundingBox;
RwBBox ClumpWorldBoundingBox;
RwInt32 WorldSpace = 0;
RwInt32 BoundingBox = 1;

#define NUMPOINTS (100)

#define FASTWORLDBOX
#define FASTWORLDSPHEREx

RpAtomic* AtomicAddVertices(RpAtomic* atomic, void* data)
{
	RpGeometry* geometry;
	RwV3d* vertsIn = NULL, * vertsOut = NULL;
	RwInt32 numVerts, i;
	RwMatrix transform;
	RwBBox* clumpBBox;

	geometry = RpAtomicGetGeometry(atomic);
	numVerts = RpGeometryGetNumVertices(geometry);

	/*
	 * Vertex positions in atomic local-space
	 * (assuming single morph target)...
	 */
	vertsIn = RpMorphTargetGetVertices(RpGeometryGetMorphTarget(geometry, 0));

	/*
	 * An array to hold the transformed vertices...
	 */
	vertsOut = (RwV3d*)RwMalloc(numVerts * sizeof(RwV3d), rwID_NAOBJECT);

	if(WorldSpace)
	{
		/*
		 * Create matrix to transform points to world space ...
		 */
		transform = *RwFrameGetLTM(RpAtomicGetFrame(atomic));
	}
	else
	{
		RwMatrix* atomicLTM, * clumpLTM, invClumpLTM;

		/*
		 * Create matrix to transform points to clump space
		 * (atomic --> world --> clump)...
		 */
		
		atomicLTM = RwFrameGetLTM(RpAtomicGetFrame(atomic));
		if(atomic->clump)
		{
			clumpLTM = RwFrameGetLTM(RpClumpGetFrame(atomic->clump));
			RwMatrixInvert(&invClumpLTM, clumpLTM);
			RwMatrixMultiply(&transform, atomicLTM, &invClumpLTM);
		}
		else
		{
			transform = *RwFrameGetLTM(RpAtomicGetFrame(atomic));
		}
	}

	RwV3dTransformPoints(vertsOut, vertsIn, numVerts, &transform);

	/*
	 * Add to the clump's bounding-box...
	 */
	clumpBBox = (RwBBox*)data;

	for(i = 0; i < numVerts; i++)
	{
		RwBBoxAddPoint(clumpBBox, &vertsOut[i]);
	}

	/*
	 * We're done with this atomic...
	 */
	RwFree(vertsOut);

	return atomic;
}


void ClumpCalculateWorldBoundingBox(RpClump* clump)
{
#ifdef FASTWORLDBOX

	/*
	 * Calculate the world-space box from the clump-space box.
	 * Does not give a tight-fitting box, but is faster...
	 */

	ClumpLocalBoundingBox.sup.x = ClumpLocalBoundingBox.sup.y =
		ClumpLocalBoundingBox.sup.z = -RwRealMAXVAL;

	ClumpLocalBoundingBox.inf.x = ClumpLocalBoundingBox.inf.y =
		ClumpLocalBoundingBox.inf.z = RwRealMAXVAL;

	RpClumpForAllAtomics(clump, AtomicAddVertices, &ClumpLocalBoundingBox);

	RwInt32 i;
	RwV3d worldPoints[8];
	RwMatrix* ltm;

	ltm = RwFrameGetLTM(RpClumpGetFrame(clump));

	for(i = 0; i < 8; i++)
	{
		worldPoints[i].x =
			i & 1 ? ClumpLocalBoundingBox.sup.x : ClumpLocalBoundingBox.inf.x;

		worldPoints[i].y =
			i & 2 ? ClumpLocalBoundingBox.sup.y : ClumpLocalBoundingBox.inf.y;

		worldPoints[i].z =
			i & 4 ? ClumpLocalBoundingBox.sup.z : ClumpLocalBoundingBox.inf.z;
	}

	RwV3dTransformPoints(worldPoints, worldPoints, 8, ltm);

	RwBBoxCalculate(&ClumpWorldBoundingBox, worldPoints, 8);

#else

	/*
	 * Calculate the world-space bounding-box directly from the geometry.
	 * Gives a tight-fitting box, but is slower...
	 */
	ClumpWorldBoundingBox.sup.x = ClumpWorldBoundingBox.sup.y =
		ClumpWorldBoundingBox.sup.z = -RwRealMAXVAL;

	ClumpWorldBoundingBox.inf.x = ClumpWorldBoundingBox.inf.y =
		ClumpWorldBoundingBox.inf.z = RwRealMAXVAL;

	RpClumpForAllAtomics(clump, AtomicAddVertices, &ClumpWorldBoundingBox);

#endif

	return;
}


void AtomicCalculateWorldBoundingBox(RpAtomic* atomic)
{
#ifdef FASTWORLDBOX

	/*
	 * Calculate the world-space box from the clump-space box.
	 * Does not give a tight-fitting box, but is faster...
	 */

	ClumpLocalBoundingBox.sup.x = ClumpLocalBoundingBox.sup.y =
		ClumpLocalBoundingBox.sup.z = -RwRealMAXVAL;

	ClumpLocalBoundingBox.inf.x = ClumpLocalBoundingBox.inf.y =
		ClumpLocalBoundingBox.inf.z = RwRealMAXVAL;

	 AtomicAddVertices(atomic, &ClumpLocalBoundingBox);

	RwInt32 i;
	RwV3d worldPoints[8];
	RwMatrix* ltm;

	ltm  = RwFrameGetLTM(RpAtomicGetFrame(atomic));

	for(i = 0; i < 8; i++)
	{
		worldPoints[i].x =
			i & 1 ? ClumpLocalBoundingBox.sup.x : ClumpLocalBoundingBox.inf.x;

		worldPoints[i].y =
			i & 2 ? ClumpLocalBoundingBox.sup.y : ClumpLocalBoundingBox.inf.y;

		worldPoints[i].z =
			i & 4 ? ClumpLocalBoundingBox.sup.z : ClumpLocalBoundingBox.inf.z;
	}

	RwV3dTransformPoints(worldPoints, worldPoints, 8, ltm);

	RwBBoxCalculate(&ClumpWorldBoundingBox, worldPoints, 8);

#else

	/*
	 * Calculate the world-space bounding-box directly from the geometry.
	 * Gives a tight-fitting box, but is slower...
	 */
	ClumpWorldBoundingBox.sup.x = ClumpWorldBoundingBox.sup.y =
		ClumpWorldBoundingBox.sup.z = -RwRealMAXVAL;

	ClumpWorldBoundingBox.inf.x = ClumpWorldBoundingBox.inf.y =
		ClumpWorldBoundingBox.inf.z = RwRealMAXVAL;

	AtomicAddVertices(atomic, &ClumpWorldBoundingBox);

#endif

	return;
}

void ClumpRenderBoundingBox(RpClump * clump)
{
	RwInt32 i;
	RwIm3DVertex imVertex[8];
	RwMatrix* ltm = NULL;
	RwBBox* bbox = NULL;
	RwImVertexIndex indices[24] =
	{
		0, 1, 1, 3, 3, 2, 2, 0, 4, 5, 5, 7,
		7, 6, 6, 4, 0, 4, 1, 5, 2, 6, 3, 7
	};

	if(WorldSpace)
	{
		bbox = &ClumpWorldBoundingBox;

		ltm = NULL;
	}
	else
	{
		bbox = &ClumpLocalBoundingBox;

		ltm = RwFrameGetLTM(RpClumpGetFrame(clump));
	}

	RwV3dScale(&bbox->sup, &bbox->sup, 2.0);
	RwV3dScale(&bbox->inf, &bbox->inf, 2.0);

	for(i = 0; i < 8; i++)
	{
		RwIm3DVertexSetPos(&imVertex[i],
						   i & 1 ? bbox->sup.x : bbox->inf.x,
						   i & 2 ? bbox->sup.y : bbox->inf.y,
						   i & 4 ? bbox->sup.z : bbox->inf.z
		);

		RwIm3DVertexSetRGBA(&imVertex[i], 196, 196, 0, 255);
	}

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEFLAT);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);

	if(RwIm3DTransform(imVertex, 8, ltm, rwIM3D_ALLOPAQUE))
	{
		RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indices, 24);

		RwIm3DEnd();
	}

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEGOURAUD);

	return;
}

void AtomicRenderBoundingBox(RpAtomic* atomic)
{
	RwInt32 i;
	RwIm3DVertex imVertex[8];
	RwMatrix* ltm = NULL;
	RwBBox* bbox = NULL;
	RwImVertexIndex indices[24] =
	{
		0, 1, 1, 3, 3, 2, 2, 0, 4, 5, 5, 7,
		7, 6, 6, 4, 0, 4, 1, 5, 2, 6, 3, 7
	};

	if(WorldSpace)
	{
		bbox = &ClumpWorldBoundingBox;

		ltm = NULL;
	}
	else
	{
		bbox = &ClumpLocalBoundingBox;

		ltm = RwFrameGetLTM(RpAtomicGetFrame(atomic));
	}

	RwV3dScale(&bbox->sup, &bbox->sup, 2.0);
	RwV3dScale(&bbox->inf, &bbox->inf, 2.0);

	for(i = 0; i < 8; i++)
	{
		RwIm3DVertexSetPos(&imVertex[i],
						   i & 1 ? bbox->sup.x : bbox->inf.x,
						   i & 2 ? bbox->sup.y : bbox->inf.y,
						   i & 4 ? bbox->sup.z : bbox->inf.z
		);

		RwIm3DVertexSetRGBA(&imVertex[i], 196, 196, 0, 255);
	}

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEFLAT);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)NULL);

	if(RwIm3DTransform(imVertex, 8, ltm, rwIM3D_ALLOPAQUE))
	{
		RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indices, 24);

		RwIm3DEnd();
	}

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEGOURAUD);

	return;
}

PlaneIntersectionType testboxplane(XMFLOAT4 plane, CVector Min, CVector Max)
{
	PlaneIntersectionType result;

	Vec3 positiveVertex;
	Vec3 negativeVertex;

	if(plane.x >= 0.0)
	{
		positiveVertex.X = Max.x;
		negativeVertex.X = Min.x;
	}
	else
	{
		positiveVertex.X = Min.x;
		negativeVertex.X = Max.x;
	}

	if(plane.y >= 0.0)
	{
		positiveVertex.Y = Max.y;
		negativeVertex.Y = Min.y;
	}
	else
	{
		positiveVertex.Y = Min.y;
		negativeVertex.Y = Max.y;
	}

	if(plane.z >= 0.0)
	{
		positiveVertex.Z = Max.z;
		negativeVertex.Z = Min.z;
	}
	else
	{
		positiveVertex.Z = Min.z;
		negativeVertex.Z = Max.z;
	}
	float distance = (plane.x * positiveVertex.X + plane.y * positiveVertex.Y + plane.z * positiveVertex.Z + plane.w);
	if(distance < 0.0)
	{
		result = PlaneIntersectionType::BACK;
		return result;
	}
	 distance = (plane.x * negativeVertex.X + plane.y * negativeVertex.Y + plane.z * negativeVertex.Z + plane.w);
	if(distance > 0.0)
	{
		result = PlaneIntersectionType::FRONT;
		return result;
	}




	result = PlaneIntersectionType::INTERSECTING;

	return result;
}

ContainmentType TestAABBPlane(XMFLOAT4* p, CVector min, CVector max)
{
	ContainmentType result;
	bool flag = false;

	for(int i = 0; i < 6; i++)
	{
		switch(testboxplane(p[i], min, max))
		{
			case PlaneIntersectionType::FRONT:
				result = ContainmentType::DISJOINT;
				return result;

			case PlaneIntersectionType::INTERSECTING:
				flag = true;
				break;
		}
	}
	result = flag ? ContainmentType::INTERSECTS : ContainmentType::CONTAINS;
	return result;
}
