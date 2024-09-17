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
		float boneids[4];
		float boneWeights[4];
	};

	struct Texture {

		// RGB "Color" of the Model
		ID3D11ShaderResourceView* volumeMap = nullptr;

		// Depth of the Model (Reduces Mesh Komplexity)
		ID3D11ShaderResourceView* normalMap = nullptr;

		// Reflexion of Light
		ID3D11ShaderResourceView* specularMap = nullptr;

		// Special 1 (i.e. Foam)
		ID3D11ShaderResourceView* special_1 = nullptr;

		// The sampler that will be used to render the Texture
		ID3D11SamplerState* sampler = nullptr;

	};

	struct Bone {
		char* name;
		int index;
		int depth;
	};

	struct BoneNode {
		Bone* data;
		BoneNode* children;
		int childCount;
	};

	struct Animation {
		char* name;

		int startTime;
		int endTime;
		float speed;
		float fps;
		int playbackMode;

		Bone* bones;
		int boneCount;

		int frameCount;
	};

	struct Mesh {

		std::vector<Bone> bones;
		std::vector<float4x4> boneMatrixes;

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

		XMFLOAT3 boundingBox[2];
	};

	struct MeshManager {
		std::map<std::string, Model*> meshList;
		unsigned int modelNums = 0;

		void StartLoading();
		void ReleaseObjects();

	} extern meshManager;
}