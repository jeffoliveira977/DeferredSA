#pragma once
#include "CommonD.h"

class SamplerState
{
public:
    uint32_t mAddressU;
    uint32_t mAddressV;
    uint32_t mAddressW;
    uint32_t mBorderColor;
    uint32_t mMagFilter;
    uint32_t mMinFilter;
    uint32_t mMipFilter;
    float mMipmapLodBias;
    uint32_t mMaxMipLevel;
    uint32_t mMaxAnisotropy;
 
	SamplerState();
	~SamplerState();

	void Apply(uint32_t stage);
};

