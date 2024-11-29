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

		ID3D11Buffer* m_vertexBuffer = nullptr;
		ID3D11Buffer* m_indiceBuffer = nullptr;
		ID3D11Buffer* m_particleMatrixBuffer = nullptr;

		void Initialize();

		void Draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projMatrix);

		void Cleanup();

	} particleManager;
}