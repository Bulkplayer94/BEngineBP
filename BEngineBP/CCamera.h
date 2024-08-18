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

        void Frame() {
            // Create translation and rotation matrices
            XMMATRIX translationMat = XMMatrixTranslation(-position.x, -position.y, -position.z);
            XMMATRIX rotationMat = XMMatrixRotationY(-rotation.z) * XMMatrixRotationX(-rotation.y);

            // Combine translation and rotation
            viewMat = translationMat * rotationMat;
            viewMat = XMMatrixTranspose(viewMat);

            // Extract the forward vector from the view matrix
            XMVECTOR forwardVec = XMVectorSet(-viewMat.r[2].m128_f32[0], -viewMat.r[2].m128_f32[1], -viewMat.r[2].m128_f32[2], 0.0f);;
            XMStoreFloat3(&forward, forwardVec);
        }
    };

    struct CPlayerCamera : CCamera {
        void HandleInput(float sensitivity, XMFLOAT2 mouseDrag) {
            if (Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED])
                return;

            XMVECTOR camPos = XMLoadFloat3(&position);

            XMFLOAT3 camFwdXZ = { forward.x, 0, forward.z };
            XMFLOAT3 cameraRightXZ;
            XMVECTOR fwd = XMLoadFloat3(&camFwdXZ);
            XMVECTOR up = XMVectorSet(0, 1, 0, 0);
            XMVECTOR right = XMVector3Cross(fwd, up);
            XMStoreFloat3(&cameraRightXZ, right);

            XMVECTOR fwdNormalized = XMVector3Normalize(fwd);
            XMVECTOR rightNormalized = XMVector3Normalize(XMLoadFloat3(&cameraRightXZ));

            float CAM_MOVE_SPEED = 5.f; // in metres per second
            if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
                CAM_MOVE_SPEED *= 4;

            float CAM_MOVE_AMOUNT = CAM_MOVE_SPEED * Globals::Animation::deltaTime;
            if (ImGui::IsKeyDown(ImGuiKey_W))
                camPos = XMVectorAdd(XMLoadFloat3(&position), XMVectorScale(fwdNormalized, CAM_MOVE_AMOUNT));
            if (ImGui::IsKeyDown(ImGuiKey_S))
                camPos = XMVectorSubtract(XMLoadFloat3(&position), XMVectorScale(fwdNormalized, CAM_MOVE_AMOUNT));
            if (ImGui::IsKeyDown(ImGuiKey_A))
                camPos = XMVectorSubtract(XMLoadFloat3(&position), XMVectorScale(rightNormalized, CAM_MOVE_AMOUNT));
            if (ImGui::IsKeyDown(ImGuiKey_D))
                camPos = XMVectorAdd(XMLoadFloat3(&position), XMVectorScale(rightNormalized, CAM_MOVE_AMOUNT));
            if (ImGui::IsKeyDown(ImGuiKey_E))
                camPos += XMVectorSet(0, CAM_MOVE_AMOUNT, 0, 0);
            if (ImGui::IsKeyDown(ImGuiKey_Q))
                camPos -= XMVectorSet(0, CAM_MOVE_AMOUNT, 0, 0);

            rotation.z += mouseDrag.x * (sensitivity / 10000);
            rotation.y += mouseDrag.y * (sensitivity / 10000);

            // Wrap yaw to avoid floating-point errors if we turn too far
            while (rotation.z >= XM_2PI)
                rotation.z -= XM_2PI;
            while (rotation.z <= -XM_2PI)
                rotation.z += XM_2PI;

            // Clamp pitch to stop camera flipping upside down
            if (rotation.y > XMConvertToRadians(85))
                rotation.y = XMConvertToRadians(85);
            if (rotation.y < -XMConvertToRadians(85))
                rotation.y = -XMConvertToRadians(85);

            XMStoreFloat3(&position, camPos);
        }
    } inline playerCamera;
}
