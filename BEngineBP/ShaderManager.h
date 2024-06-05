#pragma once
#include <d3dcompiler.h>
#include <d3d11_1.h>
#include <map>
#include <string>
#include "data/shader/cb_shader_defines.hlsli"
#include "3DMaths.h"

namespace BEngine {
	struct Shader {
		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;

		ID3D11InputLayout* inputLayout;

		std::string shaderName;

		void SetContext(const float4x4& modelViewProj);
	};

	struct ShaderManager {

		void StartLoading();
		void Proc();

		ID3D11Buffer* modelViewBuffer;
		ID3D11Buffer* animationBuffer;

		std::map<std::string, Shader> shaderList;

	} extern shaderManager;
}