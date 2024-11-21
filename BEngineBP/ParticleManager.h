#pragma once

#include <d3d11_1.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "ResizableBuffer.h"

namespace BEngine {
	inline struct ParticleManager {

		struct Vertex {
			DirectX::XMFLOAT2 position;
			DirectX::XMFLOAT2 texCoord;
		};

		struct MatrixBuffer {
			DirectX::XMFLOAT4X4 viewProjMatrix;
		};

		struct ParticleInstance {
			DirectX::XMFLOAT4X4 worldMatrix;
			DirectX::XMFLOAT4 color;
		};

		struct Particle {
			DirectX::XMFLOAT3 position;
			float size;
			DirectX::XMFLOAT4 color;
		};

		enum ParticleType {
			ParticleType_STATIC,
			ParticleType_SMOKE,
		};

		struct ParticleEmitter {
			std::vector<Particle> particleEmitter;
			ParticleType particleType;
			ID3D11ShaderResourceView* textureView;
			DirectX::XMFLOAT3 position;
		};

		ID3D11VertexShader* m_d3d11VertexShader = nullptr;
		ID3D11InputLayout* m_d3d11InputLayout = nullptr;

		ID3D11PixelShader* m_d3d11AdditivePixelShader = nullptr;
		ID3D11PixelShader* m_d3d11AlphaPixelShader = nullptr;

		ID3D11BlendState* m_d3d11BlendStateAdditive = nullptr;
		ID3D11BlendState* m_d3d11BlendStateAlpha = nullptr;

		ID3D11SamplerState* m_d3d11SamplerState = nullptr;
		
		ID3D11Buffer* m_vertexBuffer = nullptr;
		ID3D11Buffer* m_indiceBuffer = nullptr;
		ID3D11Buffer* m_matrixBuffer = nullptr;

		ID3D11Buffer* m_instancedBuffer = nullptr;
		ID3D11ShaderResourceView* m_instancedBufferSRV = nullptr;

		std::vector<ParticleEmitter> m_emitterVector;

		unsigned int m_currentInstancedBufferSize = 0U;

		bool Initialize();
		void RebuildParticleBuffer(unsigned int newSize);
		bool AddParticleEmitter(std::string textureName, ParticleType type, DirectX::XMFLOAT3 position, size_t particleCount);
		void Update();
		void Draw(const DirectX::XMFLOAT4X4& viewMatrixRH, const DirectX::XMFLOAT4X4& projMatrixRH);
		void Cleanup();

	} particleManager;

	// Beispiel: Emitter erstellen und hinzufügen
	void CreateExampleEmitter();
}

