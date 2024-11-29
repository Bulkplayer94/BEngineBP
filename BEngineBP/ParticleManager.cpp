#include "pch.h"
#include "ParticleManager.h"

#include "Direct3DManager.h"
#include "CCamera.h"
#include "TimeManager.h"
#include "globals.h"

#include "data/compiled_shader/ParticleVS.h"
#include "data/compiled_shader/ParticlePS.h"
#include "data/compiled_shader/ParticleCS.h"

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

	{
		unsigned int particleCount = 100;
		float startPos = 100.F - (particleCount / 2);
		
		float spacing = 7.5f;
		
		for (unsigned int I1 = 0; I1 < particleCount; ++I1) {
			for (unsigned int I2 = 0; I2 < particleCount; ++I2) {
				for (unsigned int I3 = 0; I3 < particleCount; ++I3) {
					float currPosX = startPos + I1 * spacing;
					float currPosY = startPos + I2 * spacing;
					float currPosZ = startPos + I3 * spacing;
		
					Particle computeParticle;
					computeParticle.position = XMFLOAT3(currPosX, currPosY, currPosZ);
					computeParticle.velocity = XMFLOAT3(5.0F, 2.0F, 0.0F);
					computeParticle.gravity = XMFLOAT3(0.0F, -9.807F, 0.0F);
					computeParticle.drag = 1.0F;
					computeParticle.lifeTime = 60.0F;
		
					m_particleList.push_back(computeParticle);
					m_particleInstances.dataBuffer.push_back({ XMFLOAT4X4() });
				}
			}
		}

		//Particle computeParticle = {};
		//computeParticle.position = XMFLOAT3(0.0F, 100.0F, 0.0F);
		//computeParticle.velocity = XMFLOAT3(5.0F, 2.0F, 0.0F);
		//computeParticle.gravity = XMFLOAT3(0.0F, -9.807F, 0.0F);
		//computeParticle.drag = 1.0F;
		//computeParticle.lifeTime = 60.0F;

		//m_particleList.push_back(computeParticle);
		//m_particleInstances.dataBuffer.push_back({ XMFLOAT4X4() });

		m_particleInstances.CreateBuffer();
		m_particleInstances.RefreshBuffer();
	}
}

void BEngine::ParticleManager::Draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projMatrix) {
	ID3D11DeviceContext1* ctx = BEngine::direct3DManager.m_d3d11DeviceContext;

	m_particleInstances.dataBuffer.reserve(m_particleList.size());

	bool wasChanged = false;
	for (int I = m_particleList.size() - 1; I >= 0; --I) {
		Particle* currParticle = &m_particleList[I];

		XMVECTOR smokePos = XMLoadFloat3(&currParticle->position);
		if (!Globals::Status::windowStatus[Globals::Status::WindowStatus_PAUSED] ) {
			XMVECTOR gravity = XMLoadFloat3(&currParticle->gravity);
			XMVECTOR velocity = XMLoadFloat3(&currParticle->velocity);

			smokePos += velocity * BEngine::timeManager.m_deltaTime;
			velocity += gravity * BEngine::timeManager.m_deltaTime;
			velocity *= pow(currParticle->drag, BEngine::timeManager.m_deltaTime);

			XMStoreFloat3(&currParticle->velocity, velocity);
			XMStoreFloat3(&currParticle->position, smokePos);

			currParticle->lifeTime -= BEngine::timeManager.m_deltaTime;

			if (currParticle->lifeTime < 0.0F) { 
				m_particleList.erase(m_particleList.begin() + I); 
				m_particleInstances.dataBuffer.erase(m_particleInstances.dataBuffer.begin() + I); 
				wasChanged = true; 
				continue; 
			}
		}

		XMVECTOR camPos = XMLoadFloat3(&playerCamera.position);

		XMVECTOR lookDir = XMVectorSubtract(smokePos, camPos);
		lookDir = XMVector3Normalize(lookDir);

		float pitch = asinf(XMVectorGetY(lookDir));
		float yaw = atan2f(XMVectorGetX(lookDir), XMVectorGetZ(lookDir));

		XMMATRIX rotationX = XMMatrixRotationX(-pitch);
		XMMATRIX rotationY = XMMatrixRotationY(yaw);

		XMMATRIX rotationMatrix = XMMatrixMultiply(rotationX, rotationY);

		XMMATRIX translationMatrix = XMMatrixTranslationFromVector(smokePos);
		XMMATRIX scalingMatrix = XMMatrixScaling(3.0F, 3.0F, 3.0F);

		XMMATRIX worldMatrix = XMMatrixMultiply(scalingMatrix, rotationMatrix);
		worldMatrix = XMMatrixMultiply(worldMatrix, translationMatrix);

		worldMatrix = XMMatrixTranspose(worldMatrix);

		XMFLOAT4X4 worldMatrix4X4;
		XMStoreFloat4x4(&worldMatrix4X4, worldMatrix);

		m_particleInstances.dataBuffer[I] = { worldMatrix4X4 };
	}

	if (wasChanged) {
		m_particleList.shrink_to_fit();
		m_particleInstances.dataBuffer.shrink_to_fit();

		m_particleInstances.CreateBuffer();
	}

	if (m_particleInstances.dataBuffer.size() <= 0)
		return;

	m_particleInstances.RefreshBuffer();

	ctx->IASetInputLayout(m_inputLayout);
	ctx->VSSetShader(m_vertexShader, nullptr, 0);
	ctx->PSSetShader(m_pixelShader, nullptr, 0);

	ctx->IASetIndexBuffer(m_indiceBuffer, DXGI_FORMAT_R32_UINT, 0);

	UINT strides = sizeof(Vertex);
	UINT offset = 0;
	ctx->IASetVertexBuffers(0, 1, &m_vertexBuffer, &strides, &offset);

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ctx->Map(m_particleMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	{
		ParticleMatrices* data = (ParticleMatrices*)mappedSubresource.pData;
		data->projMatrix = projMatrix;
		data->viewMatrix = viewMatrix;
	}
	ctx->Unmap(m_particleMatrixBuffer, 0);

	ctx->VSSetShaderResources(1, 1, &m_particleInstances.d3d11ShaderResources);
	ctx->VSSetConstantBuffers(3, 1, &m_particleMatrixBuffer);

	ctx->DrawIndexedInstanced(6, (UINT)m_particleInstances.dataBuffer.size(), 0, 0, 0);
}

void BEngine::ParticleManager::Cleanup() {
	RELEASE_D3D11_OBJECT(m_vertexShader);
	RELEASE_D3D11_OBJECT(m_inputLayout);
	RELEASE_D3D11_OBJECT(m_pixelShader);
	RELEASE_D3D11_OBJECT(m_vertexBuffer);
	RELEASE_D3D11_OBJECT(m_indiceBuffer);
	RELEASE_D3D11_OBJECT(m_particleMatrixBuffer);
}
