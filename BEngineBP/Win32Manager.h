#pragma once
#include <Windows.h>

namespace BEngine {
	struct Win32Manager {
		HWND hWnd = NULL;
		float windowWidth = 0.0F;
		float windowHeight = 0.0F; 
		float aspectRatio = 0.0F;
		bool windowResize = 0.0F;
		bool isRunning = true;

		void Init(HINSTANCE hInstance);
		void Proc();
	} extern win32Manager;
}
