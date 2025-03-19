#include "pch.h"
#include "IMOverlayManager.h"
#include "Direct3DManager.h"
#include "Win32Manager.h"

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_win32.h"
#include "ImGui\imgui_impl_dx11.h"

void BEngine::ImOverlayManager::Initialize() {

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(BEngine::Win32Manager::GetInstance().m_hWnd);
	ImGui_ImplDX11_Init(BEngine::Direct3DManager::GetInstance().m_d3d11Device, BEngine::Direct3DManager::GetInstance().m_d3d11DeviceContext);
	ImGui::StyleColorsDark();

}

bool BEngine::ImOverlayManager::Proc() {

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
			BEngine::Win32Manager::GetInstance().m_isRunning = false;
		}

		ImGui::End();
	}

	return true;

}

bool BEngine::ImOverlayManager::EndProc() {

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return true;

}

void BEngine::ImOverlayManager::Cleanup() {

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
}

bool BEngine::ImOverlayManager::DrawConsole() {

	if (this->IsConsoleVisible) {

	}

	return true;

}

bool BEngine::ImOverlayManager::DrawDebug() {

	if (this->IsDebugVisible) {

	}

	return true;
}