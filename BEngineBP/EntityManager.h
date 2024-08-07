#pragma once

#include <string>
#include <unordered_map>
#include <PxActor.h>
#include <PxShape.h>
#include "MeshManager.h"
#include "3DMaths.h"
#include "shader/SHADER_defines.h"

enum EntityType {
	EntityType_NONE,
	EntityType_LIGHT,
	EntityType_OPAQUE,
	EntityType_DEFAULT,
	EntityType_NOLIGHT,
};

struct BaseEntity {
	EntityType entType = EntityType_NONE;
};

struct Entity {
	BEngine::Model* modelMesh;
	std::string modelName;

	physx::PxRigidActor* physicsActor;
	bool isStatic;

	void SetPosition(float3 pos);
	float3 GetPosition();
	
	void SetRotation(float3 rot);
	float3 GetRotation();
};

struct EntityManager {
	std::vector<Entity*> registeredEntitys;
	unsigned int entitySize = 0;
	ID3D11Buffer* instanceBuffer = nullptr;

	Entity* RegisterEntity(BEngine::Model* mMesh, float3 entityPos = {0.0F, 0.0F, 0.0F});

	void Draw(SHADER* shader, BEngine::MeshManager* meshManager, float4x4* viewMat, float4x4* perspMat);
	
	void CheckInstanceBuffer(unsigned int instanceNumber);
	void Sort();
} extern entityManager;