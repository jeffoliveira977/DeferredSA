#pragma once
#include "CommonD.h"
#include "RWCommon.h"

class Raster
{
public:
	static RwBool D3D9RasterLock(void* pixelsIn, void* rasterIn, RwInt32 accessMode);
	static RwBool D3D9RasterUnlock(void* unused1 __RWUNUSED__, void* rasterIn, RwInt32 unused3 __RWUNUSED__);

	static RwInt32 D3D9FindCorrectRasterFormat(RwRasterType type, RwInt32 flags);
	static RwBool D3D9CheckRasterFormat(void* rasterIn, RwInt32 flags);

	static RwUInt32 rwD3D9FindRwFormat(D3DFORMAT d3dFormat);
	static RwBool _rwD3D9CheckValidTextureFormat(RwInt32 format);
	static RwBool _rwD3D9CheckValidCameraTextureFormat(RwInt32 format);
	static RwBool rwD3D9SetRasterFormat(void* rasterIn, RwInt32 flags);
	static RwUInt32 _rwD3D9ImageFindFormat(RwImage* image);
#undef _rwD3D9CheckValidZBufferTextureFormat
	static RwBool _rwD3D9CheckValidZBufferTextureFormat(RwInt32 format);
	static RwBool _rwD3D9CheckValidCubeTextureFormat(RwInt32 format);
	static RwBool _rwD3D9CheckValidCameraCubeTextureFormat(RwInt32 format);
	static RwBool _rwD3D9CheckValidZBufferFormat(RwInt32 format);
	static RwBool _rwD3D9CheckAutoMipmapGenCubeTextureFormat(RwInt32 format);
	static RwBool _rwD3D9CheckAutoMipmapGenTextureFormat(RwInt32 format);
	static RwBool _rwD3D9CheckAutoMipmapGenCameraTextureFormat(RwInt32 format);
	static RwBool _rwD3D9RasterHasAlpha(const RwRaster* raster);
	static RwBool _rwD3D9TextureHasAlpha(const RwTexture* texture);

	static RwBool _rwD3D9NativeTextureRead(void* streamIn, void* textureIn, RwInt32 unused3 __RWUNUSED__);
#undef _rwD3D9CubeRasterCreate
	static RwBool _rwD3D9CubeRasterCreate(RwRaster* raster, RwUInt32 d3dformat, RwInt32 mipmapLevels);
#undef _rwD3D9CubeRasterSelectFace
	static void _rwD3D9CubeRasterSelectFace(RwRaster* raster, RwUInt32 face);
	static RwBool _rwD3D9RasterIsCubeRaster(const RwRaster* raster);
	static void  _rwD3D9CheckRasterSize(RwInt32* width, RwInt32* height, RwUInt32 rasterFlags);
	static RwBool rwD3D9CreateTextureRaster(RwRaster* raster, _rwD3D9RasterExt* rasExt);
	static RwBool _rwD3D9RasterCreate(void* unused1 __RWUNUSED__, void* rasterIn, RwInt32 flags);
	static RwBool rwD3D9CreateZBufferRaster(RwRaster* raster, _rwD3D9RasterExt* rasExt);
	static RwBool rwD3D9CreateCameraTextureRaster(RwRaster* raster, _rwD3D9RasterExt* rasExt);
#undef RwD3D9RasterCreate
	static RwRaster* RwD3D9RasterCreate(RwUInt32 width, RwUInt32 height, RwUInt32 d3dFormat, RwUInt32 flags);
	static RwBool _rwD3D9RasterDestroy(void* unused1 __RWUNUSED__, void* raster, RwInt32 unused3 __RWUNUSED__);

	static void* rwD3D9RasterCtor(void* object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__);
	static void* rwD3D9RasterDtor(void* object, RwInt32 offsetInObject __RWUNUSED__, RwInt32 sizeInObject __RWUNUSED__);
	static RwBool _rwD3D9RasterPluginAttach();
	static void _rwD3D9RasterOpen();
	static void _rwD3D9RasterClose();

	static void rxD3D9VideoMemoryRasterListDestroy();
	static void _rxD3D9VideoMemoryRasterListRelease();
	static RwBool _rxD3D9VideoMemoryRasterListRestore();
	static RwBool rxD3D9VideoMemoryRasterListCreate();
	static void rxD3D9VideoMemoryRasterListRemove(RwRaster* raster);
	static void rxD3D9VideoMemoryRasterListAdd(RwRaster* raster);

	static RwRaster* RwD3D9RasterStreamReadDDS(RwStream* stream);
	static RwTexture* rwD3D9DXTTextureRead(const RwChar* name, const RwChar* fullPathName);
	static RwTexture* RwD3D9DDSTextureRead(const RwChar* name, const RwChar* maskname __RWUNUSED__);
};

