#pragma once

#include <d3d11_1.h>
#include "Direct3DManager.h"

namespace BEngine {
	template<typename T>
	struct ResizeableBuffer {
		ResizeableBuffer() {
			dataBuffer.push_back(T());
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
		ID3D11Buffer* d3d11Buffer = nullptr;
        size_t lastBufferSize = 0;
		ID3D11ShaderResourceView* d3d11ShaderResources = nullptr;
        ID3D11UnorderedAccessView* d3d11ShaderUAV = nullptr;

		void CreateBuffer();
        bool RefreshBuffer();
        bool UpdateVector();
	};
}

template<typename T>
void BEngine::ResizeableBuffer<T>::CreateBuffer() {
    lastBufferSize = dataBuffer.size();

    if (d3d11Buffer) {
        d3d11Buffer->Release();
        d3d11Buffer = nullptr;
    }
    if (d3d11ShaderResources) {
        d3d11ShaderResources->Release();
        d3d11ShaderResources = nullptr;
    }
    if (d3d11ShaderUAV) {
        d3d11ShaderUAV->Release();
        d3d11ShaderUAV = nullptr;
    }

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(T) * (dataBuffer.size() + 1);
    bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    bufferDesc.StructureByteStride = sizeof(T);
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    HRESULT hRes = BEngine::Direct3DManager::GetInstance().m_d3d11Device->CreateBuffer(&bufferDesc, nullptr, &d3d11Buffer);
    if (FAILED(hRes))
        throw std::runtime_error("Creation of Structured Buffer Failed!");

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.ElementWidth = static_cast<UINT>(dataBuffer.size() + 1);

    hRes = BEngine::Direct3DManager::GetInstance().m_d3d11Device->CreateShaderResourceView(d3d11Buffer, &srvDesc, &d3d11ShaderResources);
    if (FAILED(hRes))
        throw std::runtime_error("Creation of Structured Shader Resource View Failed!");

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Buffer.NumElements = static_cast<UINT>(dataBuffer.size() + 1);
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;

    hRes = BEngine::Direct3DManager::GetInstance().m_d3d11Device->CreateUnorderedAccessView(d3d11Buffer, &uavDesc, &d3d11ShaderUAV);
    if (FAILED(hRes))
        throw std::runtime_error("Creation of Unordered Access View Failed!");
}

template<typename T>
bool BEngine::ResizeableBuffer<T>::RefreshBuffer() {
    if (dataBuffer.size() > lastBufferSize)
        CreateBuffer();

    BEngine::Direct3DManager::GetInstance().m_d3d11DeviceContext->UpdateSubresource(d3d11Buffer, 0, nullptr, dataBuffer.data(), 0, 0);
    return true;
}

template<typename T>
bool BEngine::ResizeableBuffer<T>::UpdateVector() {
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    HRESULT hR = BEngine::Direct3DManager::GetInstance().m_d3d11DeviceContext->Map(d3d11Buffer, 0, D3D11_MAP_READ, 0, &mappedSubresource);
    assert(SUCCEEDED(hR));

    T* resourceData = reinterpret_cast<T*>(mappedSubresource.pData);
    dataBuffer.assign(resourceData, resourceData + dataBuffer.size());

    BEngine::Direct3DManager::GetInstance().m_d3d11DeviceContext->Unmap(d3d11Buffer, 0);

    return true;
}