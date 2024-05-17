#include "3DManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <DirectXMath.h>
#include <vector>
#include <PxPhysicsAPI.h>

using namespace physx;
using namespace Globals::PhysX;

float4x4 PxTransformToFloat4x4(const PxTransform& transform) {
	// Extrahiere die Translation und Rotation aus der PxTransform
	PxVec3 translation = transform.p;
	PxQuat rotation = transform.q;

	// Extrahiere die Rotationskomponenten aus der Quaternion
	float qx = rotation.x;
	float qy = rotation.y;
	float qz = rotation.z;
	float qw = rotation.w;

	// Berechne die Rotationsmatrix aus den Quaternionkomponenten
	float rotationMatrix[3][3] = {
		{1 - 2 * (qy * qy + qz * qz), 2 * (qx * qy - qz * qw), 2 * (qx * qz + qy * qw)},
		{2 * (qx * qy + qz * qw), 1 - 2 * (qx * qx + qz * qz), 2 * (qy * qz - qx * qw)},
		{2 * (qx * qz - qy * qw), 2 * (qy * qz + qx * qw), 1 - 2 * (qx * qx + qy * qy)}
	};

	// Erstelle die Gesamtmatrix (rotierte Translationsmatrix)
	float4x4 translateMatrix;
	// Setze die Rotationskomponenten in Column-Major-Ordnung
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			translateMatrix.m[j][i] = rotationMatrix[j][i]; // Spaltenweise Anordnung
		}
	}
	// Setze die Translationskomponenten
	translateMatrix.m[0][3] = translation.x;
	translateMatrix.m[1][3] = translation.y;
	translateMatrix.m[2][3] = translation.z;
	// Setze den Rest der Matrix auf Identität
	translateMatrix.m[3][0] = 0.0f;
	translateMatrix.m[3][1] = 0.0f;
	translateMatrix.m[3][2] = 0.0f;
	translateMatrix.m[3][3] = 1.0f;

	return translateMatrix;
}

#ifndef VertexData
struct VertexData
{
	float pos[3];
	float uv[2];
	float norm[3];
};
#endif // VertexData

bool Model::Load(const char* filePath) {

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_GenUVCoords);

	aiMesh* mesh = scene->mMeshes[0];

	std::vector<VertexData> vertexList;
	std::vector<PxVec3> pxVertexList;
	for (unsigned int I = 0; I < mesh->mNumVertices; ++I) {
		aiVector3D vertexP = mesh->mVertices[I];
		aiVector3D vertexN = mesh->mNormals[I];

		pxVertexList.push_back({ vertexP.x, vertexP.y, vertexP.z });

		float vertexU;
		float vertexV;

		if (mesh->mTangents != nullptr && mesh->mBitangents != nullptr) {
			vertexU = mesh->mTangents[I].x;
			vertexV = mesh->mBitangents[I].y;
		}
		else {
			vertexU = mesh->mTextureCoords[0][I].x;
			vertexV = mesh->mTextureCoords[0][I].y;
		}

		vertexList.push_back({
			vertexP.x, vertexP.y, vertexP.z,
			vertexU, vertexV,
			vertexN.x, vertexN.y, vertexN.z
			});
	}

	std::vector<unsigned int> indiceList;
	std::vector<PxVec3> triangleList;
	for (unsigned int I = 0; I < mesh->mNumFaces; ++I) {
		aiFace* face = &mesh->mFaces[I];

		PxVec3 triangle = {};
		for (unsigned int I2 = 0; I2 < face->mNumIndices; ++I2) {
			indiceList.push_back(face->mIndices[I2]);
			triangle[I2] = face->mIndices[I2];
		}
		triangleList.push_back(triangle);
	}

	this->stride = sizeof(VertexData);
	this->numIndices = indiceList.size();

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.ByteWidth = vertexList.size() * sizeof(VertexData);
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexBufferData = { vertexList.data() };

	HRESULT hResult = Globals::Direct3D::d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer);

	if (FAILED(hResult))
		assert("Creating of Vertex Buffer Failed");

	D3D11_BUFFER_DESC indiceBufferDesc = {};
	indiceBufferDesc.ByteWidth = indiceList.size() * sizeof(unsigned int);
	indiceBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indiceBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indiceBufferData = { indiceList.data() };

	hResult = Globals::Direct3D::d3d11Device->CreateBuffer(&indiceBufferDesc, &indiceBufferData, &indexBuffer);

	if (FAILED(hResult))
		assert("Creating of Indice Buffer Failed");

	
	//PxTriangleMeshDesc simpleTriangleMesh;
	//simpleTriangleMesh.points.count = pxVertexList.size();
	//simpleTriangleMesh.points.data = pxVertexList.data();
	//simpleTriangleMesh.points.stride = sizeof(PxVec3);

	//simpleTriangleMesh.triangles.count = indiceList.size();
	//simpleTriangleMesh.triangles.data = indiceList.data();
	//simpleTriangleMesh.triangles.stride = sizeof(unsigned int) * 3;

	//PxTolerancesScale scale;
	//PxCookingParams params(scale);

	////params.midphaseDesc = PxMeshMidPhase::eBVH33;
	////setupCommonCookingParams(params, true, true);

	////params.midphaseDesc.mBVH33Desc.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;
	////params.midphaseDesc.mBVH33Desc.meshSizePerformanceTradeOff = 0.55f;
	//
	//PxDefaultMemoryOutputStream outBuffer;
	//PxCookTriangleMesh(params, simpleTriangleMesh, outBuffer);

	//PxDefaultMemoryInputData stream(outBuffer.getData(), outBuffer.getSize());
	//PxTriangleMesh* triMesh = mPhysics->createTriangleMesh(stream);

	//PxShape* shape = mPhysics->createShape(PxTriangleMeshGeometry(triMesh), *mMaterial);

	////this->physXActor = PxCreateStatic(*mPhysics, PxTransform(PxVec3(0,0,0)), *shape, 1.0F);
	//this->physXActor = PxCreateDynamic(*mPhysics, PxTransform(PxIdentity), *shape, 0.5F);

	//mScene->addActor(*this->physXActor);

	PxConvexMeshDesc convexMeshDesc;
	convexMeshDesc.points.count = pxVertexList.size();
	convexMeshDesc.points.data = pxVertexList.data();
	convexMeshDesc.points.stride = sizeof(PxVec3);

	//convexMeshDesc.polygons.count = triangleList.size();
	//convexMeshDesc.polygons.data = triangleList.data();
	//convexMeshDesc.polygons.stride = sizeof(unsigned int) * 3;

	convexMeshDesc.indices.count = indiceList.size();
	convexMeshDesc.indices.data = indiceList.data();
	convexMeshDesc.indices.stride = sizeof(unsigned int) * 3;

	convexMeshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	if (!convexMeshDesc.isValid())
		assert(false);

	PxTolerancesScale scale;
	PxCookingParams params(scale);

	PxDefaultMemoryOutputStream buf;
	if (!PxCookConvexMesh(params, convexMeshDesc, buf)) {
		assert(false);
		return false;
	}

	PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	PxConvexMesh* convexMesh = mPhysics->createConvexMesh(input);

	PxShape* convexShape = mPhysics->createShape(PxConvexMeshGeometry(convexMesh), *mMaterial);

	if (!isStatic) {
		this->physXActor = PxCreateDynamic(*mPhysics, PxTransform(PxIdentity), *convexShape, 10.0F);
	}
	else {
		this->physXActor = PxCreateStatic(*mPhysics, PxTransform(PxIdentity), *convexShape);
	}

	mScene->addActor(*this->physXActor);

	return true;
	
}

bool Model::LoadTexture(const char* filePath) {
	
	int texWidth, texHeight, texNumChannels = 0;

	unsigned char* textureBytes = stbi_load(filePath, &texWidth, &texHeight, &texNumChannels, 4);
	int textureBytesPerRow = 4 * texWidth;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = texWidth;
	textureDesc.Height = texHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureSubresourceData = {};
	textureSubresourceData.pSysMem = textureBytes;
	textureSubresourceData.SysMemPitch = textureBytesPerRow;

	ID3D11Texture2D* texture;
	Globals::Direct3D::d3d11Device->CreateTexture2D(&textureDesc, &textureSubresourceData, &texture);

	Globals::Direct3D::d3d11Device->CreateShaderResourceView(texture, nullptr, &this->textureView);
	texture->Release();

	free(textureBytes);

	return true;
}

void Model::UpdateViewMats() {
	
	if (physXActor) {
		PxTransform actorTransform = physXActor->getGlobalPose();
		this->cubeModelViewMat = PxTransformToFloat4x4(actorTransform);
	}
	
}

void Model::Draw(SHADER* shader, float4x4* vMat, float4x4* pMat) {
	
	using namespace Globals::Direct3D;

	// Hier überprüfst du, ob der PhysX-Actor vorhanden ist, bevor du versuchst zu zeichnen
	if (physXActor) {
		PxTransform actorTransform = physXActor->getGlobalPose();
		float4x4 mvpMatrix = this->getModelViewProj(vMat, pMat);

		this->Pos = { actorTransform.p.x, actorTransform.p.y, actorTransform.p.z };

		// Setze die Shader-Ressourcen und zeichne den PhysX-Körper
		shader->sContextFunc(d3d11DeviceContext);
		d3d11DeviceContext->PSSetShaderResources(0, 1, &this->textureView);
		d3d11DeviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &this->stride, &this->offset);
		d3d11DeviceContext->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		d3d11DeviceContext->PSSetSamplers(0, 1, &Globals::Direct3D::samplerState);
		d3d11DeviceContext->VSSetSamplers(0, 1, &Globals::Direct3D::samplerState);
		// Übergebe die korrekte Projektionsmatrix an den Shader
		shader->sBufferFunc(d3d11DeviceContext, mvpMatrix);
		
		d3d11DeviceContext->DrawIndexed(this->numIndices, 0, 0);
	}
	
}

void Model::SetPosition(float3 newPosition) {
	
	Pos = newPosition;

	if (physXActor) {
		PxTransform actorTransform = physXActor->getGlobalPose();
		actorTransform.p = PxVec3(newPosition.x, newPosition.y, newPosition.z);
		physXActor->setGlobalPose(actorTransform);
	}

	this->UpdateViewMats();
	
}

float4x4 Model::getModelViewProj(float4x4* viewMat, float4x4* perspectiveMat) {
	
	if (physXActor) {
		// Holen Sie die Transformation des PhysX-Aktors
		PxTransform actorTransform = physXActor->getGlobalPose();
		
		// Berechne die ModelViewProj-Matrix basierend auf der PhysX-Transformation
		return PxTransformToFloat4x4(actorTransform) * (*viewMat * *perspectiveMat);
	}
	else {
		return translationMat(this->Pos) * (*viewMat * *perspectiveMat);
	}
	
}

void Model::SetSleeping(bool status)
{
	this->physXActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, status);
}

void Model::SetShader(UINT newShader) {
	this->ShaderType = newShader;
}

Model::~Model() {
	this->physXActor->release();
	this->textureView->Release();
	this->vertexBuffer->Release();
}