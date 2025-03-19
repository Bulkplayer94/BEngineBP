#include "pch.h"
#include "ShaderManager.h"
#include "ErrorReporter.h"
#include "Direct3DManager.h"
#include "globals.h"
#include <filesystem>
#include "3DMaths.h"
#include <iostream>
#include "TimeManager.h"
#include "ResizableBuffer.h"

using namespace BEngine;

namespace ConstantBuffers {
	struct AnimationCBuffer {
		float deltaTime;
		double currTime;
	};

	struct MatrixCBuffer {
		XMFLOAT4X4 perspMat;
		XMFLOAT4X4 viewMat;
	};

	struct InstancedCBuffer {
		XMFLOAT4X4 modelMat;
	};

	namespace Lights {
		struct DirectionalLight {
			XMFLOAT3 lightDirection;
			float padding;
			XMFLOAT4 diffuseColor;
		};

		struct PointLight {
			XMFLOAT3 position;
			float brightness;
			XMFLOAT4 diffuseColor;
		};
	}

	struct LightCBuffer {
		Lights::DirectionalLight directionalLight;
		Lights::PointLight pointLights[LIGHTS_COUNT];
	};
}

void ShaderManager::CreateInstancedBuffer(int numElements)
{
	// Beschreibe den Structured Buffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(ConstantBuffers::InstancedCBuffer) * numElements;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.StructureByteStride = sizeof(ConstantBuffers::InstancedCBuffer);
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	HRESULT hResult = BEngine::Direct3DManager::GetInstance().m_d3d11Device->CreateBuffer(&bufferDesc, nullptr, &instancedBuffer);
	if (FAILED(hResult)) {
		BEngine::ErrorReporter::GetInstance().Report(ErrorReporter::ErrorLevel_HIGH, "Creation of Instance Buffer Failed!");
		return;
	}

	// Shader Resource View erstellen
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementWidth = numElements;

	hResult = BEngine::Direct3DManager::GetInstance().m_d3d11Device->CreateShaderResourceView(instancedBuffer, &srvDesc, &instancedBufferSRV);
	if (FAILED(hResult)) {
		BEngine::ErrorReporter::GetInstance().Report(ErrorReporter::ErrorLevel_HIGH, "Creation of SRV for Instance Buffer Failed!");
		return;
	}
}

void ShaderManager::FillInstancedBuffer(unsigned int numElements, void* data)
{
	if (instancedBuffer == nullptr || instancedBufferCapacity < numElements)
	{
		if (instancedBuffer)
		{
			instancedBuffer->Release();
			instancedBufferSRV->Release();
		}
		CreateInstancedBuffer(numElements);
	}

	BEngine::Direct3DManager::GetInstance().m_d3d11DeviceContext->UpdateSubresource(instancedBuffer, 0, nullptr, data, 0, 0);
	BEngine::Direct3DManager::GetInstance().m_d3d11DeviceContext->VSSetShaderResources(5, 1, &instancedBufferSRV);
}

void ShaderManager::StartLoading()
{
	{
		D3D11_BUFFER_DESC constantBufferDesc = {};
		constantBufferDesc.ByteWidth = (sizeof(ConstantBuffers::MatrixCBuffer) + 15) & ~15;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hResult = BEngine::Direct3DManager::GetInstance().m_d3d11Device->CreateBuffer(&constantBufferDesc, nullptr, &modelViewBuffer);
		if (FAILED(hResult))
			BEngine::ErrorReporter::GetInstance().Report(ErrorReporter::ErrorLevel_HIGH, "Creatíon of ModelViewProj Buffer failed!");

		BEngine::Direct3DManager::GetInstance().m_d3d11DeviceContext->VSSetConstantBuffers(0, 1, &modelViewBuffer);
		BEngine::Direct3DManager::GetInstance().m_d3d11DeviceContext->PSSetConstantBuffers(0, 1, &modelViewBuffer);
	}

	{
		D3D11_BUFFER_DESC constantBufferDesc = {};
		// ByteWidth must be a multiple of 16, per the docs
		constantBufferDesc.ByteWidth = (sizeof(ConstantBuffers::AnimationCBuffer) + 15) & ~15;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hResult = BEngine::Direct3DManager::GetInstance().m_d3d11Device->CreateBuffer(&constantBufferDesc, nullptr, &animationBuffer);
		if (FAILED(hResult)) {
			BEngine::ErrorReporter::GetInstance().Report(ErrorReporter::ErrorLevel_HIGH, "Creation of Animation Buffer Failed!");
			return;
		}

		BEngine::Direct3DManager::GetInstance().m_d3d11DeviceContext->VSSetConstantBuffers(1, 1, &animationBuffer);
		BEngine::Direct3DManager::GetInstance().m_d3d11DeviceContext->PSSetConstantBuffers(1, 1, &animationBuffer);
	}

	{
		D3D11_BUFFER_DESC constantBufferDesc = {};
		// ByteWidth must be a multiple of 16, per the docs
		constantBufferDesc.ByteWidth = (sizeof(ConstantBuffers::LightCBuffer) + 15) & ~15;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hResult = BEngine::Direct3DManager::GetInstance().m_d3d11Device->CreateBuffer(&constantBufferDesc, nullptr, &lightsBuffer);
		if (FAILED(hResult)) {
			BEngine::ErrorReporter::GetInstance().Report(ErrorReporter::ErrorLevel_HIGH, "Creation of Light Buffer Failed!");
			return;
		}

		BEngine::Direct3DManager::GetInstance().m_d3d11DeviceContext->VSSetConstantBuffers(2, 1, &lightsBuffer);
		BEngine::Direct3DManager::GetInstance().m_d3d11DeviceContext->PSSetConstantBuffers(2, 1, &lightsBuffer);
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
			ID3D11Device* d3d11Device = BEngine::Direct3DManager::GetInstance().m_d3d11Device;

			ID3DBlob* shaderCompilerBlob;
			ID3DBlob* vertexShaderBlob;
			HRESULT hResult = D3DCompileFromFile(wPath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", 0, 0, &vertexShaderBlob, &shaderCompilerBlob);
			if (FAILED(hResult)) {
				const char* errorString = (const char*)shaderCompilerBlob->GetBufferPointer();
				BEngine::ErrorReporter::GetInstance().Report(ErrorReporter::ErrorLevel_MEDIUM, "\"" + fileName + "\" Shader Compiler Error: " + std::string(errorString));

				continue;
			}

			hResult = d3d11Device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &compiledShader->vertexShader);
			if (FAILED(hResult)) {
				BEngine::ErrorReporter::GetInstance().Report(ErrorReporter::ErrorLevel_MEDIUM, "Shader Compiler Error");
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
				BEngine::ErrorReporter::GetInstance().Report(ErrorReporter::ErrorLevel_MEDIUM, "Shader Compiler Error");
				continue;
			}

			if (shaderCompilerBlob)
				shaderCompilerBlob->Release();

			vertexShaderBlob->Release();
		}

		if (strcmp(shortFront.c_str(), "ps_") == 0) {
			ID3D11Device* d3d11Device = BEngine::Direct3DManager::GetInstance().m_d3d11Device;

			ID3DBlob* shaderCompilerBlob;
			ID3DBlob* vertexShaderBlob;
			HRESULT hResult = D3DCompileFromFile(wPath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", 0, 0, &vertexShaderBlob, &shaderCompilerBlob);
			if (FAILED(hResult)) {
				const char* errorString = (const char*)shaderCompilerBlob->GetBufferPointer();
				BEngine::ErrorReporter::GetInstance().Report(ErrorReporter::ErrorLevel_MEDIUM, "\"" + fileName + "\" Shader Compiler Error: " + std::string(errorString));

				continue;
			}

			hResult = d3d11Device->CreatePixelShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &compiledShader->pixelShader);
			if (FAILED(hResult)) {
				BEngine::ErrorReporter::GetInstance().Report(ErrorReporter::ErrorLevel_MEDIUM, "Shader Compiler Error");
				continue;
			}
		}
	}
}

static std::string currentShader = "";

void ShaderManager::Proc() {
	ID3D11DeviceContext* ctx = BEngine::Direct3DManager::GetInstance().m_d3d11DeviceContext;

	ctx->VSSetConstantBuffers(0, 1, &modelViewBuffer);
	ctx->PSSetConstantBuffers(0, 1, &modelViewBuffer);

	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		ctx->Map(animationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		ConstantBuffers::AnimationCBuffer* buffer = (ConstantBuffers::AnimationCBuffer*)mappedSubresource.pData;
		buffer->deltaTime = BEngine::TimeManager::GetInstance().m_deltaTime;
		buffer->currTime = (double)BEngine::TimeManager::GetInstance().m_currTime;
		ctx->Unmap(animationBuffer, 0);

		ctx->VSSetConstantBuffers(1, 1, &animationBuffer);
		ctx->PSSetConstantBuffers(1, 1, &animationBuffer);
	}

	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		ctx->Map(lightsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		ConstantBuffers::LightCBuffer* buffer = (ConstantBuffers::LightCBuffer*)mappedSubresource.pData;

		buffer->directionalLight.diffuseColor = { 1.F, 0.F, 0.F, 1.F };
		XMVECTOR normalizedDir = XMVector3Normalize({ 0.0F, -0.5F, -0.5F });
		XMStoreFloat3(&buffer->directionalLight.lightDirection, normalizedDir);

		buffer->pointLights[0].position = { 0.F, -40.F, 0.F };
		//buffer->pointLights[1].position = { -600.F, -30.F, 600.F };
		//buffer->pointLights[2].position = { 600.F, -30.F, -600.F };
		//buffer->pointLights[3].position = { -230.F, -70.F, -140.F };

		buffer->pointLights[0].diffuseColor = { 1.F, 1.F, 1.F, 1.F };
		//buffer->pointLights[1].diffuseColor = { 1.F, 1.F, 1.F, 1.F };
		//buffer->pointLights[2].diffuseColor = { 1.F, 1.F, 1.F, 1.F };
		//buffer->pointLights[3].diffuseColor = { 1.F, 1.F, 1.F, 1.F };

		buffer->pointLights[0].brightness = 0.3F;

		ctx->Unmap(lightsBuffer, 0);

		ctx->VSSetConstantBuffers(2, 1, &lightsBuffer);
		ctx->PSSetConstantBuffers(2, 1, &lightsBuffer);
	}

	currentShader = "";
}

void Shader::SetContext(const XMMATRIX& perspectiveMat, const XMMATRIX& viewMat)
{
	ID3D11DeviceContext* ctx = BEngine::Direct3DManager::GetInstance().m_d3d11DeviceContext;

	if (strcmp(currentShader.c_str(), shaderName.c_str()) != 0) {
		ctx->IASetInputLayout(inputLayout);
		ctx->VSSetShader(vertexShader, NULL, 0);
		ctx->PSSetShader(pixelShader, NULL, 0);

		currentShader = shaderName;
	}

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ctx->Map(BEngine::ShaderManager::GetInstance().modelViewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	ConstantBuffers::MatrixCBuffer* buffer = (ConstantBuffers::MatrixCBuffer*)mappedSubresource.pData;

	XMStoreFloat4x4(&buffer->perspMat, perspectiveMat);
	XMStoreFloat4x4(&buffer->viewMat, viewMat);

	ctx->Unmap(BEngine::ShaderManager::GetInstance().modelViewBuffer, 0);
}