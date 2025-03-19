#include "pch.h"
#include "Win32Manager.h"
#include "SettingsManager.h"

#include "ImGui/imgui_impl_win32.h"
#include "globals.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wparam, lparam);

	LRESULT result = 0;
	switch (msg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	case WM_SIZE:
	{
		if (BEngine::SettingsManager::GetInstance().getSetting("Display Mode")->dropdownIndex == 0) {
			Globals::Status::windowStatus[Globals::Status::WindowStatus_RESIZE] = true;
		}
		else {
			return 1;
		}
		
		break;
	}
	case WM_INPUT:
	{
		UINT dwSize;

		GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
		LPBYTE lpb = new BYTE[dwSize];

		if (lpb == nullptr)
			break;

		if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
			delete[] lpb;
			break;
		}

		RAWINPUT* raw = (RAWINPUT*)lpb;

		if (raw->header.dwType == RIM_TYPEMOUSE) {
			RAWMOUSE* rawMouse = &raw->data.mouse;

			BEngine::Win32Manager::GetInstance().m_mouseDragX -= rawMouse->lLastX;
			BEngine::Win32Manager::GetInstance().m_mouseDragY -= rawMouse->lLastY;
		}

		delete[] lpb;
		break;
	}
	default:
		result = DefWindowProcW(hWnd, msg, wparam, lparam);
	}
	return result;
}

bool BEngine::Win32Manager::Initialize(HINSTANCE hInstance) {

	if (!BEngine::SettingsManager::GetInstance().isSettingRegistered("Display Mode")) {
		BEngine::SettingsManager::Setting fullscreen;
		fullscreen.type = BEngine::SettingsManager::DROPDOWN;
		fullscreen.name = "Display Mode";
		fullscreen.category = "Display";
		fullscreen.dropdownIndex = 0;
		fullscreen.dropdownOptions.push_back("Window"); // 0 : Normal Window
		fullscreen.dropdownOptions.push_back("Borderless Window"); // 1 : Window without Titlebar and on Desktop Size
		fullscreen.dropdownOptions.push_back("Fullscreen"); // 2 : Real Direct3D11 Fullscreen

		BEngine::SettingsManager::GetInstance().registerSetting(fullscreen);
	}

	WNDCLASSEXW winClass = {};
	winClass.cbSize = sizeof(WNDCLASSEXW);
	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = &WindowProc;
	winClass.hInstance = hInstance;
	//winClass.hIcon = LoadIconW(0, MAKEINTRESOURCE(IDI_ICON1));
	winClass.hCursor = NULL;
	winClass.lpszClassName = L"BEngineBP Window";
	//winClass.hIconSm = LoadIconW(0, MAKEINTRESOURCE(IDI_ICON1));

	if (!RegisterClassExW(&winClass)) {
		MessageBoxA(0, "RegisterClassEx failed", "Fatal Error", MB_OK);
		exit(GetLastError());
	}

	INT screenWidth = GetSystemMetrics(SM_CXSCREEN);
	INT screenHeight = GetSystemMetrics(SM_CYSCREEN);

	BEngine::SettingsManager::Setting* fullscreenSetting = BEngine::SettingsManager::GetInstance().getSetting("Display Mode");


	RECT initialRect = { 0, 0, 1024, 768 };
	if (fullscreenSetting->dropdownIndex == 0 || fullscreenSetting->dropdownIndex == 2) {
		initialRect = { 0, 0, 1024, 768 };
		AdjustWindowRectEx(&initialRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);

		m_width = 1024;
		m_height = 768;
	}
	else if (fullscreenSetting->dropdownIndex == 1) {
		initialRect = { 0, 0, screenWidth, screenHeight };
		AdjustWindowRectEx(&initialRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);
	
		m_width = screenWidth;
		m_height = screenHeight;
	}
	
	LONG initialWidth = initialRect.right - initialRect.left;
	LONG initialHeight = initialRect.bottom - initialRect.top;

	m_hWnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW,
		winClass.lpszClassName,
		L"BEngineBP",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		initialWidth,
		initialHeight,
		0, 0, hInstance, 0);

	if (!m_hWnd) {
		MessageBoxA(0, "CreateWindowEx failed", "Fatal Error", MB_OK);
		exit(GetLastError());
	}

	if (fullscreenSetting->dropdownIndex == 1 || fullscreenSetting->dropdownIndex == 2) {
		SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
		SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW);

		SetWindowPos(m_hWnd, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_FRAMECHANGED);

		SetForegroundWindow(m_hWnd);
		ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);

		if (fullscreenSetting->dropdownIndex == 2) {
			OutputDebugStringA("[BEngineBP] The D3D11 Fullscreen Mode is not Implemented, falling back to Fullscreen Window\n");
		}
	}
	else {
		INT posX = (screenWidth - initialWidth) / 2;
		INT posY = (screenHeight - initialHeight) / 2;

		SetWindowPos(m_hWnd, NULL, posX, posY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
		SetForegroundWindow(m_hWnd);
	}

	while (ShowCursor(FALSE) >= 0);

	RAWINPUTDEVICE mouseDeviceDesc;
	mouseDeviceDesc.usUsagePage = 0x01;
	mouseDeviceDesc.usUsage = 0x02;
	mouseDeviceDesc.dwFlags = RIDEV_INPUTSINK;
	mouseDeviceDesc.hwndTarget = m_hWnd;

	if (!RegisterRawInputDevices(&mouseDeviceDesc, 1, sizeof(RAWINPUTDEVICE)))
		assert(false);

	return true;
}

void BEngine::Win32Manager::CheckMessages()
{
	{
		m_mouseDragX = 0.0F;
		m_mouseDragY = 0.0F;
	}

	MSG msg = {};
	while (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			m_isRunning = false;
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	{
		RECT clientRect;
		GetClientRect(m_hWnd, &clientRect);

		m_width = static_cast<float>(clientRect.right - clientRect.left);
		m_height = static_cast<float>(clientRect.bottom - clientRect.top);
		m_aspectRatio = m_width / m_height;

		if (!Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED] && GetActiveWindow() == BEngine::Win32Manager::GetInstance().m_hWnd && GetFocus() == BEngine::Win32Manager::GetInstance().m_hWnd) {
			RECT hwndInfo;
			GetWindowRect(BEngine::Win32Manager::GetInstance().m_hWnd, &hwndInfo);
			int HWNDwindowWidth = static_cast<int>(hwndInfo.left);
			int HWNDwindowHeight = static_cast<int>(hwndInfo.top);

			SetCursorPos(HWNDwindowWidth + static_cast<int>(m_width / 2), HWNDwindowHeight + static_cast<int>(m_height / 2));
		}
	}

	if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
		if (Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED]) {
			LockMouse();
		}
		else {
			UnlockMouse();
		}

		Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED] =
			!Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED];
	}
}

void BEngine::Win32Manager::LockMouse() {
	SetCursor(NULL);
	while (ShowCursor(FALSE) >= 0);

	RECT hwndInfo;
	GetWindowRect(BEngine::Win32Manager::GetInstance().m_hWnd, &hwndInfo);
	int HWNDwindowWidth = static_cast<int>(hwndInfo.left);
	int HWNDwindowHeight = static_cast<int>(hwndInfo.top);

	SetCursorPos(HWNDwindowWidth + static_cast<int>(BEngine::Win32Manager::GetInstance().m_width / 2),
		HWNDwindowHeight + static_cast<int>(BEngine::Win32Manager::GetInstance().m_height / 2));
}

void BEngine::Win32Manager::UnlockMouse() {
	SetCursor(LoadCursorW(NULL, IDC_ARROW));
	while (ShowCursor(TRUE) < 0);
}