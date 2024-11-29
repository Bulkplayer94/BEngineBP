#include "pch.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Win32Manager.h"
#include "Direct3DManager.h"
#include "TimeManager.h"

#include "globals.h"
#include "GuiLib.h"

#include <iostream>
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <assert.h>
#include <stdint.h>

#include "IMOverlayManager.h"
#include "SettingsManager.h"
#include "EntityManager.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_stdlib.h"

#include "PhysXManager.h"

#include "MeshManager.h"
#include "LuaManager.h"
#include "ShaderManager.h"

#include "SmokeEffect.h"
#include "ParticleManager.h"

#include "ErrorReporter.h"
#include "CCamera.h"

#include "ImMenus.h"
#include "Querys.h"

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

static void performExplosion(PxScene* scene, const PxVec3& explosionCenter, float explosionStrength, float explosionRadius) {
    PxSphereGeometry sphereGeom(explosionRadius);

    const PxU32 maxNbHits = 256;
    PxSweepHit hitBuffer[maxNbHits];

    ExplosionCallback sweepCallback(hitBuffer, maxNbHits);
    sweepCallback.explosionCenter = explosionCenter;

    PxQueryFilterData filterData;
    filterData.flags = PxQueryFlag::eDYNAMIC;

    scene->sweep(sphereGeom, PxTransform(explosionCenter), PxVec3(1.0f, 0.0f, 0.0f), 0, sweepCallback, PxHitFlag::eDEFAULT, filterData);
}

static void LoadRessources() {

    BEngine::shaderManager.StartLoading();
    BEngine::smokeEffect.Initialize();
    BEngine::meshManager.StartLoading();
    BEngine::particleManager.Initialize();

    //BEngine::Model* model = BEngine::meshManager.meshList["welt"];
    //std::cout << "Welt Bounding Box X,Y,Z:\n"
    //    << model->boundingBox[0].x << " " << model->boundingBox[0].y << " " << model->boundingBox[0].z << "\n"
    //    << model->boundingBox[1].x << " " << model->boundingBox[1].y << " " << model->boundingBox[1].z << std::endl;

    //Entity* welt = entityManager.RegisterEntity(BEngine::meshManager.meshList["welt"], { 0.0F, 0.0F, 0.0F });
    //XMFLOAT3 weltRotation = welt->GetRotation();
    //welt->SetRotation({ weltRotation.y + XMConvertToRadians(90.0F), 0.0F, 0.0F });

    unsigned int cubeCount = 10;
    float startPos = 0.F - (cubeCount / 2);

    float spacing = 7.5f;

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

    //entityManager.RegisterEntity(BEngine::meshManager.meshList["cube"], { 0.0F, 0.0F, 0.0F });

    Entity* welt = entityManager.RegisterEntity(BEngine::meshManager.meshList["welt"], { -400.0F, -200.0F, -400.0F });
    welt->SetRotation({ XMConvertToRadians(90.0F), 0.0F, 0.0F });

    isLoading = false;

    return;

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nShowCmd*/)
{
#ifdef _DEBUG
    AllocConsole();
    freopen_s(&stream, "conout$", "w", stdout);
#else
    freopen_s(&stream, "debug.log", "w", stdout);
#endif
    BEngine::settingsManager.loadSettings();

    BEngine::win32Manager.Initialize(hInstance);
    BEngine::direct3DManager.Initialize();
    BEngine::physXManager.Initialize();
    BEngine::timeManager.Initialize();
    BEngine::imOverlayManager.Initialize();
    BEngine::playerCamera.Initialize();
    

    if (!BEngine::settingsManager.isSettingRegistered("FOV")) {
        BEngine::SettingsManager::Setting fovSetting;
        fovSetting.type = BEngine::SettingsManager::SLIDER_FLOAT;
        fovSetting.name = "FOV";
        fovSetting.category = "Camera";
        fovSetting.sliderFloatValue = 86.0F;
        fovSetting.sliderFloatMinValue = 0.1F;
        fovSetting.sliderFloatMaxValue = 120.F;
        
        BEngine::settingsManager.registerSetting(fovSetting);
    }

    using namespace Globals::CUserCmd;
    using namespace physx;

    std::thread thr(&LoadRessources);
    thr.detach();
    
    XMMATRIX perspectiveMat = {};
    Globals::Status::windowStatus[Globals::Status::WindowStatus_RESIZE] = true;

    BEngine::luaManager.Init();
    Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED] = false;

    // Main Loop
    while (BEngine::win32Manager.m_isRunning)
    {
        BEngine::win32Manager.CheckMessages();
        BEngine::timeManager.Frame();

        if (!isLoading && !Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED]) {
            BEngine::physXManager.Frame();
            //BEngine::particleManager.Update();
        }

        

        XMFLOAT3 eyeTracePos = { 0.0F, 0.0F, 0.0F };
        if (!isLoading) {
            using namespace BEngine;

            PxVec3 origin = { playerCamera.position.x, playerCamera.position.y, playerCamera.position.z };
            PxVec3 unitDir = { playerCamera.forward.x, playerCamera.forward.y, playerCamera.forward.z };
            PxReal maxDistance = 500.0F;
            PxRaycastBuffer hit;

            unitDir.normalize();

            bool status = BEngine::physXManager.m_scene->raycast(origin, unitDir, maxDistance, hit);
            if (status) {
                eyeTracePos = { hit.block.position.x, hit.block.position.y, hit.block.position.z };

                if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
                {
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

                        performExplosion(BEngine::physXManager.m_scene, explosionCenter, explosionStrength, explosionRadius);
                    }
                }
            }

            if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)) {
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
            }
        }

        BEngine::imOverlayManager.Proc();

        if (Globals::Status::windowStatus[Globals::Status::WindowStatus_RESIZE] == true)
        {
            BEngine::direct3DManager.m_d3d11DeviceContext->OMSetRenderTargets(0, 0, 0);
            BEngine::direct3DManager.m_d3d11FrameBufferView->Release();
            BEngine::direct3DManager.m_d3d11DepthBufferView->Release();

            HRESULT res = BEngine::direct3DManager.m_d3d11SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
            assert(SUCCEEDED(res));

            //win32CreateD3D11RenderTargets(d3d11Device, d3d11SwapChain, &d3d11FrameBufferView, &depthBufferView);

            BEngine::direct3DManager.CreateD3D11RenderTargets();

            Globals::Status::windowStatus[Globals::Status::WindowStatus_RESIZE] = false;
        } 

        {
            BEngine::SettingsManager::Setting* cameraFov = BEngine::settingsManager.getSetting("FOV");

            perspectiveMat = XMMatrixPerspectiveFovRH(XMConvertToRadians(cameraFov->sliderFloatValue), BEngine::win32Manager.m_aspectRatio , 0.1f, 1000.f);
        }

        XMFLOAT4X4 perspectiveMatLH;
        DirectX::XMStoreFloat4x4(&perspectiveMatLH, XMMatrixTranspose(perspectiveMat));

        XMFLOAT4X4 perspectiveMatRH;
        DirectX::XMStoreFloat4x4(&perspectiveMatRH, perspectiveMat);

        XMFLOAT4X4 viewMatLH;
        DirectX::XMStoreFloat4x4(&viewMatLH, XMMatrixTranspose(BEngine::playerCamera.viewMat));

        XMFLOAT4X4 viewMatRH;
        DirectX::XMStoreFloat4x4(&viewMatRH, BEngine::playerCamera.viewMat);

        BEngine::playerCamera.HandleInput({BEngine::win32Manager.m_mouseDragX, BEngine::win32Manager.m_mouseDragY});
        BEngine::playerCamera.Frame();

        BEngine::direct3DManager.ResetState();

        XMMATRIX playerMatrix = BEngine::playerCamera.viewMat;
        
        if (!isLoading) {
            BEngine::shaderManager.Proc();
            entityManager.Draw(&playerMatrix, &perspectiveMat);

            BEngine::smokeEffect.Draw(viewMatLH, perspectiveMatLH);
            BEngine::particleManager.Draw(viewMatLH, perspectiveMatLH);
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
            if (ImGui::Begin("Camera"))
            {
                std::string delta = "DeltaTime: " + std::to_string(BEngine::timeManager.m_deltaTime);
                ImGui::Text(delta.c_str());
                
                static float smoothFPS = 60.0F;
                smoothFPS -= (smoothFPS - (1.0F / BEngine::timeManager.m_deltaTime)) * BEngine::timeManager.m_deltaTime * 0.8F;

                static float counter = 0;
                counter += BEngine::timeManager.m_deltaTime;

                static float maxFPS = FLT_MIN;
                static float minFPS = FLT_MAX;
                const float FPS = (1.0F / BEngine::timeManager.m_deltaTime);

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

                ImGui::NewLine();

                ImGui::Text("Actor Num Static: %d", (unsigned int)BEngine::physXManager.m_scene->getNbActors(PxActorTypeFlag::eRIGID_STATIC));
                ImGui::Text("Actor Num Dynamic: %d", (unsigned int)BEngine::physXManager.m_scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC));
                
                ImGui::Text("Eyetrace Position: %.2f, %.2f, %.2f", eyeTracePos.x, eyeTracePos.y, eyeTracePos.z );
            }

            //ImGui::ShowDemoWindow();

            BEngine::PhysTrace eyeTrace;
            if (BEngine::Traces::Eyetrace(FLT_MAX, &eyeTrace))
            {

                ImDrawList* backgroundList = ImGui::GetBackgroundDrawList();
                XMFLOAT2 screenPos = BEngine::GuiLib::WorldToScreen(eyeTrace.position, BEngine::playerCamera.viewMat, perspectiveMat, BEngine::win32Manager.m_width, BEngine::win32Manager.m_height);
                backgroundList->AddCircleFilled(ImVec2(screenPos.x, screenPos.y), 1.0F, ImColor(255, 255, 255));

            }

            ImGui::End();

            {
                ImDrawList* backgroundList = ImGui::GetBackgroundDrawList();
                XMFLOAT2 screenPos = BEngine::GuiLib::WorldToScreen({ -100.0F, -100.0F, 40.0F }, BEngine::playerCamera.viewMat, perspectiveMat, BEngine::win32Manager.m_width, BEngine::win32Manager.m_height);
                backgroundList->AddCircleFilled(ImVec2(screenPos.x, screenPos.y), 1.0F, ImColor(255, 0, 0));
            }

            BEngine::settingsManager.drawSettingsMenu();
        }

        BEngine::errorReporter.Draw();
        BEngine::ImMenus::DrawImMenus();

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        BEngine::direct3DManager.PresentFrame();
    }

    BEngine::settingsManager.saveSettings();

    BEngine::direct3DManager.m_d3d11DeviceContext->ClearState();

    BEngine::particleManager.Cleanup();
    BEngine::imOverlayManager.Cleanup();
    //BEngine::particleManager.Cleanup();
    BEngine::smokeEffect.Cleanup();
    BEngine::meshManager.ReleaseObjects();
    BEngine::direct3DManager.ClearObjects();

    return 0;
}
