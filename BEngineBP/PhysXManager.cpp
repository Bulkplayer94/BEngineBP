#include "PhysXManager.h"
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

namespace PhysXManager {
    bool SetupPhysX() {
        using namespace Globals::PhysX;

        static PhysXErrorCallback gErrorCallback;
        static PxDefaultAllocator gDefaultAllocatorCallback;

        mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gErrorCallback);
        if (!mFoundation) {
            assert("PhysX Loading Failed!");
        }

        bool recordMemoryAllocation = true;

        mPvd = PxCreatePvd(*mFoundation);
        mTransport = PxDefaultPvdFileTransportCreate("C:\\Users\\goris\\Desktop\\C++\\BEngineBP\\BEngineBP\\PxSaved.pvd");
        mPvd->connect(*mTransport, PxPvdInstrumentationFlag::eALL);

        mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(), recordMemoryAllocation, mPvd);
        if (!mPhysics) {
            assert("PxPhysics didnt Initialize!");
        }
        

        if (!PxInitExtensions(*mPhysics, mPvd))
            assert("PxExtension Failed!");

        // Erstelle ein Material für die Szene
        mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.1f); // Ändere die Reibung und Restitution nach Bedarf

        if (!mMaterial) {
            assert("Material creation Failed!");
        }

        // Erstelle die PxScene
        PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
        sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
        sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(6);
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;

        mScene = mPhysics->createScene(sceneDesc);
        if (!mScene) {
            assert("PxScene creation failed!");
        }

        //mScene->setFlag(PxSceneFlag::eENABLE_ENHANCED_DETERMINISM, true);

        PxRigidStatic* groundPlane = PxCreatePlane(*mPhysics, PxPlane(0, 1, 0, 0), *mMaterial);
        PxTransform transform = groundPlane->getGlobalPose();
        transform.p.y = -200.0f;
        groundPlane->setGlobalPose(transform);
        mScene->addActor(*groundPlane);

        mControllerManager = PxCreateControllerManager(*mScene);

        PxCapsuleControllerDesc controllerDesc;
        controllerDesc.setToDefault();

        controllerDesc.height = 3.0F;
        controllerDesc.radius = 2.0F;
        controllerDesc.position = { 5.0F, 5.0F, 5.0F };
        controllerDesc.material = mMaterial;

        if (!controllerDesc.isValid())
            assert(false);

        mPlayerController = (PxCapsuleController*)mControllerManager->createController(controllerDesc);

        return true;
    }
}

#pragma warning(pop)