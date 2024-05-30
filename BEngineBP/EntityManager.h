#pragma once

#include <string>
#include <unordered_map>
#include <PxActor.h>
#include <PxShape.h>
#include "MeshManager.h"
#include "3DMaths.h"
#include "shader/SHADER_defines.h"

struct Entity {
	BEngine::Mesh* modelMesh;
	std::string modelName;

	physx::PxRigidActor* physicsActor;
	bool isStatic;

	void SetPosition(float3 pos);
	float3 GetPosition();
};

struct EntityManager {
	std::vector<Entity*> registeredEntitys;
	unsigned int entitySize = 0;

	Entity* RegisterEntity(BEngine::Mesh* mMesh, bool isStatic, float3 entityPos = {0.0F, 0.0F, 0.0F});

	enum LightType_ {
		LightType_NONE,
		LightType_BEAM,
		LightType_POINT,
		LightType_DIRECTIONAL
	};
	//unsigned int RegisterLight(float3 lightPos, float3 lightAngle, float3 lightColor, float lightStrenght);

	void Draw(SHADER* shader, BEngine::MeshManager* meshManager, float4x4* viewMat, float4x4* perspMat);
	
	void Sort();
} extern entityManager;