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

float3 TransformCoord(const float3& vec, const float4x4& mat) {
	float3 result;
	// Transformieren des Punktes mit der Matrix
	float w = vec.x * mat.m[3][0] + vec.y * mat.m[3][1] + vec.z * mat.m[3][2] + mat.m[3][3];
	result.x = (vec.x * mat.m[0][0] + vec.y * mat.m[0][1] + vec.z * mat.m[0][2] + mat.m[0][3]) / w;
	result.y = (vec.x * mat.m[1][0] + vec.y * mat.m[1][1] + vec.z * mat.m[1][2] + mat.m[1][3]) / w;
	result.z = (vec.x * mat.m[2][0] + vec.y * mat.m[2][1] + vec.z * mat.m[2][2] + mat.m[2][3]) / w;

	return result;
}

ImVec2 Convert3Dto2D(float3 point, float4x4* viewMat, float4x4* perspectiveMat, D3D11_VIEWPORT* viewPort)
{
	float3 transformedViewSpacePoint = TransformCoord(point, *viewMat);
	float3 transformedProjSpacePoint = TransformCoord(point, *perspectiveMat);

	transformedProjSpacePoint.x /= transformedProjSpacePoint.z;
	transformedProjSpacePoint.y /= transformedProjSpacePoint.z;

	ImVec2 screenSpacePoint;
	screenSpacePoint.x = (transformedProjSpacePoint.x + 1.0f) * 0.5f * viewPort->Width + viewPort->TopLeftX;
	screenSpacePoint.y = (-transformedProjSpacePoint.y + 1.0f) * 0.5f * viewPort->Height + viewPort->TopLeftY;
	//screenSpacePoint.z = transformedProjSpacePoint.z;

	return screenSpacePoint;
}
