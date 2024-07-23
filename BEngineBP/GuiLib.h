#pragma once

#include "ImGui/imgui.h"
#include <d3d11_1.h>
#include "3DMaths.h"
#include "globals.h"

namespace BEngine {
    namespace GuiLib {
        float2 project3Dto2D(const float3& point, const float4x4& viewMat, const float4x4& projMat, float screenWidth, float screenHeight) {

            const float4x4& modelMat = createIdentityMatrix();

            const float4x4& transformationMat = modelMat * viewMat * projMat;

            const float4& homogeneousPoint = { point.x, point.y, point.z, 1.0f };

            const float4& screenPoint = homogeneousPoint * transformationMat;

            const float invW = 1.0f / screenPoint.w;
            if (invW < 0.001F) {
                return { -1.0F, -1.0F };
            }
            float2 screenPosition = { screenPoint.x * invW, screenPoint.y * invW };

            screenPosition.x = (screenPosition.x + 1.0f) * 0.5f * screenWidth;
            screenPosition.y = (1.0f - screenPosition.y) * 0.5f * screenHeight;

            return screenPosition;
        }
    }
}


