#include "pch.h"
#include "EntityManager.h"

#include <PxPhysicsAPI.h>
#include "globals.h"
#include <algorithm>

EntityManager entityManager = {};

using namespace Globals::PhysX;

Entity* EntityManager::RegisterEntity(BEngine::Model* mMesh, XMFLOAT3 entityPos)
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

	newEnt->physicsActor->userData = newEnt;

	registeredEntitys.push_back(newEnt);
	entitySize++;

	return newEnt;
}

XMMATRIX PxTransformToFloat4x4Alt(const PxTransform& transform) {

	PxMat44 transformMat = (PxMat44)transform;
	transformMat = transformMat.getTranspose();
	
	XMFLOAT4X4 transMat;
	std::memcpy(&transMat.m, &transformMat, sizeof(float) * 4 * 4);

	XMMATRIX storedTransMat = XMLoadFloat4x4(&transMat);

	return storedTransMat;
}

UINT modelStride = sizeof(BEngine::VertexData);
UINT modelOffset = 0;

static std::string currentShader = "";

bool compareMeshFunc(const std::pair<BEngine::Mesh*, XMMATRIX>& pair1, const std::pair<const BEngine::Mesh*, XMMATRIX>& pair2) {
	return (pair1.first->modelID < pair2.first->modelID);
}

inline void BindMeshResources(BEngine::Mesh* mesh)
{
	using namespace Globals::Direct3D;

	d3d11DeviceContext->PSSetShaderResources(0, 1, &mesh->modelTexture.volumeMap);
	d3d11DeviceContext->IASetVertexBuffers(0, 1, &mesh->vertexBuffer, &modelStride, &modelOffset);
	d3d11DeviceContext->IASetIndexBuffer(mesh->indiceBuffer, DXGI_FORMAT_R32_UINT, 0);
	d3d11DeviceContext->PSSetSamplers(0, 1, &samplerState);
}

inline void RenderBatch(BEngine::Mesh* mesh, unsigned int instanceCount, const std::vector<XMFLOAT4X4>& matrixBuffer, const XMMATRIX& perspMat, const XMMATRIX& viewMat)
{
	using namespace Globals::Direct3D;

	BEngine::shaderManager.FillInstancedBuffer(instanceCount, (void*)matrixBuffer.data());
	mesh->shader->SetContext(perspMat, viewMat);
	d3d11DeviceContext->DrawIndexedInstanced(mesh->numIndices, instanceCount, 0, 0, 0);
}

void EntityManager::Draw(XMMATRIX* viewMatnT, XMMATRIX* perspMatnT)
{
	using namespace Globals::Direct3D;
	using namespace BEngine;

	// Transpose the view and perspective matrices
	XMMATRIX viewMat = XMMatrixTranspose(*viewMatnT);
	XMMATRIX perspMat = XMMatrixTranspose(*perspMatnT);

	// Prepare a vector of mesh and corresponding model matrices
	std::vector<std::pair<Mesh*, XMMATRIX>> modelVector;
	modelVector.reserve(registeredEntitys.size());

	for (auto& I : registeredEntitys)
	{
		PxTransform trans = I->physicsActor->getGlobalPose();
		XMMATRIX modelMatrix = PxTransformToFloat4x4Alt(trans);

		for (auto& I2 : I->modelMesh->models)
		{
			modelVector.emplace_back(&I2, modelMatrix);
		}
	}

	modelVector.shrink_to_fit();
	std::sort(modelVector.begin(), modelVector.end(), compareMeshFunc);

	// Static matrix buffer to store the matrices for instancing
	static std::vector<XMFLOAT4X4> matrixBuffer(100);
	matrixBuffer.clear();

	Mesh* currMesh = nullptr;
	unsigned int currModelCount = 0;

	// Iterate over the sorted model vector
	for (const auto& I : modelVector)
	{
		Mesh* newMesh = I.first;

		// Check if the mesh has changed
		if (currMesh == nullptr || newMesh->modelID != currMesh->modelID) {
			if (currModelCount > 0) {
				RenderBatch(currMesh, currModelCount, matrixBuffer, perspMat, viewMat);
				matrixBuffer.clear();
				currModelCount = 0;
			}

			currMesh = newMesh;
			BindMeshResources(currMesh); // Bind new mesh resources
		}

		XMFLOAT4X4 modelMatrix;
		XMStoreFloat4x4(&modelMatrix, I.second);

		matrixBuffer.push_back(modelMatrix);
		currModelCount++;
	}

	// Render any remaining instances
	if (currModelCount > 0) {
		RenderBatch(currMesh, currModelCount, matrixBuffer, perspMat, viewMat);
	}
}

void EntityManager::CheckInstanceBuffer(unsigned int instanceNumber) {

}

void EntityManager::Sort() {
}

void Entity::SetPosition(XMFLOAT3 pos)
{
	PxTransform trans = physicsActor->getGlobalPose();
	trans.p = PxVec3(pos.x, pos.y, pos.z);
	physicsActor->setGlobalPose(trans);
}

XMFLOAT3 Entity::GetPosition()
{
	PxTransform trans = physicsActor->getGlobalPose();
	return { trans.p.x, trans.p.y, trans.p.z };
}

void Entity::SetRotation(XMFLOAT3 rot)
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

XMFLOAT3 Entity::GetRotation()
{
	PxQuat quat = this->physicsActor->getGlobalPose().q;
	XMFLOAT3 rotation;

	// roll (x-axis rotation)
	double sinr_cosp = 2 * (quat.w * quat.x + quat.y * quat.z);
	double cosr_cosp = 1 - 2 * (quat.x * quat.x + quat.y * quat.y);
	rotation.x = (float)std::atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = std::sqrt(1 + 2 * (quat.w * quat.y - quat.x * quat.z));
	double cosp = std::sqrt(1 - 2 * (quat.w * quat.y - quat.x * quat.z));
	rotation.y = 2.F * (float)std::atan2(sinp, cosp) - (float)M_PI / 2.F;

	// yaw (z-axis rotation)
	double siny_cosp = 2 * (quat.w * quat.z + quat.x * quat.y);
	double cosy_cosp = 1 - 2 * (quat.y * quat.y + quat.z * quat.z);
	rotation.z = (float)std::atan2(siny_cosp, cosy_cosp);

	return rotation;
}

