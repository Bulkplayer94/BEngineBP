#include "Win32Manager.h"
#include "assert.h"
#include "ImGui/imgui_impl_win32.h"

using namespace BEngine;

Win32Manager BEngine::win32Manager = {};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
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
		win32Manager.windowResize = true;
		break;
	}
	default:
		result = DefWindowProcW(hWnd, msg, wparam, lparam);
	}
	return result;
}

void Win32Manager::Init(HINSTANCE hInstance)
{
	WNDCLASSEXW winClass = {};
	winClass.cbSize = sizeof(WNDCLASSEXW);
	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = &WndProc;
	winClass.hInstance = hInstance;
	winClass.hIcon = LoadIconW(0, IDI_APPLICATION);
	winClass.hCursor = LoadCursorW(0, IDC_ARROW);
	winClass.lpszClassName = L"BEngineWindowClass";
	winClass.hIconSm = LoadIconW(0, IDI_APPLICATION);

	if (!RegisterClassExW(&winClass)) {
		MessageBoxA(0, "RegisterClassEx failed", "Fatal Error", MB_OK);
		//return GetLastError();
		assert(false && GetLastError());
	}

	RECT initialRect = { 0, 0, 1024, 768 };
	AdjustWindowRectEx(&initialRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);
	LONG initialWidth = initialRect.right - initialRect.left;
	LONG initialHeight = initialRect.bottom - initialRect.top;

	hWnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW,
		winClass.lpszClassName,
		L"BEngine",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		initialWidth,
		initialHeight,
		0, 0, hInstance, 0);

	if (!hWnd) {
		MessageBoxA(0, "CreateWindowEx failed", "Fatal Error", MB_OK);
		assert(false && GetLastError());
	}
}

void Win32Manager::Proc()
{
	MSG msg;
	while (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT)
			isRunning = false;

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	float windowWidth, windowHeight;
	float windowAspectRatio;
	{
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		windowWidth = clientRect.right - clientRect.left;
		windowHeight = clientRect.bottom - clientRect.top;
		windowAspectRatio = (float)windowWidth / (float)windowHeight;

		if (!Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED] && GetActiveWindow() == hWnd && GetFocus() == hWnd) {
			RECT hwndInfo;
			GetWindowRect(hWnd, &hwndInfo);
			int HWNDwindowWidth = hwndInfo.left;
			int HWNDwindowHeight = hwndInfo.top;

			SetCursorPos(HWNDwindowWidth + (windowWidth / 2), HWNDwindowHeight + (windowHeight / 2));
			//std::cout << HWNDwindowWidth << " : " << HWNDwindowHeight << std::endl;
		}
	}
}
