#pragma once

#include <PxPhysicsAPI.h>

using namespace physx;

namespace BEngine {
	inline struct PhysXManager {

        PxFoundation* m_foundation = nullptr;
        PxPvd* m_pvd = nullptr;
        PxPvdTransport* m_transport = nullptr;
        PxPhysics* m_physics = nullptr;
        PxScene* m_scene = nullptr;
        PxMaterial* m_material = nullptr;
        PxControllerManager* m_controllerManager = nullptr;
        PxCapsuleController* m_playerController = nullptr;

        bool Initialize();
        void Frame();

	} physXManager;
}