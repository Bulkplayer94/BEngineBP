#pragma once

#include <string>
#include <unordered_map>
#include <PxActor.h>
#include <PxShape.h>
#include "MeshManager.h"
#include "3DMaths.h"

#include <DirectXMath.h>
using namespace DirectX;

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

	void SetPosition(XMFLOAT3 pos);
	XMFLOAT3 GetPosition();
	
	void SetRotation(XMFLOAT3 rot);
	XMFLOAT3 GetRotation();
};

struct EntityManager {
	std::vector<Entity*> registeredEntitys;
	unsigned int entitySize = 0;
	ID3D11Buffer* instanceBuffer = nullptr;

	Entity* RegisterEntity(BEngine::Model* mMesh, XMFLOAT3 entityPos = {0.0F, 0.0F, 0.0F});

	void Draw(XMMATRIX* viewMat, XMMATRIX* perspMat);
	
	void CheckInstanceBuffer(unsigned int instanceNumber);
	void Sort();
} extern entityManager;