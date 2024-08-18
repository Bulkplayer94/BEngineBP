#include "ShaderManager.h"
#include "ErrorReporter.h"
#include "globals.h"
#include <filesystem>
#include "3DMaths.h"
#include <iostream>

using namespace BEngine;

ShaderManager BEngine::shaderManager = {};

namespace ConstantBuffers {
	struct AnimationCBuffer {
		float deltaTime;
		double currTime;
	};

	struct MatrixCBuffer {
		XMFLOAT4X4 worldMat;
		XMFLOAT4X4 perspMat;
		XMFLOAT4X4 viewMat;
	};

	namespace Lights {
		struct DirectionalLight {
			float3 lightDirection;
			float4 diffuseColor;
			float padding;
		};

		struct PointLight {
			float3 position;
			float4 diffuseColor;
			float padding;
		};
	}

	struct LightCBuffer {
		Lights::DirectionalLight directionalLight;
		Lights::PointLight pointLights[LIGHTS_COUNT];
	};
}
                                  

void ShaderManager::StartLoading()
{
	{
		D3D11_BUFFER_DESC constantBufferDesc = {};
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffers::MatrixCBuffer) + 0xf & 0xfffffff0;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		
		HRESULT hResult = Globals::Direct3D::d3d11Device->CreateBuffer(&constantBufferDesc, nullptr, &modelViewBuffer);
		if (FAILED(hResult))
			errorReporter.Report(ErrorReporter::ErrorLevel_HIGH, "Creatíon of ModelViewProj Buffer failed!");

		Globals::Direct3D::d3d11DeviceContext->VSSetConstantBuffers(0, 1, &modelViewBuffer);
		Globals::Direct3D::d3d11DeviceContext->PSSetConstantBuffers(0, 1, &modelViewBuffer);
	}

	{
		D3D11_BUFFER_DESC constantBufferDesc = {};
		// ByteWidth must be a multiple of 16, per the docs
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffers::AnimationCBuffer) + 0xf & 0xfffffff0;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hResult = Globals::Direct3D::d3d11Device->CreateBuffer(&constantBufferDesc, nullptr, &animationBuffer);
		if (FAILED(hResult))
			errorReporter.Report(ErrorReporter::ErrorLevel_HIGH, "Creation of Animation Buffer Failed!");

		Globals::Direct3D::d3d11DeviceContext->VSSetConstantBuffers(1, 1, &animationBuffer);
		Globals::Direct3D::d3d11DeviceContext->PSSetConstantBuffers(1, 1, &animationBuffer);
	}

	{
		D3D11_BUFFER_DESC constantBufferDesc = {};
		// ByteWidth must be a multiple of 16, per the docs
		constantBufferDesc.ByteWidth = sizeof(ConstantBuffers::LightCBuffer) + 0xf & 0xfffffff0;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hResult = Globals::Direct3D::d3d11Device->CreateBuffer(&constantBufferDesc, nullptr, &lightsBuffer);
		if (FAILED(hResult))
			errorReporter.Report(ErrorReporter::ErrorLevel_HIGH, "Creation of Light Buffer Failed!");

		Globals::Direct3D::d3d11DeviceContext->VSSetConstantBuffers(2, 1, &lightsBuffer);
		Globals::Direct3D::d3d11DeviceContext->PSSetConstantBuffers(2, 1, &lightsBuffer);
	}

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

		if (!shaderList.contains(shaderName)) {
			shaderList[shaderName] = {};
			shaderList[shaderName].shaderName = shaderName;
		}

		Shader* compiledShader = &shaderList[shaderName];

		if (strcmp(shortFront.c_str(), "vs_") == 0) {
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

			constexpr D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORM", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONEID", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BONEW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};


			hResult = d3d11Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &compiledShader->inputLayout);
			if (FAILED(hResult)) {
				errorReporter.Report(ErrorReporter::ErrorLevel_MEDIUM, "Shader Compiler Error");
				continue;
			}

			if (shaderCompilerBlob)
				shaderCompilerBlob->Release();

			vertexShaderBlob->Release();
		}

		if (strcmp(shortFront.c_str(), "ps_") == 0) {
			ID3D11Device* d3d11Device = Globals::Direct3D::d3d11Device;

			ID3DBlob* shaderCompilerBlob;
			ID3DBlob* vertexShaderBlob;
			HRESULT hResult = D3DCompileFromFile(wPath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", 0, 0, &vertexShaderBlob, &shaderCompilerBlob);
			if (FAILED(hResult)) {
				const char* errorString = (const char*)shaderCompilerBlob->GetBufferPointer();
				errorReporter.Report(ErrorReporter::ErrorLevel_MEDIUM, "\"" + fileName + "\" Shader Compiler Error: " + std::string(errorString));

				continue;
			}

			hResult = d3d11Device->CreatePixelShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &compiledShader->pixelShader);
			if (FAILED(hResult)) {
				errorReporter.Report(ErrorReporter::ErrorLevel_MEDIUM, "Shader Compiler Error");
				continue;
			}

		}
	}
}

void ShaderManager::Proc() {
	ID3D11DeviceContext* ctx = Globals::Direct3D::d3d11DeviceContext;

	ctx->VSSetConstantBuffers(0, 1, &modelViewBuffer);
	ctx->PSSetConstantBuffers(0, 1, &modelViewBuffer);

	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		ctx->Map(animationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		ConstantBuffers::AnimationCBuffer* buffer = (ConstantBuffers::AnimationCBuffer*)mappedSubresource.pData;
		buffer->deltaTime = Globals::Animation::deltaTime;
		buffer->currTime = (double)Globals::Animation::currTime;
		ctx->Unmap(animationBuffer, 0);

		ctx->VSSetConstantBuffers(1, 1, &animationBuffer);
		ctx->PSSetConstantBuffers(1, 1, &animationBuffer);
	}

	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		ctx->Map(lightsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		ConstantBuffers::LightCBuffer* buffer = (ConstantBuffers::LightCBuffer*)mappedSubresource.pData;
		buffer->directionalLight.diffuseColor = { 1.F, 1.F, 1.F, 1.F };
		buffer->directionalLight.lightDirection = { 0.0F, -0.5F, -0.5F };
		ctx->Unmap(lightsBuffer, 0);

		ctx->VSSetConstantBuffers(2, 1, &lightsBuffer);
		ctx->PSSetConstantBuffers(2, 1, &lightsBuffer);
	}
}

void ShaderManager::SetDirectionalLight(float3 rotation, float4 color) {

}

int ShaderManager::AddPointLight(float3 position, float4 color) {

	return 0;
}

static std::string currentShader = "";

void Shader::SetContext(const XMMATRIX& worldMat, const XMMATRIX& perspectiveMat, const XMMATRIX& viewMat)
{
	ID3D11DeviceContext* ctx = Globals::Direct3D::d3d11DeviceContext;

	if (strcmp(currentShader.c_str(), shaderName.c_str()) != 0) {
		ctx->IASetInputLayout(inputLayout);
		ctx->VSSetShader(vertexShader, NULL, 0);
		ctx->PSSetShader(pixelShader, NULL, 0);
		
		currentShader = shaderName;
	}

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ctx->Map(shaderManager.modelViewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	ConstantBuffers::MatrixCBuffer* buffer = (ConstantBuffers::MatrixCBuffer*)mappedSubresource.pData;

	//buffer->worldMat = worldMat;
	//buffer->perspMat = perspectiveMat;
	//buffer->viewMat = viewMat;

	XMStoreFloat4x4(&buffer->worldMat, worldMat);
	XMStoreFloat4x4(&buffer->perspMat, perspectiveMat);
	XMStoreFloat4x4(&buffer->viewMat, viewMat);

	ctx->Unmap(shaderManager.modelViewBuffer, 0);
}
