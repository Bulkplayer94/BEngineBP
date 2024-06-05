#include "ShaderManager.h"
#include "ErrorReporter.h"
#include "globals.h"
#include <filesystem>
#include "3DMaths.h"

using namespace BEngine;

ShaderManager BEngine::shaderManager = {};

namespace ConstantBuffers {
	struct AnimationCBuffer {
		float deltaTime;
		float currTime;
	};

	struct ModelViewCBuffer {
		float4x4 modelViewMatrix;
	};
}


void ShaderManager::StartLoading()
{
	std::filesystem::directory_iterator dirIterator("data\\shader");
	for (auto& I : dirIterator) {
		if (!I.is_regular_file())
			continue;

		const std::string& fileExtension = I.path().extension().string();
		if (strcmp(fileExtension.c_str(), ".hlsl") != 0)
			continue;

		const std::string& pathString = I.path().string();
		const std::string& fileName = I.path().filename().string();
		const std::string& shortFront = fileName.substr(0, 3);
		const std::string& shaderName = fileName.substr(3, fileName.size() - 8);

		std::wstring wPath(pathString.begin(), pathString.end());

		if (!shaderList.contains(fileName))
			shaderList[fileName] = {};

		Shader* compiledShader = &shaderList[fileName];

		if (strcmp(shortFront.c_str(), "vs_") != 0) {
			ID3D11Device* d3d11Device = Globals::Direct3D::d3d11Device;

			ID3DBlob* shaderCompilerBlob;
			ID3DBlob* vertexShaderBlob;
			HRESULT hResult = D3DCompileFromFile(wPath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", 0, 0, &vertexShaderBlob, &shaderCompilerBlob);
			if (FAILED(hResult)) {
				const char* errorString = (const char*)shaderCompilerBlob->GetBufferPointer();
				errorReporter.Report(ErrorReporter::ErrorLevel_MEDIUM, "\"" + fileName + "\" Shader Compiler Error: " + std::string(errorString));

				continue;
			}

			hResult = d3d11Device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &compiledShader->vertexShader);
			if (FAILED(hResult)) {
				errorReporter.Report(ErrorReporter::ErrorLevel_MEDIUM, "Shader Compiler Error");
				continue;
			}

			constexpr D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
			{
				{ "POS",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                          D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEX",  0, DXGI_FORMAT_R32G32_FLOAT,    0, sizeof(float) * 3,          D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * (3 + 2),    D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			hResult = d3d11Device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &compiledShader->inputLayout);
			if (FAILED(hResult)) {
				errorReporter.Report(ErrorReporter::ErrorLevel_MEDIUM, "Shader Compiler Error");
				continue;
			}

			if (shaderCompilerBlob)
				shaderCompilerBlob->Release();

			vertexShaderBlob->Release();
		}

		if (strcmp(shortFront.c_str(), "ps_") != 0) {
			ID3D11Device* d3d11Device = Globals::Direct3D::d3d11Device;

			ID3DBlob* shaderCompilerBlob;
			ID3DBlob* vertexShaderBlob;
			HRESULT hResult = D3DCompileFromFile(wPath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", 0, 0, &vertexShaderBlob, &shaderCompilerBlob);
			if (FAILED(hResult)) {
				const char* errorString = (const char*)shaderCompilerBlob->GetBufferPointer();
				errorReporter.Report(ErrorReporter::ErrorLevel_MEDIUM, "\"" + fileName + "\" Shader Compiler Error: " + std::string(errorString));

				continue;
			}

			hResult = d3d11Device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &compiledShader->vertexShader);
			if (FAILED(hResult)) {
				errorReporter.Report(ErrorReporter::ErrorLevel_MEDIUM, "Shader Compiler Error");
				continue;
			}
		}
	}
}

ShaderManager::Shader* ShaderManager::GetShader(std::string shaderName)
{
	return &shaderList[shaderName];
}
