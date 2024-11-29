#include "pch.h"
#include "IMOverlayManager.h"
#include "Direct3DManager.h"
#include "Win32Manager.h"

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_win32.h"
#include "ImGui\imgui_impl_dx11.h"

void BEngine::IMOverlayManager::Initialize() {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(BEngine::win32Manager.m_hWnd);
	ImGui_ImplDX11_Init(BEngine::direct3DManager.m_d3d11Device, BEngine::direct3DManager.m_d3d11DeviceContext);
	ImGui::StyleColorsDark();

}

bool BEngine::IMOverlayManager::Proc() {

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED]) {
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowSize(ImVec2(200, 400));
		ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x / 2) - 100, (io.DisplaySize.y / 2) - 200));

		ImGui::Begin("Pause Menu", nullptr, ImGuiWindowFlags_NoResize | 
			ImGuiWindowFlags_NoMove | 
			ImGuiWindowFlags_NoCollapse | 
			ImGuiWindowFlags_NoDecoration);

		if (ImGui::Button("Continue")) {
			Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED] = false;
			ImGui::SetWindowFocus(NULL);
		}

		ImDrawList* windowDrawList = ImGui::GetWindowDrawList();

		if (ImGui::Button("Settings")) {
			
		}

		if (ImGui::Button("Exit")) {
			BEngine::win32Manager.m_isRunning = false;
		}

		ImGui::End();
	}

	return true;

}

bool BEngine::IMOverlayManager::EndProc() {

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return true;

}

void BEngine::IMOverlayManager::Cleanup() {

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
}

bool BEngine::IMOverlayManager::DrawConsole() {

	if (this->IsConsoleVisible) {

	}

	return true;

}

bool BEngine::IMOverlayManager::DrawDebug() {

	if (this->IsDebugVisible) {

	}

	return true;
}