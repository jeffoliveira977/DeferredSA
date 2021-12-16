#include "RenderTargetManager.h"

list<RenderTarget*> RenderTargetManager::mRenderTargetList;

RenderTarget* RenderTargetManager::Add(RenderTarget* renderTarget)
{
	if(!Find(renderTarget))
		mRenderTargetList.push_back(renderTarget);
	
	return renderTarget;
}

void RenderTargetManager::Remove(RenderTarget* renderTarget)
{
	mRenderTargetList.remove(renderTarget);
}

bool RenderTargetManager::Find(RenderTarget* renderTarget)
{
	auto it = std::find(mRenderTargetList.begin(), mRenderTargetList.end(), renderTarget);
	return it != mRenderTargetList.end();
}

void RenderTargetManager::RemoveAll()
{
	for(auto& item : mRenderTargetList)
	{
		SAFE_RELEASE(item);
	}

	mRenderTargetList.clear();
}

void RenderTargetManager::Rebuild()
{
	D3DDISPLAYMODE display;
	RwD3DDevice->GetDisplayMode(0, &display);

	if(RsGlobal.maximumWidth != display.Width ||
	   RsGlobal.maximumHeight != display.Height)
	{
		for(auto& item : mRenderTargetList)
		{
			if(item)
			{
				item->Release();
				item->Initialize();
			}
		}
	}
}
