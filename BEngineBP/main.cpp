#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")


#include "globals.h"

#include <iostream>
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <assert.h>
#include <stdint.h>
//#include "3DManager.h"
#include "shader/SHADER_default.h"
#include "IMOverlayManager.h"
#include "EntityManager.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_stdlib.h"

#include "PhysXManager.h"

#include "Shader.h"
#include "MeshManager.h"
#include "LuaManager.h"

#include <thread>

FILE* stream;

bool isLoading = true;

void LoadRessources() {
    
    BEngine::meshManager.StartLoading();

    for (unsigned int i = 2; i != 10; ++i) {
        for (unsigned int i2 = 2; i2 != 10; ++i2) {
            int createdEnt = entityManager.RegisterEntity(BEngine::meshManager.meshList["cube"], false);
            Entity* positionedEnt = entityManager.GetEntity(std::abs(createdEnt));
            positionedEnt->SetPosition({ 5.0F * i, 0, 5.0F * i2});
        }
    }

    entityManager.RegisterEntity(BEngine::meshManager.meshList["base_plattform"], true, { 0.0F, -50.0F, 0.0F });
    entityManager.RegisterEntity(BEngine::meshManager.meshList["cube"], false);

    entityManager.RegisterEntity(BEngine::meshManager.meshList["galil"], false, { 0.0F, 0.0F, 0.0F });

    isLoading = false;

    return;

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/)
{
    freopen_s(&stream, "debug.log", "w", stdout);
    Globals::initGlobals(hInstance);

    PhysXManager::SetupPhysX();

    using namespace Globals::Direct3D;
    using namespace Globals::Win32;
    using namespace Globals::CUserCmd;
    using namespace physx;

    std::thread thr(&LoadRessources);
    thr.detach();

    IMOverlayManager imOverlayManager;

    BEngine::LoadAdvancedShaders(d3d11Device);

    SHADER DefaultShades = SHADER_DEFAULT::Load(Globals::Direct3D::d3d11Device);

    // Camera
    float3 cameraPos = { 0.78F, -46.1F, 18.0F };
    float3 cameraFwd = { -0.02F, -0.076F, -1.0F };
    float cameraPitch = 0.f;
    float cameraYaw = 0.f;

    float4x4 perspectiveMat = {};
    Globals::Status::windowStatus[Globals::Status::WindowStatus_RESIZE] = true; // To force initial perspectiveMat calculation

    // Timing
    LONGLONG startPerfCount = 0;
    LONGLONG perfCounterFrequency = 0;
    {
        LARGE_INTEGER perfCount;
        QueryPerformanceCounter(&perfCount);
        startPerfCount = perfCount.QuadPart;
        LARGE_INTEGER perfFreq;
        QueryPerformanceFrequency(&perfFreq);
        perfCounterFrequency = perfFreq.QuadPart;
    }
    long double currentTimeInSeconds = 0.0L;

    std::cout << "Devices" << std::endl << \
        d3d11Device << std::endl << \
        d3d11DeviceContext << std::endl << \
        d3d11SwapChain << std::endl; 

    BEngine::luaManager.Init();

    ImVec2 MousePos = { 0.0F, 0.0F };
    
    // Main Loop
    bool isRunning = true;
    while (isRunning)
    {

        float dt;
        {
            double previousTimeInSeconds = currentTimeInSeconds;
            LARGE_INTEGER perfCount;
            QueryPerformanceCounter(&perfCount);

            currentTimeInSeconds = (double)(perfCount.QuadPart - startPerfCount) / (double)perfCounterFrequency;
            dt = (float)(currentTimeInSeconds - previousTimeInSeconds);
            if (dt > (1.f / 60.f))
                dt = (1.f / 60.f);
        }

        MSG msg = {};
        while (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                isRunning = false;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        //if (isLoading) {
        //    imOverlayManager.Proc();

        //    ImGuiIO& io = ImGui::GetIO();
        //    ImDrawList* bgList = ImGui::GetBackgroundDrawList();

        //    ImVec2 screenCenter(io.DisplaySize.x / 2, io.DisplaySize.y / 2);
        //    ImVec2 textSize = ImGui::CalcTextSize("Loading...");

        //    ImGui::Begin("Hurensohn Loading");
        //    ImGui::Text("BOMBE!");
        //    ImGui::End();

        //    //bgList->AddText(ImVec2(screenCenter.x, screenCenter.y), ImColor(255, 255, 255), "Loading...");
        //    //bgList->AddRectFilled(ImVec2(0, 0), io.DisplaySize, ImColor(100, 100, 100));

        //    imOverlayManager.EndProc();
        //    d3d11SwapChain->Present(1, 0);
        //    continue;
        //}
        
        if (!isLoading) {
            Globals::PhysX::mScene->simulate(dt);
            Globals::PhysX::mScene->fetchResults(true);
        }

        imOverlayManager.Proc();

        static bool mouseWasReleased = true;
        ImVec2 realMouseDrag = { 0,0 };
        {
            POINT mousePoint;
            if (GetCursorPos(&mousePoint)) {

                RECT hwndInfo;
                RECT clientRect;
                if (GetWindowRect(hWnd, &hwndInfo) && GetClientRect(hWnd, &clientRect)) {
                    int HWNDwindowWidth = hwndInfo.left;
                    int HWNDwindowHeight = hwndInfo.top;
                    int _windowWidth = clientRect.right - clientRect.left;
                    int _windowHeight = clientRect.bottom - clientRect.top;

                    ImVec2 newMousePos = { 0.0F,0.0F };

                    //ScreenToClient(hWnd, &mousePoint);

                    newMousePos.x = (float)mousePoint.x;
                    newMousePos.y = (float)mousePoint.y;

                    realMouseDrag = { (HWNDwindowWidth + (_windowWidth / 2))  - newMousePos.x, (HWNDwindowHeight + (_windowHeight / 2)) - newMousePos.y};

                    MousePos = newMousePos;

                    if (!Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED]) {
                        SetCursor(NULL);
                    }
                    else { 
                        HCURSOR SetCursor(LoadCursorW(0, IDC_ARROW));
                    }
                    
                }
            }
        } 

        // Get window dimensions
        int windowWidth, windowHeight;
        float windowAspectRatio;
        {
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);
            windowWidth = clientRect.right - clientRect.left;
            windowHeight = clientRect.bottom - clientRect.top;
            windowAspectRatio = (float)windowWidth / (float)windowHeight;

            if (!Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED] && GetActiveWindow() == hWnd && GetFocus() == hWnd) {
                RECT hwndInfo;
                GetWindowRect(hWnd, &hwndInfo);
                int HWNDwindowWidth = hwndInfo.left;
                int HWNDwindowHeight = hwndInfo.top;
                
                SetCursorPos(HWNDwindowWidth + (windowWidth / 2), HWNDwindowHeight + (windowHeight / 2));
                //std::cout << HWNDwindowWidth << " : " << HWNDwindowHeight << std::endl;
            }
        }

        if (Globals::Status::windowStatus[Globals::Status::WindowStatus_RESIZE] == true)
        {
            d3d11DeviceContext->OMSetRenderTargets(0, 0, 0);
            d3d11FrameBufferView->Release();
            depthBufferView->Release();

            HRESULT res = d3d11SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
            assert(SUCCEEDED(res));

            win32CreateD3D11RenderTargets(d3d11Device, d3d11SwapChain, &d3d11FrameBufferView, &depthBufferView);
            perspectiveMat = makePerspectiveMat(windowAspectRatio, degreesToRadians(84), 0.1f, 1000.f);

            Globals::Status::windowStatus[Globals::Status::WindowStatus_RESIZE] = false;
        } 

        static float mouseSensitivity = 20.0F;

        // Update camera
        if (!Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED])
        {
            float3 camFwdXZ = normalise(float3{ cameraFwd.x, 0, cameraFwd.z });
            float3 cameraRightXZ = cross(camFwdXZ, { 0, 1, 0 });

            float CAM_MOVE_SPEED = 5.f; // in metres per second
            if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
                CAM_MOVE_SPEED *= 4;

            const float CAM_MOVE_AMOUNT = CAM_MOVE_SPEED * dt;
            if (ImGui::IsKeyDown(ImGuiKey_W))
                cameraPos += camFwdXZ * CAM_MOVE_AMOUNT;
            if (ImGui::IsKeyDown(ImGuiKey_S))
                cameraPos -= camFwdXZ * CAM_MOVE_AMOUNT;
            if (ImGui::IsKeyDown(ImGuiKey_A))
                cameraPos -= cameraRightXZ * CAM_MOVE_AMOUNT;
            if (ImGui::IsKeyDown(ImGuiKey_D))
                cameraPos += cameraRightXZ * CAM_MOVE_AMOUNT;
            if (ImGui::IsKeyDown(ImGuiKey_E))
                cameraPos.y += CAM_MOVE_AMOUNT;
            if (ImGui::IsKeyDown(ImGuiKey_Q))
                cameraPos.y -= CAM_MOVE_AMOUNT;

            if (mouseWasReleased == false) {
                mouseWasReleased = true;
            } else {
                cameraYaw += realMouseDrag.x * (mouseSensitivity / 10000);
                cameraPitch += realMouseDrag.y * (mouseSensitivity / 10000);
            }

            // Wrap yaw to avoid floating-point errors if we turn too far
            while (cameraYaw >= 2.0F * (float)M_PI)
                cameraYaw -= 2.0F * (float)M_PI;
            while (cameraYaw <= -2.0F * (float)M_PI)
                cameraYaw += 2.0F * (float)M_PI;

            // Clamp pitch to stop camera flipping upside down
            if (cameraPitch > degreesToRadians(85))
                cameraPitch = degreesToRadians(85);
            if (cameraPitch < -degreesToRadians(85))
                cameraPitch = -degreesToRadians(85);

        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
            Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED] = !Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED];
            mouseWasReleased = true;
        }

        float4x4 viewMat = translationMat(-cameraPos) * rotateYMat(-cameraYaw) * rotateXMat(-cameraPitch);
        cameraFwd = { -viewMat.m[2][0], -viewMat.m[2][1], -viewMat.m[2][2] };

        FLOAT backgroundColor[4] = { 0.1f, 0.2f, 0.6f, 1.0f };
        d3d11DeviceContext->ClearRenderTargetView(d3d11FrameBufferView, backgroundColor);


        d3d11DeviceContext->ClearDepthStencilView(depthBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);

        D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)windowWidth, (FLOAT)windowHeight, 0.0f, 1.0f };
        d3d11DeviceContext->RSSetViewports(1, &viewport);

        d3d11DeviceContext->RSSetState(rasterizerState);
        d3d11DeviceContext->OMSetDepthStencilState(depthStencilState, 0);

        d3d11DeviceContext->OMSetRenderTargets(1, &d3d11FrameBufferView, depthBufferView);

        d3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        d3d11DeviceContext->PSSetSamplers(1, 1, &samplerState);

        if (!isLoading) {
            entityManager.Draw(&DefaultShades, &BEngine::meshManager, &viewMat, &perspectiveMat);
        }

        if (isLoading)
        {
            ImDrawList* drawList = ImGui::GetBackgroundDrawList();
            ImGuiIO& io = ImGui::GetIO();

            ImVec2 textSize = ImGui::CalcTextSize("Loading...");

            drawList->AddRectFilled(ImVec2(0, 0), io.DisplaySize, ImColor(0, 0, 0));
            drawList->AddText(ImVec2((io.DisplaySize.x / 2) - (textSize.x / 2), (io.DisplaySize.y / 2) - (textSize.y / 2)), ImColor(255, 255, 255), "Loading...");
            
        }
        else {
            ImGui::Begin("Camera");
            {
                std::string delta = "DeltaTime: " + std::to_string(dt);
                ImGui::Text(delta.c_str());

                static float smoothFPS = 30;
                smoothFPS -= (smoothFPS - (1 / dt)) * dt * 0.8F;
                std::string FPS = "FPS: " + std::to_string(smoothFPS);
                ImGui::Text(FPS.c_str());

                std::string vecPos = "Pos: " + std::to_string(cameraPos.x) + " " + std::to_string(cameraPos.y) + " " + std::to_string(cameraPos.z);
                ImGui::Text(vecPos.c_str());

                std::string vecFwd = "Fwd: " + std::to_string(cameraFwd.x) + " " + std::to_string(cameraFwd.y) + " " + std::to_string(cameraFwd.z);
                ImGui::Text(vecFwd.c_str());

                std::string vecPitch = "Pit: " + std::to_string(cameraPitch);
                ImGui::Text(vecPitch.c_str());

                std::string vecYaw = "Yaw: " + std::to_string(cameraYaw);
                ImGui::Text(vecYaw.c_str());

                ImGui::SliderFloat("Maus Empfindlichkeit", &mouseSensitivity, 0, 100);

                ImGui::NewLine();

                ImGui::Text("Actor Num Static: %d", (unsigned int)Globals::PhysX::mScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC));
                ImGui::Text("Actor Num Dynamic: %d", (unsigned int)Globals::PhysX::mScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC));

            }
            ImGui::End();
        }



        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        d3d11SwapChain->Present(1, 0);
    }

    return 0;
}
