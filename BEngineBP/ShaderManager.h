#pragma once
#include <d3dcompiler.h>
#include <d3d11_1.h>
#include <map>
#include <string>

namespace BEngine {
	struct ShaderManager {
		struct Shader {
			ID3D11VertexShader* vertexShader;
			ID3D11PixelShader* pixelShader;

			ID3D11InputLayout* inputLayout;
		};

		void StartLoading();
		Shader* GetShader(std::string shaderName);

	private:
		std::map<std::string, Shader> shaderList;

	} extern shaderManager;
}