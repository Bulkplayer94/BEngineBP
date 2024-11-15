#pragma once

#include "ImGui/imgui.h"
#include <d3d11_1.h>
#include "3DMaths.h"

#include <DirectXMath.h>

using namespace DirectX;

namespace BEngine {
    namespace GuiLib {
        XMFLOAT2 WorldToScreen(const XMFLOAT3& worldPoint, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, float screenWidth, float screenHeight);
    }
}


