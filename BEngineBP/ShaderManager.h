#pragma once
#include <d3dcompiler.h>
#include <d3d11_1.h>
#include <map>
#include <string>
#include "data/shader/cb_shader_defines.hlsli"

namespace BEngine {
	struct ShaderManager {
		struct Shader {
			ID3D11VertexShader* vertexShader;
			ID3D11PixelShader* pixelShader;

			ID3D11InputLayout* inputLayout;

			void SetContext(const float4x4& modelViewProj);
		};

		void StartLoading();
		void Proc();
		Shader* GetShader(std::string shaderName);

	private:
		std::map<std::string, Shader> shaderList;

		ID3D11Buffer* modelViewBuffer;
		ID3D11Buffer* animationBuffer;

	} extern shaderManager;
}