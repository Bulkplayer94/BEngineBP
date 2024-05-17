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

struct VertexData
{
	float pos[3];
	float uv[2];
	float norm[3];
};

BEngine::MeshManager BEngine::meshManager = {};

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

		// Because the Standard Filesystem gives / and not \ why tho?

		std::string meshName = pathString.substr(pathString.find_last_of('/') + 1, pathString.size());

		Mesh* loadedMesh = new Mesh;
		loadedMesh->modelName = meshName;
		std::vector<float3> allVertices;
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(std::string(pathString + "\\" + std::string(jsonData["modelMesh"])).c_str(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_GenUVCoords);
		for (int scene_iterator = 0; scene_iterator < scene->mNumMeshes; ++scene_iterator) 
		{
			Model newModel;
			
			const aiMesh* mesh = scene->mMeshes[scene_iterator];

			std::vector<VertexData> vertexVec;
			for (int vertexIterator = 0; vertexIterator < mesh->mNumVertices; ++vertexIterator) 
			{
				aiVector3D vertexP = mesh->mVertices[vertexIterator];
				aiVector3D vertexN = mesh->mNormals[vertexIterator];

				float vertexU, vertexV;

				if (mesh->mTangents != nullptr && mesh->mBitangents != nullptr) {
					vertexU = mesh->mTangents[vertexIterator].x;
					vertexV = mesh->mBitangents[vertexIterator].y;
				}
				else {
					if (mesh->mNumUVComponents[scene_iterator] > vertexIterator) {
						vertexU = mesh->mTextureCoords[scene_iterator][vertexIterator].x;
						vertexV = mesh->mTextureCoords[scene_iterator][vertexIterator].y;
					} else 
					{
						vertexU = 0.0F;
						vertexV = 0.0F;
					}
				}

				vertexVec.push_back({
					vertexP.x, vertexP.y, vertexP.z,
					vertexU, vertexV,
					vertexN.x, vertexN.y, vertexN.z
					});

				allVertices.push_back({ vertexP.x, vertexP.y, vertexP.z });
			}

			std::vector<unsigned int> indiceVec;
			for (int faceIterator = 0; faceIterator < mesh->mNumFaces; ++faceIterator) {
				aiFace* face = &mesh->mFaces[faceIterator];
				for (unsigned int indiceIterator = 0; indiceIterator < face->mNumIndices; ++indiceIterator) {
					indiceVec.push_back(face->mIndices[indiceIterator]);
				}
			}

			D3D11_BUFFER_DESC vertexBufferDesc = {};
			vertexBufferDesc.ByteWidth = sizeof(VertexData) * vertexVec.size();
			vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA vertexResource = { vertexVec.data() };

			HRESULT hResult = Globals::Direct3D::d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexResource, &newModel.vertexBuffer);
			if (FAILED(hResult))
				assert("Vertex Buffer Creation Failed");

			D3D11_BUFFER_DESC indiceBufferDesc = {};
			indiceBufferDesc.ByteWidth = sizeof(unsigned int) * indiceVec.size();
			indiceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indiceBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA indiceResource = { indiceVec.data() };

			hResult = Globals::Direct3D::d3d11Device->CreateBuffer(&indiceBufferDesc, &indiceResource, &newModel.indiceBuffer);
			if (FAILED(hResult))
				assert("Indice Buffer Creation Failed");

			std::vector<float3> collisionVertex;
			for (unsigned int I = 0; I < vertexVec.size(); ++I) {
				auto& currElem = vertexVec[I];
				collisionVertex.push_back({ currElem.pos[0], currElem.pos[1], currElem.pos[2] });
			}

			newModel.modelTexture.diffuseMap = LoadTexture(std::string(pathString) + std::string("\\") + std::string(jsonData["material"]["volume"]));
			newModel.numIndices = indiceVec.size();

			loadedMesh->AddModel(newModel);
			
		}

		PxConvexMeshDesc convexMeshDesc;
		convexMeshDesc.points.count = allVertices.size();
		convexMeshDesc.points.data = allVertices.data();
		convexMeshDesc.points.stride = sizeof(float3);
		convexMeshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

		PxCookingParams convexMeshParams(Globals::PhysX::mPhysics->getTolerancesScale());

		PxConvexMesh* convexMesh = PxCreateConvexMesh(convexMeshParams, convexMeshDesc, Globals::PhysX::mPhysics->getPhysicsInsertionCallback());

		// Erstelle eine Form mit dem konvexen Mesh
		loadedMesh->physicsModel = Globals::PhysX::mPhysics->createShape(PxConvexMeshGeometry(convexMesh), *Globals::PhysX::mMaterial, false);

		convexMesh->release();

		this->meshList[meshName] = loadedMesh;
	}
}
