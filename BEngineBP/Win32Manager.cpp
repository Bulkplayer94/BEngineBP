#include "pch.h"
#include "Win32Manager.h"

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
		Globals::Status::windowStatus[Globals::Status::WindowStatus_RESIZE] = true;
		break;
	}
	default:
		result = DefWindowProcW(hWnd, msg, wparam, lparam);
	}
	return result;
}

bool BEngine::Win32Manager::Initialize(HINSTANCE hInstance) {

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

	RECT initialRect = { 0, 0, 1024, 768 };
	AdjustWindowRectEx(&initialRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);
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

	INT screenWidth = GetSystemMetrics(SM_CXSCREEN);
	INT screenHeight = GetSystemMetrics(SM_CYSCREEN);

	INT posX = (screenWidth - initialWidth) / 2;
	INT posY = (screenHeight - initialHeight) / 2;

	SetWindowPos(m_hWnd, NULL, posX, posY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	while (ShowCursor(FALSE) >= 0);

	SetForegroundWindow(m_hWnd);

	return true;
}

void BEngine::Win32Manager::CheckMessages()
{
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

		if (!Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED] && GetActiveWindow() == BEngine::win32Manager.m_hWnd && GetFocus() == BEngine::win32Manager.m_hWnd) {
			RECT hwndInfo;
			GetWindowRect(BEngine::win32Manager.m_hWnd, &hwndInfo);
			int HWNDwindowWidth = static_cast<int>(hwndInfo.left);
			int HWNDwindowHeight = static_cast<int>(hwndInfo.top);

			SetCursorPos(HWNDwindowWidth + static_cast<int>(m_width / 2), HWNDwindowHeight + static_cast<int>(m_height / 2));
		}
	}

}
