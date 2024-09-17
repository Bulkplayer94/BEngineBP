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
	template<typename T>
	struct ResizeableBuffer {
		ResizeableBuffer() {
			dataBuffer.push_back(T());
			bufferCapacity = dataBuffer.size();
		}

		~ResizeableBuffer() {
			if (d3d11Buffer) {
				d3d11Buffer->Release();
			}
			if (d3d11ShaderResources) {
				d3d11ShaderResources->Release();
			}
		}

		std::vector<T> dataBuffer = {};
		size_t bufferCapacity = 0;
		ID3D11Buffer* d3d11Buffer = nullptr;
		ID3D11ShaderResourceView* d3d11ShaderResources = nullptr;

		void CreateBuffer();
		bool RefreshBuffer();
	};

	struct Shader {
		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11PixelShader* pixelShader = nullptr;

		ID3D11InputLayout* inputLayout = nullptr;

		std::string shaderName = "";

		void SetContext(const XMMATRIX& perspectiveMat, const XMMATRIX& viewMat);
	};

	struct ShaderManager {
		void CreateInstancedBuffer(int numElements);
		void FillInstancedBuffer(unsigned int numElements, void* data);
		void StartLoading();
		void Proc();

		ID3D11Buffer* modelViewBuffer = nullptr;
		ID3D11Buffer* animationBuffer = nullptr;
		ID3D11Buffer* lightsBuffer = nullptr;
		ID3D11Buffer* instancedBuffer = nullptr;
		ID3D11ShaderResourceView* instancedBufferSRV = nullptr;
		unsigned int instancedBufferCapacity = 0;

		std::map<std::string, Shader> shaderList;
	} extern shaderManager;
}