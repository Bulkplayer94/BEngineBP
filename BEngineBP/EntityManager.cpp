#include "EntityManager.h"

#include <PxPhysicsAPI.h>
#include "globals.h"
#include <algorithm>

EntityManager entityManager = {};

using namespace Globals::PhysX;

Entity* EntityManager::RegisterEntity(BEngine::Mesh* mMesh, bool isStatic, float3 entityPos)
{
	Entity* newEnt = new Entity();
	newEnt->isStatic = isStatic;
	newEnt->modelName = mMesh->modelName;
	newEnt->modelMesh = mMesh;
	
	if (!isStatic) {
		newEnt->physicsActor = physx::PxCreateDynamic(*mPhysics, PxTransform(PxIdentity), *mMesh->physicsModel, 10.0F);
	}
	else {
		newEnt->physicsActor = physx::PxCreateStatic(*mPhysics, PxTransform(PxIdentity), *mMesh->physicsModel);
	}

	mScene->addActor(*newEnt->physicsActor);

	PxTransform actorPos = newEnt->physicsActor->getGlobalPose();
	actorPos.p.x = entityPos.x;
	actorPos.p.y = entityPos.y;
	actorPos.p.z = entityPos.z;
	newEnt->physicsActor->setGlobalPose(actorPos);

	registeredEntitys.push_back(newEnt);
	entitySize++;

	return newEnt;
}

float4x4 PxTransformToFloat4x4Alt(const PxTransform& transform) {

	PxMat44 transformMat = (PxMat44)transform;
	transformMat = transformMat.getTranspose();

	float4x4 transMat;
	for (unsigned int I = 0; I != 5; ++I) {
		const PxVec4& floatding = transformMat[I];
		transMat.cols[I] = { floatding[0], floatding[1], floatding[2], floatding[3] };
	}

	//for (unsigned int I = 0; I != 5; ++I) {
	//	for (unsigned int I2 = 0; I2 != 5; ++I2) {
	//		switch (I) {
	//		case(0):
	//			transMat.m[0][I2] = transformMat.column0[I2];
	//			break;

	//		case(1):
	//			transMat.m[1][I2] = transformMat.column1[I2];
	//			break;

	//		case(2):
	//			transMat.m[2][I2] = transformMat.column2[I2];
	//			break;

	//		case(3):
	//			transMat.m[3][I2] = transformMat.column3[I2];
	//			break;
	//		}
	//	}
	//}

	return transMat;
}

UINT modelStride = 32U;
UINT modelOffset = 0;

static std::string currentShader = "";

void EntityManager::Draw(SHADER* shader, BEngine::MeshManager* meshManager, float4x4* viewMat, float4x4* perspMat)
{
	using namespace Globals::Direct3D;
	
	const float4x4& viewPerspMat = *viewMat * *perspMat;

	for (auto& I : registeredEntitys)
	{
		const float4x4& modelViewProj = PxTransformToFloat4x4Alt(I->physicsActor->getGlobalPose()) * viewPerspMat;

		for (auto& meshPart : I->modelMesh->models) {
			shader->sContextFunc(d3d11DeviceContext, &currentShader);
			d3d11DeviceContext->PSSetShaderResources(0, 1, &meshPart.modelTexture.diffuseMap);
			d3d11DeviceContext->IASetVertexBuffers(0, 1, &meshPart.vertexBuffer, &modelStride, &modelOffset);
			d3d11DeviceContext->IASetIndexBuffer(meshPart.indiceBuffer, DXGI_FORMAT_R32_UINT, 0);
			d3d11DeviceContext->PSSetSamplers(0, 1, &samplerState);
			d3d11DeviceContext->VSSetSamplers(0, 1, &samplerState);
			shader->sBufferFunc(d3d11DeviceContext, modelViewProj);

			d3d11DeviceContext->DrawIndexed(meshPart.numIndices, 0, 0);
		}
	}
}

void EntityManager::Sort() {
}

void Entity::SetPosition(float3 pos)
{
	PxTransform trans = physicsActor->getGlobalPose();
	trans.p = PxVec3(pos.x, pos.y, pos.z);
	physicsActor->setGlobalPose(trans);
}

float3 Entity::GetPosition()
{
	PxTransform trans = physicsActor->getGlobalPose();
	return { trans.p.x, trans.p.y, trans.p.z };
}

