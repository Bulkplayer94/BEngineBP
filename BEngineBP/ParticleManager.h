#pragma once
#include <DirectXMath.h>
#include <vector>
#include "ResizableBuffer.h"

using namespace DirectX;

namespace BEngine {
	inline struct ParticleManager {
		struct Vertex {
			DirectX::XMFLOAT2 position;
			DirectX::XMFLOAT2 uv;
		};

		struct ParticleInstance {
			DirectX::XMFLOAT4X4 modelMatrix;
		};

		struct Particle {
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 velocity;
			DirectX::XMFLOAT3 gravity;
			float drag;
			float lifeTime;
			unsigned int textureID;
			bool isDeleted;
		};

		struct AnimationBuffer {
			float deltaTime;
			float particleCount;
			float padding[2];
		};
		
		struct ParticleMatrices {
			DirectX::XMFLOAT4X4 viewMatrix;
			DirectX::XMFLOAT4X4 projMatrix;
		};

		std::vector<Particle> m_particleList;

		BEngine::ResizeableBuffer<ParticleInstance> m_particleInstances;

		ID3D11VertexShader* m_vertexShader = nullptr;
		ID3D11InputLayout* m_inputLayout = nullptr;
		ID3D11PixelShader* m_pixelShader = nullptr;
		ID3D11ComputeShader* m_computeShader = nullptr;

		ID3D11Buffer* m_vertexBuffer = nullptr;
		ID3D11Buffer* m_indiceBuffer = nullptr;
		ID3D11Buffer* m_particleMatrixBuffer = nullptr;

		ID3D11Buffer* m_rwBuffer = nullptr;
		ID3D11Buffer* m_rwBufferStaging = nullptr;
		ID3D11Buffer* m_animationBuffer = nullptr;

		ID3D11UnorderedAccessView* m_rwBufferUAV = nullptr;

		void Initialize();

		void RebuildBuffer();

		void Draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projMatrix);

		void Cleanup();

	} particleManager;
}