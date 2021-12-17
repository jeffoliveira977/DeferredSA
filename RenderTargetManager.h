#pragma once
#include "RenderTarget.h"

class RenderTargetManager
{
public:
	static RenderTarget* Add(RenderTarget* renderTarget);
	static void Remove(RenderTarget* renderTarget);
	static bool Find(RenderTarget* renderTarget);
	static void RemoveAll();
	static void Rebuild();
private:

	static list<RenderTarget*> mRenderTargetList;
};

