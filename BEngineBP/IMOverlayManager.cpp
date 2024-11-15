#include "pch.h"
#include "IMOverlayManager.h"
#include "Direct3DManager.h"
#include "Win32Manager.h"

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_win32.h"
#include "ImGui\imgui_impl_dx11.h"

IMOverlayManager::IMOverlayManager() {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(BEngine::win32Manager.m_hWnd);
	ImGui_ImplDX11_Init(BEngine::direct3DManager.m_d3d11Device, BEngine::direct3DManager.m_d3d11DeviceContext);
	ImGui::StyleColorsDark();

}

bool IMOverlayManager::Proc() {

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	return true;

}

bool IMOverlayManager::EndProc() {

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return true;

}

IMOverlayManager::~IMOverlayManager() {

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
}

bool IMOverlayManager::DrawConsole() {

	if (this->IsConsoleVisible) {

	}

	return true;

}

bool IMOverlayManager::DrawDebug() {

	if (this->IsDebugVisible) {

	}

	return true;
}