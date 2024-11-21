#include "pch.h"
#include "ParticleManager.h"
#include "Direct3DManager.h"
#include "TimeManager.h"
#include "CCamera.h"

namespace {
#include "data/compiled_shader/ParticleVS.h"
#include "data/compiled_shader/AdditiveParticlePS.h"
#include "data/compiled_shader/AlphaParticlePS.h"
}

using namespace DirectX;

bool BEngine::ParticleManager::Initialize() {

	HRESULT hRes;
	hRes = BEngine::direct3DManager.m_d3d11Device->CreateVertexShader(ParticleVS, ARRAYSIZE(ParticleVS), nullptr, &m_d3d11VertexShader);
	if (FAILED(hRes))
		return false;

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hRes = BEngine::direct3DManager.m_d3d11Device->CreateInputLayout(inputElements, ARRAYSIZE(inputElements), ParticleVS, ARRAYSIZE(ParticleVS), &m_d3d11InputLayout);
	if (FAILED(hRes))
		return false;

	hRes = BEngine::direct3DManager.m_d3d11Device->CreatePixelShader(AdditiveParticlePS, ARRAYSIZE(AdditiveParticlePS), nullptr, &m_d3d11AdditivePixelShader);
	if (FAILED(hRes))
		return false;

	hRes = BEngine::direct3DManager.m_d3d11Device->CreatePixelShader(AlphaParticlePS, ARRAYSIZE(AlphaParticlePS), nullptr, &m_d3d11AlphaPixelShader);
	if (FAILED(hRes))
		return false;


	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.BorderColor[0] = 0.0f; // Red
	samplerDesc.BorderColor[1] = 0.0f; // Green
	samplerDesc.BorderColor[2] = 0.0f; // Blue
	samplerDesc.BorderColor[3] = 0.0f; // Alpha
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hRes = direct3DManager.m_d3d11Device->CreateSamplerState(&samplerDesc, &m_d3d11SamplerState);
	if (FAILED(hRes)) {
		return false;
	}


	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;

	D3D11_RENDER_TARGET_BLEND_DESC rtBlendDesc = {};
	rtBlendDesc.BlendEnable = TRUE;
	rtBlendDesc.SrcBlend = D3D11_BLEND_ONE;
	rtBlendDesc.DestBlend = D3D11_BLEND_ONE;
	rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
	rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.RenderTarget[0] = rtBlendDesc;

	hRes = direct3DManager.m_d3d11Device->CreateBlendState(&blendDesc, &m_d3d11BlendStateAdditive);
	if (FAILED(hRes)) {
		return false;
	}

	rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;

	blendDesc.RenderTarget[0] = rtBlendDesc;

	hRes = direct3DManager.m_d3d11Device->CreateBlendState(&blendDesc, &m_d3d11BlendStateAlpha);
	if (FAILED(hRes)) {
		return false;
	}


	Vertex vertices[] = {
		{XMFLOAT2(-1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT2(-1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT2(1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f)},
		{XMFLOAT2(1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	
	D3D11_SUBRESOURCE_DATA verticesSubresource = { vertices };

	hRes = BEngine::direct3DManager.m_d3d11Device->CreateBuffer(&vertexBufferDesc, &verticesSubresource, &m_vertexBuffer);
	if (FAILED(hRes))
		return false;


	UINT indices[] = {
		0, 1, 2,
		2, 1, 3
	};

	D3D11_BUFFER_DESC indicesBufferDesc;
	ZeroMemory(&indicesBufferDesc, sizeof(D3D11_BUFFER_DESC));
	indicesBufferDesc.ByteWidth = sizeof(UINT) * ARRAYSIZE(indices);
	indicesBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indicesBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indicesSubresource = { indices };

	hRes = BEngine::direct3DManager.m_d3d11Device->CreateBuffer(&indicesBufferDesc, &indicesSubresource, &m_indiceBuffer);
	if (FAILED(hRes))
		return false;

	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(D3D11_BUFFER_DESC));
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer) & ~15;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hRes = BEngine::direct3DManager.m_d3d11Device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	if (FAILED(hRes))
		return false;

	RebuildParticleBuffer(10);
}

void BEngine::ParticleManager::RebuildParticleBuffer(unsigned int newSize) {

	if (m_currentInstancedBufferSize > newSize) {
		return;
	}

	if (m_instancedBuffer != nullptr) m_instancedBuffer->Release();
	if (m_instancedBufferSRV != nullptr) m_instancedBufferSRV->Release();
	m_instancedBuffer = nullptr;
	m_instancedBufferSRV = nullptr;


	D3D11_BUFFER_DESC instancedBufferDesc;
	ZeroMemory(&instancedBufferDesc, sizeof(D3D11_BUFFER_DESC));
	instancedBufferDesc.ByteWidth = (sizeof(ParticleInstance) * newSize) & ~15;
	instancedBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instancedBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	instancedBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instancedBufferDesc.StructureByteStride = sizeof(ParticleInstance);
	instancedBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	HRESULT hRes = BEngine::direct3DManager.m_d3d11Device->CreateBuffer(&instancedBufferDesc, nullptr, &m_instancedBuffer);
	if (FAILED(hRes) || m_instancedBuffer == nullptr) {
		assert(false);
		return;
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC instancedSRVDesc;
	ZeroMemory(&instancedSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	instancedSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancedSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	instancedSRVDesc.Buffer.ElementWidth = newSize;

	hRes = BEngine::direct3DManager.m_d3d11Device->CreateShaderResourceView(m_instancedBuffer, &instancedSRVDesc, &m_instancedBufferSRV);
	if (FAILED(hRes) || m_instancedBufferSRV == nullptr) {
		assert(false);
		return;
	}

	m_currentInstancedBufferSize = newSize;
}

/*
namespace {
	ID3D11ShaderResourceView* LoadTexture(std::string filePath) {
		int texWidth, texHeight, texNumChannels = 0;

		unsigned char* textureBytes = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texNumChannels, 4);
		if (textureBytes == nullptr)
			return nullptr;

		int textureBytesPerRow = 4 * texWidth;

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = texWidth;
		textureDesc.Height = texHeight;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA textureSubresourceData = {};
		textureSubresourceData.pSysMem = textureBytes;
		textureSubresourceData.SysMemPitch = textureBytesPerRow;

		ID3D11Texture2D* texture;
		if (FAILED(BEngine::direct3DManager.m_d3d11Device->CreateTexture2D(&textureDesc, &textureSubresourceData, &texture))) {
			return nullptr;
		}

		ID3D11ShaderResourceView* textureView;
		if (FAILED(BEngine::direct3DManager.m_d3d11Device->CreateShaderResourceView(texture, nullptr, &textureView))) {
			return nullptr;
		}
		texture->Release();

		free(textureBytes);

		return textureView;
	}
}
*/

extern ID3D11ShaderResourceView* LoadTexture(std::string);

bool BEngine::ParticleManager::AddParticleEmitter(std::string texturePath, ParticleType type, DirectX::XMFLOAT3 position, size_t particleCount) {

	if (!std::filesystem::exists(texturePath))
		return false;

	ParticleEmitter emitter;
	emitter.particleType = type;
	emitter.textureView = LoadTexture(texturePath);

	for (size_t I = 0; I < particleCount; ++I) {
		emitter.particleEmitter.emplace_back(Particle{
			position,
			5.F,
			{1.0F, 1.0F, 1.0F, 1.0F}
			});
	}

	m_emitterVector.push_back(emitter);
	
	return true;

}

void BEngine::ParticleManager::Update() {
	// Update all emitters
	for (auto& emitter : m_emitterVector) {
		for (auto& particle : emitter.particleEmitter) {
			// Update particle position based on type (simple placeholder logic)
			if (emitter.particleType == ParticleType_SMOKE) {
				particle.position.y += 0.1f * BEngine::timeManager.m_deltaTime; // Smoke rises
				particle.color.w -= 0.01f * BEngine::timeManager.m_deltaTime;  // Fade over time
			}
			
			// Clamp particle color alpha
			particle.color.w = std::max(0.0f, particle.color.w);
			
			// Reset particle if alpha fades out
			if (particle.color.w <= 0.0f) {
				particle.position = emitter.position; // Reset to emitter position
				particle.color = { 1.0f, 1.0f, 1.0f, 1.0f };
			}
		}
	}
}

void BEngine::ParticleManager::Draw(const XMFLOAT4X4& viewMatrixRH, const XMFLOAT4X4& projMatrixRH) {
	ID3D11DeviceContext1* ctx = BEngine::direct3DManager.m_d3d11DeviceContext;

	// Bind pipeline states
	UINT vertexStride = sizeof(Vertex);
	UINT vertexOffset = 0;
	ctx->IASetInputLayout(m_d3d11InputLayout);
	ctx->IASetVertexBuffers(0, 1, &m_vertexBuffer, &vertexStride, &vertexOffset);
	ctx->IASetIndexBuffer(m_indiceBuffer, DXGI_FORMAT_R32_UINT, 0);
	ctx->VSSetShader(m_d3d11VertexShader, nullptr, 0);
	ctx->PSSetSamplers(0, 1, &m_d3d11SamplerState);

	// Create view-projection matrix
	MatrixBuffer matrixBuffer = {};
	XMMATRIX viewMatrix = XMLoadFloat4x4(&viewMatrixRH);
	XMMATRIX projMatrix = XMLoadFloat4x4(&projMatrixRH);
	XMStoreFloat4x4(&matrixBuffer.viewProjMatrix, XMMatrixTranspose(viewMatrix * projMatrix));

	// Update constant buffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hRes = ctx->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(hRes)) {
		memcpy(mappedResource.pData, &matrixBuffer, sizeof(MatrixBuffer));
		ctx->Unmap(m_matrixBuffer, 0);
	}

	ctx->VSSetConstantBuffers(5, 1, &m_matrixBuffer);

	// Draw each emitter
	for (auto& emitter : m_emitterVector) {
		ctx->PSSetShaderResources(0, 1, &emitter.textureView);

		// Update particle instance buffer
		std::vector<ParticleInstance> instances;
		for (const auto& particle : emitter.particleEmitter) {
			ParticleInstance instance = {};
			XMMATRIX worldMatrix;
			{
				XMVECTOR parPos = XMLoadFloat3(&particle.position);
				XMVECTOR camPos = XMLoadFloat3(&BEngine::playerCamera.position);
				XMVECTOR lookDir = parPos - camPos;
				lookDir = XMVector3Normalize(lookDir);

				float pitch = asinf(XMVectorGetY(lookDir));
				float yaw = atan2f(XMVectorGetX(lookDir), XMVectorGetZ(lookDir));

				XMMATRIX rotationX = XMMatrixRotationX(-pitch);
				XMMATRIX rotationY = XMMatrixRotationY(yaw);

				XMMATRIX rotationMatrix = XMMatrixMultiply(rotationX, rotationY);

				XMMATRIX translationMatrix = XMMatrixTranslationFromVector(parPos);
				XMMATRIX scalingMatrix = XMMatrixScaling(particle.size, particle.size, particle.size);

				worldMatrix = XMMatrixMultiply(scalingMatrix, rotationMatrix);
				worldMatrix = XMMatrixMultiply(worldMatrix, translationMatrix);
			}

			XMStoreFloat4x4(&instance.worldMatrix, XMMatrixTranspose(worldMatrix));
			instance.color = particle.color;
			instances.push_back(instance);
		}

		// Resize instance buffer if necessary
		if (instances.size() > m_currentInstancedBufferSize) {
			RebuildParticleBuffer(static_cast<unsigned int>(instances.size()));
		}

		// Map instance data to buffer
		hRes = ctx->Map(m_instancedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (SUCCEEDED(hRes)) {
			memcpy(mappedResource.pData, instances.data(), instances.size() * sizeof(ParticleInstance));
			ctx->Unmap(m_instancedBuffer, 0);
		}

		// Bind instance buffer and shader
		ctx->VSSetShaderResources(1, 1, &m_instancedBufferSRV);
		ctx->DrawIndexedInstanced(6, static_cast<UINT>(instances.size()), 0, 0, 0);
	}
}


void BEngine::ParticleManager::Cleanup() {
	if (m_d3d11VertexShader) m_d3d11VertexShader->Release();
	if (m_d3d11InputLayout) m_d3d11InputLayout->Release();
	if (m_d3d11AdditivePixelShader) m_d3d11AdditivePixelShader->Release();
	if (m_d3d11AlphaPixelShader) m_d3d11AlphaPixelShader->Release();
	if (m_d3d11BlendStateAdditive) m_d3d11BlendStateAdditive->Release();
	if (m_d3d11BlendStateAlpha) m_d3d11BlendStateAlpha->Release();
	if (m_d3d11SamplerState) m_d3d11SamplerState->Release();
	if (m_vertexBuffer) m_vertexBuffer->Release();
	if (m_indiceBuffer) m_indiceBuffer->Release();
	if (m_matrixBuffer) m_matrixBuffer->Release();
	if (m_instancedBuffer) m_instancedBuffer->Release();
	if (m_instancedBufferSRV) m_instancedBufferSRV->Release();

	for (auto& emitter : m_emitterVector) {
		if (emitter.textureView) emitter.textureView->Release();
	}

	m_d3d11VertexShader = nullptr;
	m_d3d11InputLayout = nullptr;
	m_d3d11AdditivePixelShader = nullptr;
	m_d3d11AlphaPixelShader = nullptr;
	m_d3d11BlendStateAdditive = nullptr;
	m_d3d11BlendStateAlpha = nullptr;
	m_d3d11SamplerState = nullptr;
	m_vertexBuffer = nullptr;
	m_indiceBuffer = nullptr;
	m_matrixBuffer = nullptr;
	m_instancedBuffer = nullptr;
	m_instancedBufferSRV = nullptr;
	m_emitterVector.clear();
}

// Beispiel: Emitter erstellen und hinzufügen
void BEngine::CreateExampleEmitter() {
	using namespace DirectX;

	// Definiere die Position des Emitters
	XMFLOAT3 emitterPosition = { 0.0f, 0.0f, 0.0f };

	// Pfad zur Textur des Emitters
	std::string texturePath = "data/textures/smoke.png";

	// Partikeltyp auswählen
	BEngine::ParticleManager::ParticleType particleType = BEngine::ParticleManager::ParticleType_SMOKE;

	// Anzahl der Partikel
	size_t particleCount = 100;

	// Emitter zum Partikelmanager hinzufügen
	bool success = BEngine::particleManager.AddParticleEmitter(texturePath, particleType, emitterPosition, particleCount);
	if (success) {
		std::cout << "Particle emitter successfully created!" << std::endl;
	}
	else {
		std::cerr << "Failed to create particle emitter. Check the texture path or other parameters." << std::endl;
	}
}
