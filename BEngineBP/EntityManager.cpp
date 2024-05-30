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
		for (unsigned int I2 = 0; I2 != 5; ++I2) {
			switch (I) {
			case(0):
				transMat.m[0][I2] = transformMat.column0[I2];
				break;

			case(1):
				transMat.m[1][I2] = transformMat.column1[I2];
				break;

			case(2):
				transMat.m[2][I2] = transformMat.column2[I2];
				break;

			case(3):
				transMat.m[3][I2] = transformMat.column3[I2];
				break;
			}
		}
	}

	// Extrahiere die Translation und Rotation aus der PxTransform
	//PxVec3 translation = transform.p;
	//PxQuat rotation = transform.q;

	//// Extrahiere die Rotationskomponenten aus der Quaternion
	//float qx = rotation.x;
	//float qy = rotation.y;
	//float qz = rotation.z;
	//float qw = rotation.w;

	//// Berechne die Rotationsmatrix aus den Quaternionkomponenten
	//float rotationMatrix[3][3] = {
	//	{1 - 2 * (qy * qy + qz * qz), 2 * (qx * qy - qz * qw), 2 * (qx * qz + qy * qw)},
	//	{2 * (qx * qy + qz * qw), 1 - 2 * (qx * qx + qz * qz), 2 * (qy * qz - qx * qw)},
	//	{2 * (qx * qz - qy * qw), 2 * (qy * qz + qx * qw), 1 - 2 * (qx * qx + qy * qy)}
	//};

	//// Erstelle die Gesamtmatrix (rotierte Translationsmatrix)
	//float4x4 translateMatrix;
	//// Setze die Rotationskomponenten in Column-Major-Ordnung
	//for (int i = 0; i < 3; ++i) {
	//	for (int j = 0; j < 3; ++j) {
	//		translateMatrix.m[j][i] = rotationMatrix[j][i]; // Spaltenweise Anordnung
	//	}
	//}
	//// Setze die Translationskomponenten
	//translateMatrix.m[0][3] = translation.x;
	//translateMatrix.m[1][3] = translation.y;
	//translateMatrix.m[2][3] = translation.z;
	//// Setze den Rest der Matrix auf Identität
	//translateMatrix.m[3][0] = 0.0f;
	//translateMatrix.m[3][1] = 0.0f;
	//translateMatrix.m[3][2] = 0.0f;
	//translateMatrix.m[3][3] = 1.0f;

	return transMat;
}

UINT modelStride = 32U;
UINT modelOffset = 0;

static std::string currentShader = "";

void EntityManager::Draw(SHADER* shader, BEngine::MeshManager* meshManager, float4x4* viewMat, float4x4* perspMat)
{
	using namespace Globals::Direct3D;
	
	float4x4 viewPerspMat = *viewMat * *perspMat;

	for (auto& I : registeredEntitys)
	{
		float4x4 modelViewProj = PxTransformToFloat4x4Alt(I->physicsActor->getGlobalPose()) * viewPerspMat;

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

