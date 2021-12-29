#include "CommonD.h"

#include "ImguiManager.h"

#include "imgui/backends/imgui_impl_rw.h"
#include "imgui/backends/imgui_impl_dx9.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"

#define USE_RWIM3D

void ImguiManager::Initialize()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	ImGui::GetIO().MouseDrawCursor = true;
	ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

	ImGui_ImplWin32_Init(*(HWND*)0x00C8CF88);

#ifndef USE_RWIM3D
	ImGui_ImplDX9_Init(RwD3DDevice);
#endif // USE_RW_IMGUI

}

void ImguiManager::Release()
{
#ifndef USE_RWIM3D
	ImGui_ImplDX9_Shutdown(); 
#endif // USE_RW_IMGUI
}

void ImguiManager::Render()
{}

void ImguiManager::Bind()
{
	ImGui_ImplWin32_NewFrame();

#ifdef USE_RWIM3D
	ImGui_ImplRW_NewFrame();
#else
	ImGui_ImplDX9_NewFrame();
#endif // USE_RWIM3D	
	ImGui::NewFrame();
}

void ImguiManager::Unbind()
{
	ImGui::EndFrame();
	ImGui::Render();

#ifdef USE_RWIM3D
	ImGui_ImplRW_RenderDrawData(ImGui::GetDrawData());
#else
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
#endif // USE_RWIM3D
}
