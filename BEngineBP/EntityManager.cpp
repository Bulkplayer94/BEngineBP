#include "EntityManager.h"

#include <PxPhysicsAPI.h>
#include "globals.h"
#include <algorithm>

EntityManager entityManager = {};

using namespace Globals::PhysX;

Entity* EntityManager::RegisterEntity(BEngine::Model* mMesh, float3 entityPos)
{
	Entity* newEnt = new Entity();
	newEnt->isStatic = mMesh->isStatic;
	newEnt->modelName = mMesh->modelName;
	newEnt->modelMesh = mMesh;


	if (!newEnt->isStatic) {
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
	for (unsigned int I = 0; I != 4; ++I) {
		const PxVec4& floatding = transformMat[I];
		transMat.cols[I] = { floatding[0], floatding[1], floatding[2], floatding[3] };
	}

	return transMat;
}

UINT modelStride = 32U;
UINT modelOffset = 0;

static std::string currentShader = "";

bool compareMeshFunc(const std::pair<BEngine::Mesh*, float4x4>& pair1, const std::pair<const BEngine::Mesh*, float4x4>& pair2) {
	return (pair1.first->modelID < pair2.first->modelID);
}

void EntityManager::Draw(SHADER* shader, BEngine::MeshManager* meshManager, float4x4* viewMat, float4x4* perspMat)
{
	using namespace Globals::Direct3D;
	using namespace BEngine;

	float4x4 modelMatrix[100];

	//std::vector<float4x4> floatVec = {};
	//floatVec.reserve(100);

	//memcpy(modelMatrix, floatVec.data() + 100, 100);

	std::vector<std::pair<Mesh*, float4x4>> modelVector;
	modelVector.reserve(registeredEntitys.size());

	for (auto& I : registeredEntitys)
	{
		const float4x4& modelMatrix = PxTransformToFloat4x4Alt(I->physicsActor->getGlobalPose());
		for (auto& I2 : I->modelMesh->models)
		{
			modelVector.emplace_back( &I2, modelMatrix );
		}
	}

	modelVector.shrink_to_fit();
	std::sort(modelVector.begin(), modelVector.end(), compareMeshFunc);

	const float4x4 modelViewProj = createIdentityMatrix();

	std::string currShader = "";
	unsigned int lastModelID = 0;
	for (auto& model : modelVector)
	{
		if (lastModelID != model.first->modelID) {
			//shader->sContextFunc(d3d11DeviceContext, &currShader);
			d3d11DeviceContext->PSSetShaderResources(0, 1, &model.first->modelTexture.diffuseMap);
			d3d11DeviceContext->IASetVertexBuffers(0, 1, &model.first->vertexBuffer, &modelStride, &modelOffset);
			d3d11DeviceContext->IASetIndexBuffer(model.first->indiceBuffer, DXGI_FORMAT_R32_UINT, 0);
			d3d11DeviceContext->PSSetSamplers(0, 1, &samplerState);
			d3d11DeviceContext->VSSetSamplers(0, 1, &samplerState);
			//shader->sBufferFunc(d3d11DeviceContext, &modelViewProj, &model.second, perspMat, viewMat);
		}
		model.first->shader->SetContext(model.second, *perspMat, *viewMat);
		d3d11DeviceContext->DrawIndexed(model.first->numIndices, 0, 0);
	}

	//const float4x4& viewPerspMat = *viewMat * *perspMat;

	//for (auto& I : registeredEntitys)
	//{
	//	const float4x4& modelViewProj = PxTransformToFloat4x4Alt(I->physicsActor->getGlobalPose());// * viewPerspMat;

	//	for (auto& meshPart : I->modelMesh->models) {
	//		shader->sContextFunc(d3d11DeviceContext, &currentShader);
	//		d3d11DeviceContext->PSSetShaderResources(0, 1, &meshPart.modelTexture.diffuseMap);
	//		d3d11DeviceContext->IASetVertexBuffers(0, 1, &meshPart.vertexBuffer, &modelStride, &modelOffset);
	//		d3d11DeviceContext->IASetIndexBuffer(meshPart.indiceBuffer, DXGI_FORMAT_R32_UINT, 0);
	//		d3d11DeviceContext->PSSetSamplers(0, 1, &samplerState);
	//		d3d11DeviceContext->VSSetSamplers(0, 1, &samplerState);
	//		shader->sBufferFunc(d3d11DeviceContext, &modelViewProj, &modelViewProj, perspMat, viewMat);

	//		d3d11DeviceContext->DrawIndexed(meshPart.numIndices, 0, 0);
	//	}
	//}
}

void EntityManager::CheckInstanceBuffer(unsigned int instanceNumber) {

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

void Entity::SetRotation(float3 rot)
{
	float cr = std::cos(rot.x * 0.5F);
	float sr = std::sin(rot.x * 0.5F);
	float cp = std::cos(rot.y * 0.5F);
	float sp = std::sin(rot.y * 0.5F);
	float cy = std::cos(rot.z * 0.5F);
	float sy = std::sin(rot.z * 0.5F);

	PxQuat quat;
	quat.w = cr * cp * cy + sr * sp * sy;
	quat.x = sr * cp * cy - cr * sp * sy;
	quat.y = cr * sp * cy + sr * cp * sy;
	quat.z = cr * cp * sy - sr * sp * cy;

	PxTransform trans = this->physicsActor->getGlobalPose();
	trans.q = quat;
	this->physicsActor->setGlobalPose(trans);
}

float3 Entity::GetRotation()
{
	PxQuat quat = this->physicsActor->getGlobalPose().q;
	float3 rotation;

	// roll (x-axis rotation)
	double sinr_cosp = 2 * (quat.w * quat.x + quat.y * quat.z);
	double cosr_cosp = 1 - 2 * (quat.x * quat.x + quat.y * quat.y);
	rotation.x = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = std::sqrt(1 + 2 * (quat.w * quat.y - quat.x * quat.z));
	double cosp = std::sqrt(1 - 2 * (quat.w * quat.y - quat.x * quat.z));
	rotation.y = 2 * std::atan2(sinp, cosp) - M_PI / 2;

	// yaw (z-axis rotation)
	double siny_cosp = 2 * (quat.w * quat.z + quat.x * quat.y);
	double cosy_cosp = 1 - 2 * (quat.y * quat.y + quat.z * quat.z);
	rotation.z = std::atan2(siny_cosp, cosy_cosp);

	return rotation;
}

