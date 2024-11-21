#pragma once
#include <DirectXMath.h>
#include "globals.h"

using namespace DirectX;

namespace BEngine {
    struct CCamera {
        XMFLOAT3 position; // X, Y, Z
        XMFLOAT3 rotation; // Roll, Pitch, Yaw
        XMFLOAT3 forward;  // X, Y, Z
        XMMATRIX viewMat;

        
        void Frame();
    };

    struct CPlayerCamera : CCamera {
        void Initialize();
        void HandleInput(XMFLOAT2 mouseDrag);
    } inline playerCamera;
}
