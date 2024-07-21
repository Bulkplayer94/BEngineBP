#pragma once
#include <d3d11_1.h>
#include <vector>
#include <map>
#include <string>
#include <PxPhysicsAPI.h>
#include "ShaderManager.h"

namespace BEngine {

	struct VertexData
	{
		float pos[3];
		float uv[2];
		float norm[3];
	};

	struct Texture {

		// RGB "Color" of the Model
		ID3D11ShaderResourceView* diffuseMap = nullptr;

		// Depth of the Model (Reduces Mesh Komplexity)
		ID3D11ShaderResourceView* normalMap = nullptr;

		// Reflexion of Light
		ID3D11ShaderResourceView* specularMap = nullptr;

		// The Shader that will Render the Texture
		Shader* drawShader = nullptr;

	};

	struct Mesh {

		Texture modelTexture = {};

		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indiceBuffer = nullptr;

		UINT numIndices = 0;

		// Shader that will be used to Draw the Model
		Shader* shader = nullptr;

		unsigned int modelID = 0;

		bool isDynamic = false;

		std::vector<VertexData> vertexData;

		void RefillBuffers();
	};

	struct Model {

		std::vector<Mesh> models;
		std::string modelName;

		Shader* defaultShader = nullptr;

		void AddModel(Mesh& mdl) {
			models.emplace_back(mdl);
		}

		unsigned int modelID = 0;
		bool isStatic = false;
		physx::PxShape* physicsModel = nullptr;
	};

	struct MeshManager {
		std::map<std::string, Model*> meshList;
		unsigned int modelNums = 0;

		void StartLoading();

	} extern meshManager;
}