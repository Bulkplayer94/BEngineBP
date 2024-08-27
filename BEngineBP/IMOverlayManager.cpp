#include "pch.h"
#include "IMOverlayManager.h"

IMOverlayManager::IMOverlayManager() {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(Globals::Win32::hWnd);
	ImGui_ImplDX11_Init(Globals::Direct3D::d3d11Device, Globals::Direct3D::d3d11DeviceContext);
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