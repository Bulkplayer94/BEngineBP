#pragma once

#include <Windows.h>

namespace BEngine {
	inline struct Win32Manager {
		HWND m_hWnd = NULL;
		bool m_isRunning = true;
		bool m_wasResized = true;

		float m_width = 0.0F;
		float m_height = 0.0F;
		float m_aspectRatio = 0.0F;

		bool Initialize(HINSTANCE hInstance);
		void CheckMessages();
	} win32Manager;
}