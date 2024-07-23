#pragma once
#include "3DMaths.h"
#include "globals.h"

namespace BEngine {
    struct CCamera {
        float3 position; // X, Y, Z
        float3 rotation; // Roll, Pitch, Yaw
        float3 forward; // X, Y, Z
        float4x4 viewMat;

        void Frame() {
            viewMat = translationMat(-position) * rotateYMat(-rotation.z) * rotateXMat(-rotation.y);
            forward = { -viewMat.m[2][0], -viewMat.m[2][1], -viewMat.m[2][2] };
        }
    };

    struct CPlayerCamera : CCamera {
        void HandleInput(float sensitivity, float2 mouseDrag) {
            if (Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED])
                return;

            float3 camFwdXZ = normalise(float3{ forward.x, 0, forward.z });
            float3 cameraRightXZ = cross(camFwdXZ, { 0, 1, 0 });

            float CAM_MOVE_SPEED = 5.f; // in metres per second
            if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
                CAM_MOVE_SPEED *= 4;

            const float CAM_MOVE_AMOUNT = CAM_MOVE_SPEED * Globals::Animation::deltaTime;
            if (ImGui::IsKeyDown(ImGuiKey_W))
                position += camFwdXZ * CAM_MOVE_AMOUNT;
            if (ImGui::IsKeyDown(ImGuiKey_S))
                position -= camFwdXZ * CAM_MOVE_AMOUNT;
            if (ImGui::IsKeyDown(ImGuiKey_A))
                position -= cameraRightXZ * CAM_MOVE_AMOUNT;
            if (ImGui::IsKeyDown(ImGuiKey_D))
                position += cameraRightXZ * CAM_MOVE_AMOUNT;
            if (ImGui::IsKeyDown(ImGuiKey_E))
                position.y += CAM_MOVE_AMOUNT;
            if (ImGui::IsKeyDown(ImGuiKey_Q))
                position.y -= CAM_MOVE_AMOUNT;

            rotation.z += mouseDrag.x * (sensitivity / 10000);
            rotation.y += mouseDrag.y * (sensitivity / 10000);

            // Wrap yaw to avoid floating-point errors if we turn too far
            while (rotation.z >= 2.0F * (float)M_PI)
                rotation.z -= 2.0F * (float)M_PI;
            while (rotation.y <= -2.0F * (float)M_PI)
                rotation.y += 2.0F * (float)M_PI;

            // Clamp pitch to stop camera flipping upside down
            if (rotation.y > degreesToRadians(85))
                rotation.y = degreesToRadians(85);
            if (rotation.y < -degreesToRadians(85))
                rotation.y = -degreesToRadians(85);

        }
    } inline playerCamera;
    
	
}