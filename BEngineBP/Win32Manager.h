#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace BEngine {
	struct Win32Manager {
		static Win32Manager& GetInstance() {
			static Win32Manager instance;
			return instance;
		}

		HWND m_hWnd = NULL;
		bool m_isRunning = true;
		bool m_wasResized = true;
		
		float m_width = 0.0F;
		float m_height = 0.0F;
		float m_aspectRatio = 0.0F;
		float m_mouseDragX = 0.0F;
		float m_mouseDragY = 0.0F;
		
		bool Initialize(HINSTANCE hInstance);
		void CheckMessages();
		void LockMouse();
		void UnlockMouse();
	};
}