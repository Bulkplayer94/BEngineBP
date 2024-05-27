#include "InputManager.h"
#include <Windows.h>
#include "globals.h"

using namespace BEngine;

InputHandler BEngine::inputHandler = {};

void InputHandler::Proc()
{
    static bool mouseWasReleased = true;
    float2 realMouseDrag = { 0,0 };
    {
        POINT mousePoint;
        if (GetCursorPos(&mousePoint)) {

            RECT hwndInfo;
            RECT clientRect;
            if (GetWindowRect(Globals::Win32::hWnd, &hwndInfo) && GetClientRect(Globals::Win32::hWnd, &clientRect)) {
                int HWNDwindowWidth = hwndInfo.left;
                int HWNDwindowHeight = hwndInfo.top;
                int _windowWidth = clientRect.right - clientRect.left;
                int _windowHeight = clientRect.bottom - clientRect.top;

                float2 newMousePos = { 0.0F,0.0F };

                newMousePos.x = (float)mousePoint.x;
                newMousePos.y = (float)mousePoint.y;

                realMouseDrag = { (HWNDwindowWidth + (_windowWidth / 2)) - newMousePos.x, (HWNDwindowHeight + (_windowHeight / 2)) - newMousePos.y };

                mousePos = newMousePos;

                if (!Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED]) {
                    SetCursor(NULL);
                }
                else {
                    SetCursor(LoadCursorW(0, IDC_ARROW));
                }

            }
        }
    }
}
