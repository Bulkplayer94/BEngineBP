#pragma once
#include "CCamera.h"
#include "globals.h"
#include "EntityManager.h"
#include <DirectXMath.h>

using namespace physx;

namespace BEngine {
	namespace Traces {
		struct RayTrace {
			XMFLOAT3 position = XMFLOAT3(0.F, 0.F, 0.F);
			XMFLOAT3 normal = XMFLOAT3(0.F, 0.F, 0.F);
			Entity* entity = nullptr;
			float distance = 0.F;
		};

		bool Eyetrace(float maxDistance, RayTrace* tracePtr) {

			PxVec3 origin = { playerCamera.position.x, playerCamera.position.y, playerCamera.position.z };
			PxVec3 unitDir = { playerCamera.forward.x, playerCamera.forward.y, playerCamera.forward.z };
			PxReal maxDist = (physx::PxReal)maxDistance;
			PxRaycastBuffer hit;

			bool result = Globals::PhysX::mScene->raycast(origin, unitDir, maxDist, hit);
			if (!result)
				return false;

			const PxHitFlags& flag = hit.block.flags;

			if (flag.isSet(PxHitFlag::ePOSITION))
				tracePtr->position = { hit.block.position.x, hit.block.position.y, hit.block.position.z };

			if (flag.isSet(PxHitFlag::eNORMAL))
				tracePtr->normal = { hit.block.normal.x, hit.block.normal.y, hit.block.normal.z };

			tracePtr->distance = hit.block.distance;
			tracePtr->entity = (Entity*)hit.block.actor->userData;

			return true;
		}

		bool Trace(const XMFLOAT3& position, const XMFLOAT3& direction, float maxDistance, RayTrace* tracePtr) {

			physx::PxVec3 pos(position.x, position.y, position.z);
			physx::PxVec3 unitDir(direction.x, direction.y, direction.z);
			physx::PxReal distance(maxDistance);
			physx::PxRaycastBuffer hit;

			bool result = Globals::PhysX::mScene->raycast(pos, unitDir, distance, hit);
			if (!result)
				return false;

			const PxHitFlags& flag = hit.block.flags;

			if (flag.isSet(PxHitFlag::ePOSITION))
				tracePtr->position = { hit.block.position.x, hit.block.position.y, hit.block.position.z };

			if (flag.isSet(PxHitFlag::eNORMAL))
				tracePtr->normal = { hit.block.normal.x, hit.block.normal.y, hit.block.normal.z };

			tracePtr->distance = hit.block.distance;
			tracePtr->entity = (Entity*)hit.block.actor->userData;

			return true;
		}
	}

	namespace Querys {
		bool GetEntitesInSphere(XMFLOAT3 position, float radius, std::vector<Entity*>* entityVec)
		{
			PxVec3 origin(position.x, position.y, position.z);
			PxReal pxRadius(radius);
			PxSphereGeometry checkSphere(pxRadius);


			const PxU32 bufferSize = 256;
			PxOverlapHit buffer[bufferSize];

			PxOverlapBuffer hit(buffer, bufferSize);

			if (!Globals::PhysX::mScene->overlap(checkSphere, PxTransform(origin), hit))
				return false;

			for (int I = 0; I < hit.nbTouches; ++I)
			{
				if (hit.touches[I].actor->userData != nullptr)
					entityVec->push_back((Entity*)hit.touches[I].actor->userData);
			}
			
			return true;
		}

		bool GetEntitiesInCube(const XMFLOAT3& position, const XMFLOAT3& minCube, const XMFLOAT3& maxCube, std::vector<Entity*>* entityVec)
		{
			PxVec3 origin(position.x, position.y, position.z);

			PxVec3 pxMinCube(minCube.x, minCube.y, minCube.z);
			PxVec3 pxMaxCube(maxCube.x, maxCube.y, maxCube.z);

			const PxU32 bufferSize = 256;
			PxOverlapHit buffer[256];

			PxOverlapBuffer hit(buffer, bufferSize);

			PxVec3 halfExtends = pxMaxCube - pxMinCube;
			
			PxBoxGeometry boxGeom(halfExtends);

			if (!Globals::PhysX::mScene->overlap(boxGeom, PxTransform(origin), hit))
				return false;

			for (int I = 0; I < hit.nbTouches; ++I)
			{
				if (hit.touches[I].actor->userData != nullptr)
					entityVec->push_back((Entity*)hit.touches[I].actor->userData);
			}
			
			return true;
		}

	}
}