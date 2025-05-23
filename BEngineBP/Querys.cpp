#include "pch.h"
#include "Querys.h"
#include "PhysXManager.h"

bool BEngine::Traces::Eyetrace(float maxDistance, PhysTrace* tracePtr) {
	PxVec3 origin = { BEngine::CPlayerCamera::GetInstance().position.x, BEngine::CPlayerCamera::GetInstance().position.y, BEngine::CPlayerCamera::GetInstance().position.z};
	PxVec3 unitDir = { BEngine::CPlayerCamera::GetInstance().forward.x, BEngine::CPlayerCamera::GetInstance().forward.y, BEngine::CPlayerCamera::GetInstance().forward.z };
	PxReal maxDist = (physx::PxReal)maxDistance;
	PxRaycastBuffer hit;

	bool result = BEngine::PhysXManager::GetInstance().m_scene->raycast(origin, unitDir, maxDist, hit);
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

bool BEngine::Traces::Trace(const XMFLOAT3& position, const XMFLOAT3& direction, float maxDistance, PhysTrace* tracePtr) {
	physx::PxVec3 pos(position.x, position.y, position.z);
	physx::PxVec3 unitDir(direction.x, direction.y, direction.z);
	physx::PxReal distance(maxDistance);
	physx::PxRaycastBuffer hit;

	bool result = BEngine::PhysXManager::GetInstance().m_scene->raycast(pos, unitDir, distance, hit);
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

bool BEngine::Querys::GetEntitesInSphere(const XMFLOAT3& position, float radius, EntityBuffer* entityVec)
{
	PxVec3 origin(position.x, position.y, position.z);
	PxReal pxRadius(radius);
	PxSphereGeometry checkSphere(pxRadius);

	const PxU32 bufferSize = 256;
	PxOverlapHit buffer[bufferSize];

	PxOverlapBuffer hit(buffer, bufferSize);

	if (!BEngine::PhysXManager::GetInstance().m_scene->overlap(checkSphere, PxTransform(origin), hit))
		return false;

	for (unsigned int I = 0; I < hit.nbTouches; ++I)
	{
		if (hit.touches[I].actor->userData != nullptr)
			entityVec->push_back((Entity*)hit.touches[I].actor->userData);
	}

	return true;
}

bool BEngine::Querys::GetEntitiesInBox(const XMFLOAT3& position, const XMFLOAT3& minCube, const XMFLOAT3& maxCube, EntityBuffer* entityVec)
{
	PxVec3 origin(position.x, position.y, position.z);

	PxVec3 pxMinCube(minCube.x, minCube.y, minCube.z);
	PxVec3 pxMaxCube(maxCube.x, maxCube.y, maxCube.z);

	const PxU32 bufferSize = 256;
	PxOverlapHit buffer[256];

	PxOverlapBuffer hit(buffer, bufferSize);

	PxVec3 halfExtends = (pxMaxCube - pxMinCube) / 2;

	PxBoxGeometry boxGeom(halfExtends);

	if (!BEngine::PhysXManager::GetInstance().m_scene->overlap(boxGeom, PxTransform(origin), hit))
		return false;

	for (unsigned int I = 0; I < hit.nbTouches; ++I)
	{
		if (hit.touches[I].actor->userData != nullptr)
			entityVec->push_back((Entity*)hit.touches[I].actor->userData);
	}

	return true;
}