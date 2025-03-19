#pragma once

#include <d3d11_1.h>
#include <assert.h>
#include "globals.h"

#define RELEASE_D3D11_OBJECT(obj) if (obj != nullptr) { obj->Release(); obj = nullptr; }

namespace BEngine {
	struct Direct3DManager {
		static Direct3DManager& GetInstance() {
			static Direct3DManager d3d11Manager;
			return d3d11Manager;
		}

		ID3D11Device1* m_d3d11Device = nullptr;
		ID3D11DeviceContext1* m_d3d11DeviceContext = nullptr;
		IDXGISwapChain1* m_d3d11SwapChain = nullptr;

		ID3D11Debug* m_d3d11Debug = nullptr;

		ID3D11RenderTargetView* m_d3d11FrameBufferView = nullptr;
		ID3D11DepthStencilView* m_d3d11DepthBufferView = nullptr;

		ID3D11SamplerState* m_d3d11SamplerState = nullptr;
		ID3D11RasterizerState* m_d3d11RasterizerState = nullptr;

		ID3D11DepthStencilState* m_d3d11DepthStencilState = nullptr;
		ID3D11DepthStencilState* m_d3d11DepthStencilStateNoZ = nullptr;

		bool Initialize();
		bool CreateD3D11RenderTargets();
		bool ResetState();
		bool PresentFrame();
		void TurnZBufferOff();
		void TurnZBufferOn();
		void ClearObjects();

	};
}