#pragma once

#include "imgui/imgui.h"


class ImguiManager
{
public:
	static void Initialize();
	static void Release();
	static void Render();

	static void Bind();
	static void Unbind();
};

