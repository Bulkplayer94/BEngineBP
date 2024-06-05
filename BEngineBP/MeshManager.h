#pragma once
#include <d3d11_1.h>
#include <vector>
#include <map>
#include <string>
#include <PxPhysicsAPI.h>
#include "ShaderManager.h"

namespace BEngine {

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

	struct Model {

		Texture modelTexture = {};

		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indiceBuffer = nullptr;

		UINT numIndices = 0;

		// Shader that will be used to Draw the Model
		Shader* shader = nullptr;

	};

	struct Mesh {

		std::vector<Model> models;
		std::string modelName;

		// The Physics Model
		physx::PxShape* physicsModel = nullptr;

		Shader* defaultShader = nullptr;

		void AddModel(Model& mdl) {
			models.emplace_back(mdl);
		}

	};

	struct MeshManager {
		std::map<std::string, Mesh*> meshList;

		void StartLoading();

	} extern meshManager;
}