#pragma once
#include <d3dcompiler.h>
#include <d3d11_1.h>
#include <map>
#include <string>
#include <DirectXMath.h>
using namespace DirectX;

#include "data/shader/cb_shader_defines.hlsli"
#include "3DMaths.h"

namespace BEngine {
	struct Shader {
		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11PixelShader* pixelShader = nullptr;

		ID3D11InputLayout* inputLayout = nullptr;

		std::string shaderName = "";

		void SetContext(const XMMATRIX& modelMat, const XMMATRIX& perspectiveMat, const XMMATRIX& viewMat);
	};

	struct ShaderManager {

		void StartLoading();
		void Proc();

		void SetDirectionalLight(float3 rotation, float4 color);
		int AddPointLight(float3 pos, float4 color);

		ID3D11Buffer* modelViewBuffer = nullptr;
		ID3D11Buffer* animationBuffer = nullptr;
		ID3D11Buffer* lightsBuffer = nullptr;

		std::map<std::string, Shader> shaderList;

	} extern shaderManager;
}