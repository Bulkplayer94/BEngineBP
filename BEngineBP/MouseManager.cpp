#include "pch.h"
#include "MouseManager.h"
#include "win32Manager.h"
#include "globals.h"
#include "ImGui\imgui.h"

void BEngine::MouseManager::Frame()
{
    UpdateMousePosition();
    HandleMouseState();
}

void BEngine::MouseManager::UpdateMousePosition() {
    POINT mousePoint;
    if (GetCursorPos(&mousePoint)) {
        RECT hwndInfo;
        RECT clientRect;
        if (GetWindowRect(BEngine::win32Manager.m_hWnd, &hwndInfo) &&
            GetClientRect(BEngine::win32Manager.m_hWnd, &clientRect)) {

            int HWNDwindowWidth = hwndInfo.left;
            int HWNDwindowHeight = hwndInfo.top;
            int windowWidth = clientRect.right - clientRect.left;
            int windowHeight = clientRect.bottom - clientRect.top;

            DirectX::XMFLOAT2 newMousePos = { static_cast<float>(mousePoint.x), static_cast<float>(mousePoint.y) };

            realMouseDrag = {
                (HWNDwindowWidth + (windowWidth / 2)) - newMousePos.x,
                (HWNDwindowHeight + (windowHeight / 2)) - newMousePos.y
            };

            MousePos = DirectX::XMFLOAT2(newMousePos.x, newMousePos.y);
        }
    }
}

void BEngine::MouseManager::HandleMouseState() {
    if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
        if (Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED]) {
            LockCursor();
        }
        else {
            UnlockCursor();
        }

        Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED] =
            !Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED];
    }
}

void BEngine::MouseManager::LockCursor() {
    SetCursor(NULL);
    while (ShowCursor(FALSE) >= 0);

    RECT hwndInfo;
    GetWindowRect(BEngine::win32Manager.m_hWnd, &hwndInfo);
    int HWNDwindowWidth = static_cast<int>(hwndInfo.left);
    int HWNDwindowHeight = static_cast<int>(hwndInfo.top);

    SetCursorPos(HWNDwindowWidth + static_cast<int>(BEngine::win32Manager.m_width / 2),
        HWNDwindowHeight + static_cast<int>(BEngine::win32Manager.m_height / 2));
}

void BEngine::MouseManager::UnlockCursor() {
    SetCursor(LoadCursorW(NULL, IDC_ARROW));
    while (ShowCursor(TRUE) < 0);
}
