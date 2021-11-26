#pragma once
#include "CommonD.h"

class SoftParticles
{
	void* VS_softParticles;
	void* PS_softParticles;
public:
	SoftParticles();
	~SoftParticles();

	void hook();
	void initGraphicsBuffer();
	void SetupParams();
};

extern SoftParticles *SoftParticlesContext;
