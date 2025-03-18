#include "pch.h"
#include "ParticleManager.h"

#include "Direct3DManager.h"
#include "CCamera.h"
#include "TimeManager.h"
#include "globals.h"

#include "data/compiled_shader/ParticleVS.h"
#include "data/compiled_shader/ParticlePS.h"
#include "data/compiled_shader/ParticleCS.h"

#include "Timer.hpp"

#define RELEASE_D3D11_OBJECT(obj) if (obj != nullptr) { obj->Release(); obj = nullptr; }


void BEngine::ParticleManager::Initialize() {

	ID3D11Device1* device = BEngine::direct3DManager.m_d3d11Device;
	HRESULT hRes;

	{
		constexpr D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		hRes = device->CreateVertexShader(ParticleVS, ARRAYSIZE(ParticleVS), nullptr, &m_vertexShader);
		assert(SUCCEEDED(hRes));

		hRes = device->CreateInputLayout(layout, ARRAYSIZE(layout), ParticleVS, ARRAYSIZE(ParticleVS), &m_inputLayout);
		assert(SUCCEEDED(hRes));

		hRes = device->CreatePixelShader(ParticlePS, ARRAYSIZE(ParticlePS), nullptr, &m_pixelShader);
		assert(SUCCEEDED(hRes));

		hRes = device->CreateComputeShader(ParticleCS, ARRAYSIZE(ParticleCS), nullptr, &m_computeShader);
		assert(SUCCEEDED(hRes));
	}

	{
		constexpr Vertex vertices[] = {
			{ XMFLOAT2(-1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT2(-1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT2(1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT2(1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		};

		D3D11_BUFFER_DESC verticeBufferDesc;
		ZeroMemory(&verticeBufferDesc, sizeof(D3D11_BUFFER_DESC));

		verticeBufferDesc.ByteWidth = sizeof(Vertex) * 4;
		verticeBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		verticeBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		verticeBufferDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA verticeBufferData = { vertices };

		hRes = device->CreateBuffer(&verticeBufferDesc, &verticeBufferData, &m_vertexBuffer);
		assert(SUCCEEDED(hRes));
	}

	{
		constexpr UINT indices[] = {
			0, 1, 2,
			2, 1, 3
		};

		D3D11_BUFFER_DESC indiceBufferDesc;
		ZeroMemory(&indiceBufferDesc, sizeof(D3D11_BUFFER_DESC));

		indiceBufferDesc.ByteWidth = sizeof(UINT) * 6;
		indiceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indiceBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indiceBufferDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA indiceBufferData = { indices };

		hRes = device->CreateBuffer(&indiceBufferDesc, &indiceBufferData, &m_indiceBuffer);
		assert(SUCCEEDED(hRes));
	}

	{
		D3D11_BUFFER_DESC particleMatricesBufferDesc;
		ZeroMemory(&particleMatricesBufferDesc, sizeof(D3D11_BUFFER_DESC));

		particleMatricesBufferDesc.ByteWidth = sizeof(ParticleMatrices);
		particleMatricesBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		particleMatricesBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		particleMatricesBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		hRes = device->CreateBuffer(&particleMatricesBufferDesc, nullptr, &m_particleMatrixBuffer);
		assert(SUCCEEDED(hRes));
	}

	int textureID = m_atlas.AppendTexture("data/textures/smoke.png");

	{
		unsigned int particleCount = 64;
		float startPos = 100.F - (particleCount / 2);
		
		float spacing = 20.0f;
		
		for (unsigned int I1 = 0; I1 < particleCount; ++I1) {
			for (unsigned int I2 = 0; I2 < particleCount; ++I2) {
				for (unsigned int I3 = 0; I3 < particleCount; ++I3) {
					float currPosX = startPos + I1 * spacing;
					float currPosY = startPos + I2 * spacing;
					float currPosZ = startPos + I3 * spacing;
		
					Particle computeParticle;
					computeParticle.position = XMFLOAT3(currPosX, currPosY, currPosZ);
					computeParticle.velocity = XMFLOAT3(5.0F, 2.0F, 0.0F);
					computeParticle.gravity = XMFLOAT3(0.0F, 1.0F, 0.0F);
					computeParticle.drag = 1.0F;
					computeParticle.lifeTime = 5.0F;
					computeParticle.isDeleted = false;
					computeParticle.textureID = textureID;
		
					m_particleList.push_back(computeParticle);
					m_particleInstances.dataBuffer.push_back({ XMFLOAT4X4() });
					m_particleInstances2.dataBuffer.push_back({ XMFLOAT4X4() });
				}
			}
		}

		m_particleInstances.CreateBuffer();
		m_particleInstances.RefreshBuffer();

		m_particleInstances2.CreateBuffer();
		m_particleInstances2.RefreshBuffer();
	}

	{
		D3D11_BUFFER_DESC animationBufferDesc;
		ZeroMemory(&animationBufferDesc, sizeof(D3D11_BUFFER_DESC));

		animationBufferDesc.ByteWidth = sizeof(AnimationBuffer);
		animationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		animationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		animationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		hRes = device->CreateBuffer(&animationBufferDesc, nullptr, &m_animationBuffer);
		assert(SUCCEEDED(hRes));
	}

	{
		D3D11_QUERY_DESC queryDesc = {};
		queryDesc.Query = D3D11_QUERY_OCCLUSION_PREDICATE;
		queryDesc.MiscFlags = 0;

		hRes = device->CreatePredicate(&queryDesc, &m_occlusionPredicate);
		assert(SUCCEEDED(hRes));
	}

	RebuildBuffer();
}

void BEngine::ParticleManager::RebuildBuffer() {
	RELEASE_D3D11_OBJECT(m_rwBuffer);
	RELEASE_D3D11_OBJECT(m_rwBufferUAV);
	RELEASE_D3D11_OBJECT(m_rwBufferStaging);

	if (m_particleList.size() > 0) {
		D3D11_BUFFER_DESC rwBufferDesc;
		ZeroMemory(&rwBufferDesc, sizeof(D3D11_BUFFER_DESC));

		rwBufferDesc.ByteWidth = sizeof(Particle) * m_particleList.size();
		rwBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		rwBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		rwBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		rwBufferDesc.StructureByteStride = sizeof(Particle);

		D3D11_SUBRESOURCE_DATA rwBufferData;
		
		rwBufferData = { m_particleList.data() };
			

		HRESULT hRes = BEngine::direct3DManager.m_d3d11Device->CreateBuffer(&rwBufferDesc, &rwBufferData, &m_rwBuffer);
		assert(SUCCEEDED(hRes));

		D3D11_UNORDERED_ACCESS_VIEW_DESC rwBufferUAVDesc;
		ZeroMemory(&rwBufferUAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

		rwBufferUAVDesc.Buffer.FirstElement = 0;
		rwBufferUAVDesc.Buffer.NumElements = m_particleList.size();
		rwBufferUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
		rwBufferUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

		hRes = BEngine::direct3DManager.m_d3d11Device->CreateUnorderedAccessView(m_rwBuffer, &rwBufferUAVDesc, &m_rwBufferUAV);
		assert(SUCCEEDED(hRes));
	
		D3D11_BUFFER_DESC rwBufferStagingDesc;
		ZeroMemory(&rwBufferStagingDesc, sizeof(D3D11_BUFFER_DESC));

		rwBufferStagingDesc.ByteWidth = sizeof(Particle) * m_particleList.size();
		rwBufferStagingDesc.Usage = D3D11_USAGE_STAGING;
		rwBufferStagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		hRes = BEngine::direct3DManager.m_d3d11Device->CreateBuffer(&rwBufferStagingDesc, nullptr, &m_rwBufferStaging);
		assert(SUCCEEDED(hRes));
	}
}

void BEngine::ParticleManager::Draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projMatrix) {
	ID3D11DeviceContext1* ctx = BEngine::direct3DManager.m_d3d11DeviceContext;

	if (m_particleList.size() > 0) {
		m_particleInstances.dataBuffer.reserve(m_particleList.size());

		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		ctx->Map(m_animationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		AnimationBuffer* animBuffer = (AnimationBuffer*)mappedSubresource.pData;
		animBuffer->deltaTime = BEngine::timeManager.m_deltaTime;
		animBuffer->particleCount = (float)m_particleList.size();

		animBuffer->camPos = XMFLOAT4(
			BEngine::playerCamera.position.x, 
			BEngine::playerCamera.position.y, 
			BEngine::playerCamera.position.z, 
			1.0F);

		ctx->Unmap(m_animationBuffer, 0);

		ctx->CSSetShader(m_computeShader, nullptr, NULL);

		UINT initialCount = m_particleList.size();
		ctx->CSSetUnorderedAccessViews(0, 1, &m_rwBufferUAV, 0);
		ctx->CSSetUnorderedAccessViews(1, 1, &m_particleInstances.d3d11ShaderUAV, &initialCount);
		ctx->CSSetConstantBuffers(0, 1, &m_animationBuffer);

		ctx->Dispatch(m_particleList.size() / 1024, 1, 1);

		ID3D11UnorderedAccessView* nullUAV[] = { nullptr, nullptr };
		UINT initialUAV[] = { 0, 0 };
		ctx->CSSetUnorderedAccessViews(0, 2, nullUAV, initialUAV);

		ctx->CSSetShader(nullptr, nullptr, NULL);

		ctx->CopyResource(m_particleInstances2.d3d11Buffer, m_particleInstances.d3d11Buffer);

		bool wasChanged = false;
		for (int I = m_particleList.size() - 1; I >= 0; --I) {
			Particle* currParticle = &m_particleList[I];

			if (currParticle->isDeleted) {
				m_particleList.erase(m_particleList.begin() + I);
				m_particleInstances.dataBuffer.erase(m_particleInstances.dataBuffer.begin() + I);
				wasChanged = true;
			}
		}

		if (wasChanged) {
			m_particleList.shrink_to_fit();
			m_particleInstances.dataBuffer.shrink_to_fit();

			m_particleInstances.CreateBuffer();

			RebuildBuffer();
		}

		if (m_particleInstances.dataBuffer.size() <= 0)
			return;
		
		ctx->IASetInputLayout(m_inputLayout);
		ctx->VSSetShader(m_vertexShader, nullptr, 0);
		ctx->PSSetShader(m_pixelShader, nullptr, 0);

		ctx->IASetIndexBuffer(m_indiceBuffer, DXGI_FORMAT_R32_UINT, 0);

		UINT strides = sizeof(Vertex);
		UINT offset = 0;
		ctx->IASetVertexBuffers(0, 1, &m_vertexBuffer, &strides, &offset);

		ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		ctx->Map(m_particleMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		{
			ParticleMatrices* data = (ParticleMatrices*)mappedSubresource.pData;
			data->projMatrix = projMatrix;
			data->viewMatrix = viewMatrix;
		}
		ctx->Unmap(m_particleMatrixBuffer, 0);

		ctx->VSSetShaderResources(1, 1, &m_particleInstances2.d3d11ShaderResources);
		ctx->VSSetConstantBuffers(3, 1, &m_particleMatrixBuffer);

		ctx->PSSetShaderResources(0, 1, &m_atlas.m_textureSRV);

		ctx->SetPredication(m_occlusionPredicate, TRUE);
		ctx->DrawIndexedInstanced(6, (UINT)m_particleInstances.dataBuffer.size(), 0, 0, 0);
		ctx->SetPredication(nullptr, FALSE);
	}
}

void BEngine::ParticleManager::Cleanup() {
	RELEASE_D3D11_OBJECT(m_vertexShader);
	RELEASE_D3D11_OBJECT(m_inputLayout);
	RELEASE_D3D11_OBJECT(m_pixelShader);
	RELEASE_D3D11_OBJECT(m_vertexBuffer);
	RELEASE_D3D11_OBJECT(m_indiceBuffer);
	RELEASE_D3D11_OBJECT(m_particleMatrixBuffer);
}
