#pragma once

#include <PxPhysicsAPI.h>
using namespace physx;

#include <Windows.h>
#include <d3d11_1.h>
#include <assert.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "3DMaths.h"

#include <DirectXMath.h>
using namespace DirectX;


namespace Globals {

	bool initGlobals(HINSTANCE hInstance);

	namespace Win32 {
		extern HINSTANCE hInstance;
		extern HWND hWnd;
		extern XMFLOAT2 aspectRatio;

		bool initWin32(HINSTANCE hInstance);
	}

	namespace Direct3D {
		extern ID3D11Device1* d3d11Device;
		extern ID3D11DeviceContext1* d3d11DeviceContext;
		extern ID3D11Debug* d3dDebug;
		extern IDXGISwapChain1* d3d11SwapChain;
		
		extern ID3D11RenderTargetView* d3d11FrameBufferView;
		extern ID3D11DepthStencilView* depthBufferView;

		extern ID3D11SamplerState* samplerState;
		extern ID3D11RasterizerState* rasterizerState;
		extern ID3D11DepthStencilState* depthStencilState;

		bool initDirect3D();
		bool win32CreateD3D11RenderTargets(ID3D11Device1* d3d11Device, IDXGISwapChain1* swapChain, ID3D11RenderTargetView** d3d11FrameBufferView, ID3D11DepthStencilView** depthBufferView);
	}

	namespace CUserCmd {
		enum CUserCmd_ {
			CUserCmd_None,
			CUserCmd_MoveCamFwd,
			CUserCmd_MoveCamBack,
			CUserCmd_MoveCamLeft,
			CUserCmd_MoveCamRight,
			CUserCmd_TurnCamLeft,
			CUserCmd_TurnCamRight,
			CUserCmd_LookUp,
			CUserCmd_LookDown,
			CUserCmd_RaiseCam,
			CUserCmd_LowerCam,
			CUserCmd_Fast,
			CUserCmd_Count,
		};

		extern bool userCmd[CUserCmd_Count];
	}

	namespace Status {
		enum WindowStatus_ {
			WindowStatus_NONE,
			WindowStatus_LOADING,
			WindowStatus_RESIZE,
			WindowStatus_PAUSED,
			WindowStatus_OK,
			WindowStatus_COUNT,
		};

		extern bool windowStatus[WindowStatus_COUNT];

		enum LoadingStatus_ {
			LoadingStatus_NONE,
			LoadingStatus_D3D11,
			LoadingStatus_PhysX,
			LoadingStatus_COUNT,
		};

		extern int loadingStatus;
	}

	namespace Animation {
		extern float deltaTime;
		extern long double currTime;
	}

	namespace PhysX {	
		extern PxFoundation* mFoundation;
		extern PxPvd* mPvd;
		extern PxPhysics* mPhysics;
		extern PxPvdTransport* mTransport;
		extern PxScene* mScene;
		extern PxMaterial* mMaterial;
		extern PxControllerManager* mControllerManager;
		extern PxCapsuleController* mPlayerController;
	}
}