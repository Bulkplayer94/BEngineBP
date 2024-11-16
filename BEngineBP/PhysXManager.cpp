#include "pch.h"
#include "PhysXManager.h"
#include "TimeManager.h"
#include <iostream>

#pragma warning(push)
#pragma warning(disable: 6011)

static bool ConsoleEventOccured = false;

FILE* stream2;

using namespace physx;
class PhysXErrorCallback : public PxErrorCallback {
	void reportError(PxErrorCode::Enum code, const char* msg, const char* file, int line) {

		if (!ConsoleEventOccured) {
#ifdef _DEBUG
			AllocConsole();
            freopen_s(&stream2, "conout$", "w", stdout);
#else
            freopen_s(&stream2, "error.log", "w", stdout);
#endif // _DEBUG
		}

		std::cout << \
			"An PhysX Error Occured!\n" << \
			"In File: " << file << "\n" << \
			"on Line: " << line << "\n" << \
			msg << std::endl;
	}
};

#pragma warning(pop)

bool BEngine::PhysXManager::Initialize() {
    static PhysXErrorCallback gErrorCallback;
    static PxDefaultAllocator gDefaultAllocatorCallback;

    m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gErrorCallback);
    if (!m_foundation) {
        assert("PhysX Loading Failed!");
    }

    bool recordMemoryAllocation = true;
#ifdef _DEBUG
    m_pvd = PxCreatePvd(*m_foundation);
    m_transport = PxDefaultPvdFileTransportCreate("C:\\Users\\goris\\Desktop\\C++\\BEngineBP\\BEngineBP\\PxSaved.pvd");
    m_pvd->connect(*m_transport, PxPvdInstrumentationFlag::eALL);
#endif // _DEBUG

    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale(), recordMemoryAllocation, m_pvd);
    if (!m_physics) {
        assert("PxPhysics didnt Initialize!");
    }


    if (!PxInitExtensions(*m_physics, m_pvd))
        assert("PxExtension Failed!");

    // Erstelle ein Material für die Szene
    m_material = m_physics->createMaterial(0.5f, 0.5f, 0.1f); // Ändere die Reibung und Restitution nach Bedarf

    if (!m_material) {
        assert("Material creation Failed!");
    }

    // Erstelle die PxScene
    PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(6);
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    m_scene = m_physics->createScene(sceneDesc);
    if (!m_scene) {
        assert("PxScene creation failed!");
    }

    //mScene->setFlag(PxSceneFlag::eENABLE_ENHANCED_DETERMINISM, true);

    PxRigidStatic* groundPlane = PxCreatePlane(*m_physics, PxPlane(0, 1, 0, 0), *m_material);
    PxTransform transform = groundPlane->getGlobalPose();
    transform.p.y = -200.0f;
    groundPlane->setGlobalPose(transform);
    m_scene->addActor(*groundPlane);

    m_controllerManager = PxCreateControllerManager(*m_scene);

    PxCapsuleControllerDesc controllerDesc;
    controllerDesc.setToDefault();

    controllerDesc.height = 3.0F;
    controllerDesc.radius = 2.0F;
    controllerDesc.position = { 5.0F, 5.0F, 5.0F };
    controllerDesc.material = m_material;

    if (!controllerDesc.isValid())
        assert(false);

    m_playerController = (PxCapsuleController*)m_controllerManager->createController(controllerDesc);

    return true;
}

void BEngine::PhysXManager::Frame() {
    m_scene->simulate(BEngine::timeManager.m_deltaTime);
    m_scene->fetchResults(true);
}