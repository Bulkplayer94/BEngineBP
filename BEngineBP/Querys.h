#pragma once
#include "CCamera.h"
#include "globals.h"
#include "EntityManager.h"
#include <DirectXMath.h>

using namespace physx;

namespace BEngine {

	struct PhysTrace {
		XMFLOAT3 position = XMFLOAT3(0.F, 0.F, 0.F);
		XMFLOAT3 normal = XMFLOAT3(0.F, 0.F, 0.F);
		Entity* entity = nullptr;
		float distance = 0.F; // Euclidean Distance between the Trace Begin and the Object
	};

	typedef std::vector<Entity*> EntityBuffer;
	typedef std::vector<PhysTrace> TraceBuffer;

	// Classes for Simple Traces through the Game
	namespace Traces {
		// Gets the First Entity the Player looks at
		bool Eyetrace(float maxDistance, PhysTrace* tracePtr);

		// Makes a Trace from the Given Position and Direction and gets the first Hit Entity
		bool Trace(const XMFLOAT3& position, const XMFLOAT3& direction, float maxDistance, PhysTrace* tracePtr);
	}

	// Simple Querys that can be used to get Multiple Entities or Objects (cap of Objects is 1024)
	namespace Querys {

		// Gets all Entities in a given Range.
		bool GetEntitesInSphere(const XMFLOAT3& position, float radius, EntityBuffer* entityVec);

		// Gets all Entites that are within the size of the Box around the Position.
		bool GetEntitiesInBox(const XMFLOAT3& position, const XMFLOAT3& minCube, const XMFLOAT3& maxCube, EntityBuffer* entityVec);

	}
}