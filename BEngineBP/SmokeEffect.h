#pragma once

#include <d3d11_1.h>
#include <DirectXMath.h>
#include <string>

namespace BEngine {
	inline struct SmokeEffect {
		struct Vertex {
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT2 tex;
		};

		struct CBuffer {
			DirectX::XMFLOAT4X4 worldMatrix;
			DirectX::XMFLOAT4X4 viewMatrix;
			DirectX::XMFLOAT4X4 projMatrix;
		};

		ID3D11InputLayout* m_inputLayout = nullptr;
		ID3D11VertexShader* m_vertexShader = nullptr;
		ID3D11PixelShader* m_pixelShader = nullptr;

		ID3D11Buffer* m_vertexBuffer = nullptr;
		ID3D11Buffer* m_indexBuffer = nullptr;
		ID3D11Buffer* m_matrixBuffer = nullptr;

		ID3D11BlendState* m_blendState = nullptr;
		ID3D11ShaderResourceView* m_gradientTexture = nullptr;
		ID3D11SamplerState* m_samplerState = nullptr;

		bool Initialize();
		ID3D11ShaderResourceView* LoadTexture(std::string filePath);
		void Draw(DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projMatrix);
		void Cleanup();
	} smokeEffect;
}
