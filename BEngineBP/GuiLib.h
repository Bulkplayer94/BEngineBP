#pragma once

#include "ImGui/imgui.h"
#include <d3d11_1.h>
#include "3DMaths.h"
#include "globals.h"

namespace BEngine {
    namespace GuiLib {
        XMFLOAT2 WorldToScreen(const XMFLOAT3& worldPoint, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, float screenWidth, float screenHeight)
        {
            XMMATRIX modelMatrix = viewMatrix * projectionMatrix;

            // Corrected worldPoint.z usage
            XMVECTOR clipPos = XMVector4Transform({ worldPoint.x, worldPoint.y, worldPoint.z, 1.0F }, modelMatrix);

            float clipW = XMVectorGetW(clipPos);

            // Check if w component is zero (to avoid division by zero)
            if (clipW == 0.0f)
            {
                return XMFLOAT2(-1.0f, -1.0f); // Return some invalid position indicating failure
            }

            XMVECTOR ndc = XMVectorDivide(clipPos, XMVectorReplicate(clipW));

            float ndc_x = XMVectorGetX(ndc);
            float ndc_y = XMVectorGetY(ndc);

            float pos_x = (1.0F + ndc_x) * 0.5F * screenWidth;
            float pos_y = (1.0F - ndc_y) * 0.5F * screenHeight;

            return XMFLOAT2(pos_x, pos_y);
        }
    }
}


