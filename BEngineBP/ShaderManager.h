#pragma once
#include <d3dcompiler.h>
#include <d3d11_1.h>
#include <map>
#include <string>
#include "data/shader/cb_shader_defines.hlsli"
#include "3DMaths.h"

namespace BEngine {
	struct Shader {
		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11PixelShader* pixelShader = nullptr;

		ID3D11InputLayout* inputLayout = nullptr;

		std::string shaderName = "";

		void SetContext(const float4x4& modelMat, const float4x4& perspectiveMat, const float4x4& viewMat);
	};

	struct ShaderManager {

		void StartLoading();
		void Proc();

		ID3D11Buffer* modelViewBuffer = nullptr;
		ID3D11Buffer* animationBuffer = nullptr;
		ID3D11Buffer* lightsBuffer = nullptr;

		std::map<std::string, Shader> shaderList;

	} extern shaderManager;
}