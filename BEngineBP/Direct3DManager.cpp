#include "pch.h"
#include "Direct3DManager.h"
#include "Win32Manager.h"

bool BEngine::Direct3DManager::Initialize() {

	{
		ID3D11Device* baseDevice = nullptr;
		ID3D11DeviceContext* baseDeviceContext = nullptr;

		D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

		HRESULT hResult = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE,
			0, creationFlags,
			featureLevels, ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION, &baseDevice,
			0, &baseDeviceContext
		);

		if (FAILED(hResult)) {
			MessageBoxA(0, "D3D11CreateDevice() failed", "Fatal Error", MB_OK);
			exit(GetLastError());
		}



		hResult = baseDevice->QueryInterface(__uuidof(ID3D11Device1), (void**)&m_d3d11Device);
		assert(SUCCEEDED(hResult));
		baseDevice->Release();
		baseDevice = nullptr;


		hResult = baseDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&m_d3d11DeviceContext);
		assert(SUCCEEDED(hResult));
		baseDeviceContext->Release();
		baseDeviceContext = nullptr;
	}

#ifdef _DEBUG
	{
		HRESULT hResult = m_d3d11Device->QueryInterface(__uuidof(ID3D11Debug), (void**)&m_d3d11Debug);
		assert(SUCCEEDED(hResult));

		ID3D11InfoQueue* d3d11InfoQueue = nullptr;
		hResult = m_d3d11Debug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3d11InfoQueue);
		assert(SUCCEEDED(hResult));

		d3d11InfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
		d3d11InfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
		d3d11InfoQueue->Release();
	}
#endif // _DEBUG

	{
		HRESULT hResult;
		IDXGIFactory2* dxgiFactory = nullptr;
		{
			IDXGIDevice1* dxgiDevice = nullptr;
			HRESULT hResult = m_d3d11Device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice);
			assert(SUCCEEDED(hResult));

			IDXGIAdapter* dxgiAdapter;
			hResult = dxgiDevice->GetAdapter(&dxgiAdapter);
			assert(SUCCEEDED(hResult));
			dxgiDevice->Release();

			DXGI_ADAPTER_DESC adapterDesc;
			dxgiAdapter->GetDesc(&adapterDesc);

			OutputDebugStringA("Graphics Device: ");
			OutputDebugStringW(adapterDesc.Description);
			OutputDebugStringA("\n");

			hResult = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&dxgiFactory);
			assert(SUCCEEDED(hResult));
			dxgiAdapter->Release();
		}

		DXGI_SWAP_CHAIN_DESC1 d3d11SwapChainDesc = {};
		d3d11SwapChainDesc.Width = 0; // use window width
		d3d11SwapChainDesc.Height = 0; // use window height
		d3d11SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		d3d11SwapChainDesc.SampleDesc.Count = 1;
		d3d11SwapChainDesc.SampleDesc.Quality = 0;
		d3d11SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		d3d11SwapChainDesc.BufferCount = 1;
		d3d11SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		d3d11SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		d3d11SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		d3d11SwapChainDesc.Flags = 0;

		hResult = dxgiFactory->CreateSwapChainForHwnd(m_d3d11Device, BEngine::win32Manager.m_hWnd, &d3d11SwapChainDesc, 0, 0, &m_d3d11SwapChain);
		assert(SUCCEEDED(hResult));

		dxgiFactory->Release();
	}

	CreateD3D11RenderTargets();

	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 1.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0; // Setze MinLOD und MaxLOD auf ihre Standardwerte
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		m_d3d11Device->CreateSamplerState(&samplerDesc, &m_d3d11SamplerState);
	}

	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FrontCounterClockwise = TRUE;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;
		rasterizerDesc.DepthClipEnable = TRUE;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.MultisampleEnable = FALSE;
		rasterizerDesc.AntialiasedLineEnable = FALSE;

		m_d3d11Device->CreateRasterizerState(&rasterizerDesc, &m_d3d11RasterizerState);
	}

	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		m_d3d11Device->CreateDepthStencilState(&depthStencilDesc, &m_d3d11DepthStencilState);
	}

	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = FALSE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		m_d3d11Device->CreateDepthStencilState(&depthStencilDesc, &m_d3d11DepthStencilStateNoZ);
	}

	return true;
}

bool BEngine::Direct3DManager::CreateD3D11RenderTargets() {
	ID3D11Texture2D* d3d11FrameBuffer = nullptr;
	HRESULT hResult = m_d3d11SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&d3d11FrameBuffer);
	assert(SUCCEEDED(hResult));

	hResult = m_d3d11Device->CreateRenderTargetView(d3d11FrameBuffer, nullptr, &m_d3d11FrameBufferView);
	assert(SUCCEEDED(hResult));

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	d3d11FrameBuffer->GetDesc(&depthBufferDesc);

	d3d11FrameBuffer->Release();

	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D* depthBuffer;
	hResult = m_d3d11Device->CreateTexture2D(&depthBufferDesc, nullptr, &depthBuffer);
	if (FAILED(hResult))
		return false;

	hResult = m_d3d11Device->CreateDepthStencilView(depthBuffer, nullptr, &m_d3d11DepthBufferView);
	if (FAILED(hResult))
		return false;

	depthBuffer->Release();

	return true;
}

bool BEngine::Direct3DManager::ResetState() {

	FLOAT backgroundColor[4] = { 0.1f, 0.2f, 0.6f, 1.0f };
	m_d3d11DeviceContext->ClearRenderTargetView(m_d3d11FrameBufferView, backgroundColor);

	m_d3d11DeviceContext->ClearDepthStencilView(m_d3d11DepthBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)BEngine::win32Manager.m_width, (FLOAT)BEngine::win32Manager.m_height, 0.0f, 1.0f };
	m_d3d11DeviceContext->RSSetViewports(1, &viewport);

	m_d3d11DeviceContext->RSSetState(m_d3d11RasterizerState);
	m_d3d11DeviceContext->OMSetDepthStencilState(m_d3d11DepthStencilState, 0);
	m_d3d11DeviceContext->OMSetRenderTargets(1, &m_d3d11FrameBufferView, m_d3d11DepthBufferView);
	m_d3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_d3d11DeviceContext->PSSetSamplers(1, 1, &m_d3d11SamplerState);

	return true;
}

bool BEngine::Direct3DManager::PresentFrame() {
#ifndef _DEBUG
	m_d3d11SwapChain->Present(0, 0);
#else
	m_d3d11SwapChain->Present(1, 0);
#endif
	return true;
}

void BEngine::Direct3DManager::TurnZBufferOff() {
	m_d3d11DeviceContext->OMSetDepthStencilState(m_d3d11DepthStencilStateNoZ, 0);
}

void BEngine::Direct3DManager::TurnZBufferOn() {
	m_d3d11DeviceContext->OMSetDepthStencilState(m_d3d11DepthStencilState, 0);
}

void BEngine::Direct3DManager::ClearObjects() {
	RELEASE_D3D11_OBJECT(m_d3d11DepthStencilStateNoZ);
	RELEASE_D3D11_OBJECT(m_d3d11DepthStencilState);
	RELEASE_D3D11_OBJECT(m_d3d11RasterizerState);
	RELEASE_D3D11_OBJECT(m_d3d11SamplerState);
	RELEASE_D3D11_OBJECT(m_d3d11DepthBufferView);
	RELEASE_D3D11_OBJECT(m_d3d11FrameBufferView);
	RELEASE_D3D11_OBJECT(m_d3d11Debug);
	RELEASE_D3D11_OBJECT(m_d3d11SwapChain);
	RELEASE_D3D11_OBJECT(m_d3d11DeviceContext);
	RELEASE_D3D11_OBJECT(m_d3d11Device);
}
