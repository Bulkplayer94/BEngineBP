#include "MeshManager.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include "json.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "globals.h"
#include "3DMaths.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cooking/PxCooking.h>

using JSON = nlohmann::json;
using namespace BEngine;
using namespace physx;

BEngine::MeshManager BEngine::meshManager = {};

void Mesh::RefillBuffers() {
	if (!this->isDynamic)
		return;

	D3D11_MAPPED_SUBRESOURCE vertexSubresource;
	Globals::Direct3D::d3d11DeviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vertexSubresource);
	//vertexSubresource.pData = this->vertexData.data();
	memcpy(vertexSubresource.pData, this->vertexData.data(), this->vertexData.size() * sizeof(BEngine::VertexData));
	Globals::Direct3D::d3d11DeviceContext->Unmap(vertexBuffer, 0);
}

ID3D11ShaderResourceView* LoadTexture(std::string filePath) 
{
	int texWidth, texHeight, texNumChannels = 0;

	unsigned char* textureBytes = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texNumChannels, 4);
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

	ID3D11ShaderResourceView* textureView;
	Globals::Direct3D::d3d11Device->CreateShaderResourceView(texture, nullptr, &textureView);
	texture->Release();

	free(textureBytes);

	return textureView;
}

void MeshManager::StartLoading()
{
	std::filesystem::directory_iterator dirIterator("meshes");
	for (auto& dir : dirIterator) {

		if (!dir.is_directory())
			continue;

		std::string pathString = dir.path().generic_string();

		if (!std::filesystem::exists(pathString + "\\description.json"))
			continue;

		if (!std::filesystem::exists(dir.path().generic_string() + "\\description.json"))
			continue;

		std::ifstream jsonFile(dir.path().generic_string() + "\\description.json");
		JSON jsonData;
		jsonFile >> jsonData;

		if (!jsonData.contains("modelMesh") || !jsonData.contains("material"))
			continue;

		// Because the Standard File system gives / and not \ why tho?

		std::string meshName = pathString.substr(pathString.find_last_of('/') + 1, pathString.size());

		Model* loadedModel = new Model;
		loadedModel->modelName = meshName;
		std::vector<float3> allVertices;
		std::vector<unsigned int> allIndices;
		Assimp::Importer importer;

		if (jsonData.contains("propertys") && jsonData["propertys"].is_object()) {
			JSON propertyJson = jsonData["propertys"];

			if (propertyJson.contains("scale") && propertyJson["scale"].is_number())
				importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, propertyJson["scale"]);

			if (propertyJson.contains("static") && propertyJson["static"].is_boolean())
				loadedModel->isStatic = propertyJson["static"];
		}

		bool isDynamic = false;
		if (jsonData.contains("dynamic") && jsonData["dynamic"].is_boolean())
			isDynamic = jsonData["dynamic"];

		const aiScene* scene = importer.ReadFile(std::string(pathString + "\\" + std::string(jsonData["modelMesh"])).c_str(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_GenUVCoords | aiProcess_GenNormals | aiProcess_GlobalScale);

		for (unsigned int scene_iterator = 0; scene_iterator < scene->mNumMeshes; ++scene_iterator)
		{
			Mesh newMesh;
			newMesh.isDynamic = isDynamic;

			const aiMesh* mesh = scene->mMeshes[scene_iterator];

			std::vector<VertexData> vertexVec;
			for (unsigned int vertexIterator = 0; vertexIterator < mesh->mNumVertices; ++vertexIterator)
			{
				aiVector3D vertexP = mesh->mVertices[vertexIterator];
				aiVector3D vertexN = { 0,0,0 };
				if (mesh->HasNormals()) {
					vertexN = mesh->mNormals[vertexIterator];
				}

				float vertexU = 0.0F, vertexV = 0.0F;

				if (mesh->mTextureCoords[scene_iterator]) {
					vertexU = mesh->mTextureCoords[scene_iterator][vertexIterator].x;
					vertexV = mesh->mTextureCoords[scene_iterator][vertexIterator].y;
				}
				else if (mesh->mTangents != nullptr && mesh->mBitangents != nullptr) {
					vertexU = mesh->mTangents[vertexIterator].x;
					vertexV = mesh->mBitangents[vertexIterator].y;
				}

				vertexVec.push_back({
					vertexP.x, vertexP.y, vertexP.z,
					vertexU, vertexV,
					vertexN.x, vertexN.y, vertexN.z
					});

				allVertices.push_back({ vertexP.x, vertexP.y, vertexP.z });
			}
			
			if (newMesh.isDynamic)
				newMesh.vertexData = vertexVec;

			std::vector<unsigned int> indiceVec;
			for (unsigned int faceIterator = 0; faceIterator < mesh->mNumFaces; ++faceIterator)
			{
				const aiFace& face = mesh->mFaces[faceIterator];
				for (unsigned int indiceIterator = 0; indiceIterator < face.mNumIndices; ++indiceIterator)
				{
					indiceVec.push_back(face.mIndices[indiceIterator]);
					allIndices.push_back(face.mIndices[indiceIterator]);
				}
			}

			D3D11_BUFFER_DESC vertexBufferDesc = {};
			vertexBufferDesc.ByteWidth = static_cast<UINT>(sizeof(VertexData) * vertexVec.size());

			if (isDynamic)
			{
				vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			}
			else
				vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA vertexResource = { vertexVec.data() };

			HRESULT hResult = Globals::Direct3D::d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexResource, &newMesh.vertexBuffer);
			if (FAILED(hResult))
				assert("Vertex Buffer Creation Failed");

			D3D11_BUFFER_DESC indiceBufferDesc = {};
			indiceBufferDesc.ByteWidth = static_cast<UINT>(sizeof(unsigned int) * indiceVec.size());
			indiceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indiceBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA indiceResource = { indiceVec.data() };

			hResult = Globals::Direct3D::d3d11Device->CreateBuffer(&indiceBufferDesc, &indiceResource, &newMesh.indiceBuffer);
			if (FAILED(hResult))
				assert("indices Buffer Creation Failed");

			std::vector<float3> collisionVertex;
			for (unsigned int I = 0; I < vertexVec.size(); ++I) {
				auto& currElem = vertexVec[I];
				collisionVertex.push_back({ currElem.pos[0], currElem.pos[1], currElem.pos[2] });
			}

			newMesh.modelTexture.diffuseMap = LoadTexture(std::string(pathString) + "\\" + std::string(jsonData["material"]["volume"]));
			newMesh.numIndices = static_cast<UINT>(indiceVec.size());

			modelNums++;
			newMesh.modelID = modelNums;

			loadedModel->AddModel(newMesh);
		}

		if (!loadedModel->isStatic) {
			PxConvexMeshDesc convexMeshDesc;
			convexMeshDesc.points.count = static_cast<PxU32>(allVertices.size());
			convexMeshDesc.points.data = allVertices.data();
			convexMeshDesc.points.stride = sizeof(float3);
			convexMeshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

			PxCookingParams convexMeshParams(Globals::PhysX::mPhysics->getTolerancesScale());

			PxConvexMesh* convexMesh = PxCreateConvexMesh(convexMeshParams, convexMeshDesc, Globals::PhysX::mPhysics->getPhysicsInsertionCallback());

			loadedModel->physicsModel = Globals::PhysX::mPhysics->createShape(PxConvexMeshGeometry(convexMesh), *Globals::PhysX::mMaterial, false);

			convexMesh->release();
		}
		else {
			PxTriangleMeshDesc triangleMeshDesc;

			triangleMeshDesc.points.count = static_cast<PxU32>(allVertices.size());
			triangleMeshDesc.points.data = allVertices.data();
			triangleMeshDesc.points.stride = sizeof(float3);
			
			triangleMeshDesc.triangles.count = static_cast<PxU32>(allIndices.size()) / 3;
			triangleMeshDesc.triangles.data = allIndices.data();
			triangleMeshDesc.triangles.stride = sizeof(unsigned int) * 3;

			if (!triangleMeshDesc.isValid())
				assert(false);

			PxCookingParams triangleMeshParams(Globals::PhysX::mPhysics->getTolerancesScale());

			PxTriangleMesh* triangleMesh = PxCreateTriangleMesh(triangleMeshParams, triangleMeshDesc);

			loadedModel->physicsModel = Globals::PhysX::mPhysics->createShape(PxTriangleMeshGeometry(triangleMesh), *Globals::PhysX::mMaterial, false);

			triangleMesh->release();
		}


		if (jsonData.contains("defaultShader") && shaderManager.shaderList.contains(jsonData["defaultShader"])) {
			loadedModel->defaultShader = &shaderManager.shaderList[jsonData["defaultShader"]];
		}
		else {
			loadedModel->defaultShader = &shaderManager.shaderList["default"];
		}

		modelNums++;
		loadedModel->modelID = modelNums;

		this->meshList[meshName] = loadedModel;
	}
}