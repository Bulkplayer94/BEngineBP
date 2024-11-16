#pragma once

#include <Windows.h>
#include <DirectXMath.h>

namespace BEngine {

    inline struct MouseManager {
        DirectX::XMFLOAT2 realMouseDrag = DirectX::XMFLOAT2();
        DirectX::XMFLOAT2 MousePos = DirectX::XMFLOAT2();

        void Frame();
        void UpdateMousePosition();
        void HandleMouseState();
        void LockCursor();
        void UnlockCursor();

        DirectX::XMFLOAT2 GetRealMouseDrag() const { return realMouseDrag; }
        DirectX::XMFLOAT2 GetMousePos() const { return MousePos; }
    } mouseManager;

}
