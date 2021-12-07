#pragma once

#include "imgui.h"


class ImguiManager
{
public:
	static void Initialize();
	static void Release();
	static void Render();

	static void Bind();
	static void Unbind();
};

