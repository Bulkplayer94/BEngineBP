#include "pch.h"
#include "ResizableBuffer.h"
#include "Direct3DManager.h"

using namespace BEngine;

template<typename T>
void ResizeableBuffer<T>::CreateBuffer() {
	if (d3d11Buffer != nullptr) {
		d3d11Buffer->Release();
		d3d11Buffer = nullptr;
	}

	if (d3d11ShaderResources != nullptr) {
		d3d11ShaderResources->Release();
		d3d11ShaderResources = nullptr;
	}

	bufferCapacity = 1 + dataBuffer.size();

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(T) * bufferCapacity;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.StructureByteStride = sizeof(T);
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	HRESULT hRes = BEngine::direct3DManager.m_d3d11Device->CreateBuffer(&bufferDesc, nullptr, &d3d11Buffer);
	if (FAILED(hRes))
		throw std::runtime_error("Creation of Structured Buffer Failed!");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementWidth = bufferCapacity;

	hRes = BEngine::direct3DManager.m_d3d11Device->CreateShaderResourceView(d3d11Buffer, &srvDesc, &d3d11ShaderResources);
	if (FAILED(hRes))
		throw std::runtime_error("Creation of Structured Shader Resource View Failed!");
}

template<typename T>
bool ResizeableBuffer<T>::RefreshBuffer() {
	if (dataBuffer.size() > bufferCapacity)
		CreateBuffer();

	BEngine::direct3DManager.m_d3d11DeviceContext->UpdateSubresource(d3d11Buffer, 0, nullptr, dataBuffer.data(), 0, 0);
	return true;
}