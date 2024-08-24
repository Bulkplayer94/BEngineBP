#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include "pch.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "globals.h"
#include "GuiLib.h"

#include <iostream>
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <assert.h>
#include <stdint.h>

#include "IMOverlayManager.h"
#include "EntityManager.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_stdlib.h"

#include "PhysXManager.h"

#include "MeshManager.h"
#include "LuaManager.h"
#include "ShaderManager.h"

#include "ErrorReporter.h"
#include "CCamera.h"

#include "ImMenus.h"

#include <chrono>
#include <thread>
#include <atomic>

FILE* stream;

std::atomic<bool> isLoading = true;

using namespace physx;

class ExplosionCallback : public PxSweepCallback {
public:
    ExplosionCallback(PxSweepHit* hits, PxU32 maxNbHits)
        : PxSweepCallback(hits, maxNbHits) {}

    virtual PxAgain processTouches(const PxSweepHit* buffer, PxU32 nbHits) override {
        for (PxU32 i = 0; i < nbHits; ++i) {
            const PxSweepHit& hit = buffer[i];
           
            PxTransform actorPos = hit.actor->getGlobalPose();

            PxRigidDynamic* act = (PxRigidDynamic*)hit.actor;

            PxVec3 force = actorPos.p - explosionCenter;
            force *= (500 / (force.magnitude() + 1));

            act->addForce(force, PxForceMode::eIMPULSE);

        }
        return true;
    }

    PxVec3 explosionCenter;
    
};

void performExplosion(PxScene* scene, const PxVec3& explosionCenter, float explosionStrength, float explosionRadius) {
    PxSphereGeometry sphereGeom(explosionRadius);

    const PxU32 maxNbHits = 256;
    PxSweepHit hitBuffer[maxNbHits];

    ExplosionCallback sweepCallback(hitBuffer, maxNbHits);
    sweepCallback.explosionCenter = explosionCenter;

    PxQueryFilterData filterData;
    filterData.flags = PxQueryFlag::eDYNAMIC;

    scene->sweep(sphereGeom, PxTransform(explosionCenter), PxVec3(1.0f, 0.0f, 0.0f), 0, sweepCallback, PxHitFlag::eDEFAULT, filterData);
}

void LoadRessources() {

    BEngine::shaderManager.StartLoading();
    BEngine::meshManager.StartLoading();

    BEngine::Model* model = BEngine::meshManager.meshList["welt"];
    std::cout << "Welt Bounding Box X,Y,Z:\n"
        << model->boundingBox[0].x << " " << model->boundingBox[0].y << " " << model->boundingBox[0].z << "\n"
        << model->boundingBox[1].x << " " << model->boundingBox[1].y << " " << model->boundingBox[1].z << std::endl;

    //Entity* welt = entityManager.RegisterEntity(BEngine::meshManager.meshList["welt"], { 0.0F, 0.0F, 0.0F });
    //XMFLOAT3 weltRotation = welt->GetRotation();
    //welt->SetRotation({ weltRotation.y + XMConvertToRadians(90.0F), 0.0F, 0.0F });

    unsigned int cubeCount = 10;
    float startPos = 0.F - (cubeCount / 2);

    float spacing = 7.5f; // Abstand zwischen den Würfeln

    for (unsigned int I1 = 0; I1 < cubeCount; ++I1) {
        for (unsigned int I2 = 0; I2 < cubeCount; ++I2) {
            for (unsigned int I3 = 0; I3 < cubeCount; ++I3) {
                float currPosX = startPos + I1 * spacing;
                float currPosY = startPos + I2 * spacing;
                float currPosZ = startPos + I3 * spacing;

                entityManager.RegisterEntity(BEngine::meshManager.meshList["cube"], { currPosX, currPosY, currPosZ });
            }
        }
    }
    

    isLoading = false;

    return;

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/)
{
    freopen_s(&stream, "debug.log", "w", stdout);
    Globals::initGlobals(hInstance);

    AllocConsole();
    freopen_s(&stream, "conout$", "w", stdout);

    PhysXManager::SetupPhysX();

    using namespace Globals::Direct3D;
    using namespace Globals::Win32;
    using namespace Globals::CUserCmd;
    using namespace physx;

    std::thread thr(&LoadRessources);
    thr.detach();

    IMOverlayManager imOverlayManager;

    XMMATRIX perspectiveMat = {};
    Globals::Status::windowStatus[Globals::Status::WindowStatus_RESIZE] = true;

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
            //if (dt > (1.f / 60.f))
            //    dt = (1.f / 60.f);

            Globals::Animation::deltaTime = dt;
            Globals::Animation::currTime = currentTimeInSeconds;
        }
        
        MSG msg = {};
        while (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                isRunning = false;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        auto start = std::chrono::high_resolution_clock::now();
        if (!isLoading) {
            Globals::PhysX::mScene->simulate(dt);
            Globals::PhysX::mScene->fetchResults(true);
        }
        long long mögliche_leistung = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();

        XMFLOAT3 eyeTracePos = { 0.0F, 0.0F, 0.0F };
        if (!isLoading) {
            using namespace BEngine;

            PxVec3 origin = { playerCamera.position.x, playerCamera.position.y, playerCamera.position.z };
            PxVec3 unitDir = { playerCamera.forward.x, playerCamera.forward.y, playerCamera.forward.z };
            PxReal maxDistance = 500.0F;
            PxRaycastBuffer hit;

            unitDir.normalize();

            bool status = Globals::PhysX::mScene->raycast(origin, unitDir, maxDistance, hit);
            if (status) {
                eyeTracePos = { hit.block.position.x, hit.block.position.y, hit.block.position.z };

                if (ImGui::IsKeyPressed(ImGuiKey_MouseLeft, false) && (hit.block.actor->getType() == physx::PxActorType::eRIGID_DYNAMIC)) {
                    PxRigidDynamic* act = (PxRigidDynamic*)hit.block.actor;

                    PxVec3 forceDir = hit.block.position - act->getGlobalPose().p;
                    forceDir.normalize();
                    forceDir *= -1;

                    PxReal forceMagnitude = 2500.0f;
                    act->addForce(forceDir * forceMagnitude, physx::PxForceMode::eIMPULSE);
                }

                if (ImGui::IsKeyPressed(ImGuiKey_MouseRight, false)) {
                    PxVec3 explosionCenter = { eyeTracePos.x, eyeTracePos.y, eyeTracePos.z };
                    float explosionStrength = 2500.0f;
                    float explosionRadius = 50.0f;

                    performExplosion(Globals::PhysX::mScene, explosionCenter, explosionStrength, explosionRadius);
                }
            }

            if (ImGui::IsKeyPressed(ImGuiKey_P)) {
                Entity* entity = entityManager.RegisterEntity(BEngine::meshManager.meshList["cube"]);
                PxTransform trans = entity->physicsActor->getGlobalPose();

                trans.p.x = playerCamera.position.x;
                trans.p.y = playerCamera.position.y;
                trans.p.z = playerCamera.position.z;

                entity->physicsActor->setGlobalPose(trans);

                PxRigidDynamic* dyn = (PxRigidDynamic*)entity->physicsActor;
                dyn->addForce(unitDir * 1000.0F, physx::PxForceMode::eIMPULSE);
            }

            if (ImGui::IsKeyPressed(ImGuiKey_1)) {
                Entity* spawned_ent = entityManager.RegisterEntity(BEngine::meshManager.meshList["ball"], { playerCamera.position.x, playerCamera.position.y, playerCamera.position.z });
            }

            Globals::PhysX::mPlayerController->move(PxVec3(0.0F, 0.0F, 2.0F), 1.0F, dt, PxControllerFilters());
        }

        imOverlayManager.Proc();

        XMFLOAT2 realMouseDrag = { 0,0 };
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
                }
            }
        } 

        // Get window dimensions
        float windowWidth, windowHeight;
        float windowAspectRatio;
        {   
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);
            windowWidth = static_cast<float>(clientRect.right - clientRect.left);
            windowHeight = static_cast<float>(clientRect.bottom - clientRect.top);
            windowAspectRatio = windowWidth / windowHeight;

            if (!Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED] && GetActiveWindow() == hWnd && GetFocus() == hWnd) {
                RECT hwndInfo;
                GetWindowRect(hWnd, &hwndInfo);
                int HWNDwindowWidth = static_cast<int>(hwndInfo.left);
                int HWNDwindowHeight = static_cast<int>(hwndInfo.top);

                SetCursorPos(HWNDwindowWidth + static_cast<int>(windowWidth / 2), HWNDwindowHeight + static_cast<int>(windowHeight / 2));
            }
        }

        // Handling Mouse Freeing and Aquiring
        if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
            if (Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED])
            {
                SetCursor(NULL);
                while (ShowCursor(FALSE) >= 0); // Cursor sicher verstecken

                RECT hwndInfo;
                GetWindowRect(hWnd, &hwndInfo);
                int HWNDwindowWidth = static_cast<int>(hwndInfo.left);
                int HWNDwindowHeight = static_cast<int>(hwndInfo.top);

                SetCursorPos(HWNDwindowWidth + static_cast<int>(windowWidth / 2), HWNDwindowHeight + static_cast<int>(windowHeight / 2));
            }
            else {
                SetCursor(LoadCursorW(NULL, IDC_ARROW));
                while (ShowCursor(TRUE) < 0); // Cursor sicher anzeigen
            }

            Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED] = !Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED];
        }

        static float fov = 84.0F;
        if (Globals::Status::windowStatus[Globals::Status::WindowStatus_RESIZE] == true)
        {
            d3d11DeviceContext->OMSetRenderTargets(0, 0, 0);
            d3d11FrameBufferView->Release();
            depthBufferView->Release();

            HRESULT res = d3d11SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
            assert(SUCCEEDED(res));

            win32CreateD3D11RenderTargets(d3d11Device, d3d11SwapChain, &d3d11FrameBufferView, &depthBufferView);

            //perspectiveMat = makePerspectiveMat(windowAspectRatio, degreesToRadians(84), 0.1f, 1000.f);
            //XMFLOAT4X4 perspMat;
            //std::memcpy(&perspMat.m, &perspectiveMat.m, sizeof(float) * 4 * 4);

            Globals::Status::windowStatus[Globals::Status::WindowStatus_RESIZE] = false;
        } 

        {
            perspectiveMat = XMMatrixPerspectiveFovRH(XMConvertToRadians(fov), windowAspectRatio, 0.1f, 1000.f);
            //perspectiveMat = XMMatrixTranspose(perspectiveMat);

            //XMFLOAT4X4 xmMatBuff;
            //XMStoreFloat4x4(&xmMatBuff, xmMat);

            //std::memcpy(&perspectiveMat.m, xmMatBuff.m, sizeof(float) * 4 * 4);
        }

        static float mouseSensitivity = 20.0F;

        BEngine::playerCamera.HandleInput(mouseSensitivity, realMouseDrag);
        BEngine::playerCamera.Frame();

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

        XMMATRIX playerMatrix = BEngine::playerCamera.viewMat;
        
        if (!isLoading) {
            BEngine::shaderManager.Proc();
            entityManager.Draw(&playerMatrix, &perspectiveMat);
        }

        if (isLoading)
        {
            ImDrawList* drawList = ImGui::GetBackgroundDrawList();
            ImGuiIO& io = ImGui::GetIO();

            ImVec2 textSize = ImGui::CalcTextSize("Loading...");

            drawList->AddRectFilled(ImVec2(0, 0), io.DisplaySize, ImColor(0, 0, 0));
            drawList->AddText(ImVec2((io.DisplaySize.x / 2) - (textSize.x / 2), (io.DisplaySize.y / 2) - (textSize.y / 2)), ImColor(255, 255, 255), "Loading...");
            
            Sleep(500);
        }
        else {
            ImGui::Begin("Camera");
            {
                std::string delta = "DeltaTime: " + std::to_string(dt);
                ImGui::Text(delta.c_str());
                
                static float smoothFPS = 60.0F;
                smoothFPS -= (smoothFPS - (1.0F / dt)) * dt * 0.8F;

                static float counter = 0;
                counter += dt;

                static float maxFPS = 0.0F;
                static float minFPS = 1000000000.0F;
                const float FPS = (1.0F / dt);

                if (counter > 0.5F) {
                    if (maxFPS < smoothFPS)
                        maxFPS = smoothFPS;

                    if (minFPS > smoothFPS)
                        minFPS = smoothFPS;
                }
                

                ImGui::Text("Max FPS: %.4f", maxFPS);
                ImGui::Text("Min FPS: %.4f", minFPS);
                
                ImGui::Text("FPS: %.2f", smoothFPS);
                ImGui::Text("Pos: %.2f, %.2f, %.2f", BEngine::playerCamera.position.x, BEngine::playerCamera.position.y, BEngine::playerCamera.position.z);
                ImGui::Text("Fwd: %.2f, %.2f, %.2f", BEngine::playerCamera.forward.x, BEngine::playerCamera.forward.y, BEngine::playerCamera.forward.z);
                ImGui::Text("Rot: %.2f, %.2f, %.2f", BEngine::playerCamera.rotation.x, BEngine::playerCamera.rotation.y, BEngine::playerCamera.rotation.z);

                ImGui::SliderFloat("FOV", &fov, 0.1F, 120.0F);

                ImGui::SliderFloat("Maus Empfindlichkeit", &mouseSensitivity, 0, 100);

                ImGui::NewLine();

                ImGui::Text("Actor Num Static: %d", (unsigned int)Globals::PhysX::mScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC));
                ImGui::Text("Actor Num Dynamic: %d", (unsigned int)Globals::PhysX::mScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC));
                
                ImGui::Text("M\xc3\xb6gliche Leistung: %f", 1.0F / (static_cast<float>(mögliche_leistung) / 10000000.0F));
                ImGui::Text("Eyetrace Position: %.2f, %.2f, %.2f", eyeTracePos.x, eyeTracePos.y, eyeTracePos.z );
              
                ImDrawList* bgList = ImGui::GetBackgroundDrawList();
                XMFLOAT2 projectionPoint = BEngine::GuiLib::WorldToScreen(eyeTracePos, playerMatrix, perspectiveMat, windowWidth, windowHeight);
                bgList->AddCircle({projectionPoint.x, projectionPoint.y}, 2.0F, ImColor(255, 0, 0, 100));

                ImGui::Text("PlayerPos: %f %f", projectionPoint.x, projectionPoint.y);
            }
            ImGui::End();
        }

        BEngine::errorReporter.Draw();
        BEngine::ImMenus::DrawImMenus();

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        d3d11SwapChain->Present(0, 0);
    }

    return 0;
}
