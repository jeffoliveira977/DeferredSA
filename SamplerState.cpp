#include "SamplerState.h"

SamplerState::SamplerState()
{
	mMagFilter = D3DTEXF_LINEAR;
	mMinFilter = D3DTEXF_LINEAR;
	mMipFilter = D3DTEXF_LINEAR;
	mAddressU = D3DTADDRESS_WRAP;
	mAddressV = D3DTADDRESS_WRAP;
	mAddressW = D3DTADDRESS_WRAP;
	mBorderColor = D3DCOLOR_RGBA(255, 255, 255, 255);
	mMipmapLodBias = 0.0f;
	mMaxMipLevel = 0;
	mMaxAnisotropy = 0;
}

SamplerState::~SamplerState()
{
}

void SamplerState::Apply(uint32_t stage)
{
	rwD3D9SetSamplerState(stage, D3DSAMP_MAGFILTER, mMagFilter);
	rwD3D9SetSamplerState(stage, D3DSAMP_MINFILTER, mMinFilter);
	rwD3D9SetSamplerState(stage, D3DSAMP_MIPFILTER, mMipFilter);
	rwD3D9SetSamplerState(stage, D3DSAMP_ADDRESSU, mAddressU);
	rwD3D9SetSamplerState(stage, D3DSAMP_ADDRESSV, mAddressV);
	rwD3D9SetSamplerState(stage, D3DSAMP_ADDRESSW, mAddressW);
	rwD3D9SetSamplerState(stage, D3DSAMP_BORDERCOLOR, mBorderColor);
	rwD3D9SetSamplerState(stage, D3DSAMP_MIPMAPLODBIAS, mMipmapLodBias);
	rwD3D9SetSamplerState(stage, D3DSAMP_MAXMIPLEVEL, mMaxMipLevel);
	rwD3D9SetSamplerState(stage, D3DSAMP_MAXANISOTROPY, mMaxAnisotropy);
}
