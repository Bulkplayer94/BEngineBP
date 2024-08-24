#pragma once
#include "CCamera.h"
#include "globals.h"
#include "EntityManager.h"
#include <DirectXMath.h>

namespace BEngine {
	namespace Traces {
		struct RayTrace {
			XMFLOAT3 position;
			XMFLOAT3 normal;
			Entity* entity;
			float distance;
		};

		bool GetEyetrace(float maxDistance, DirectX::XMFLOAT3 position) {

			physx::PxVec3 origin = { playerCamera.position.x, playerCamera.position.y, playerCamera.position.z };
			physx::PxVec3 unitDir = { playerCamera.forward.x, playerCamera.forward.y, playerCamera.forward.z };
			physx::PxReal maxDist = (physx::PxReal)maxDistance;
			physx::PxRaycastBuffer hit;

			bool result = Globals::PhysX::mScene->raycast(origin, unitDir, maxDist, hit);
			if (!result)
				return false;
		}
	}
}