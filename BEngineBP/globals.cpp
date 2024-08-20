#include "pch.h"
#include "globals.h"

namespace Globals {

    namespace Win32 {
        HINSTANCE hInstance = nullptr;
        HWND hWnd = nullptr;
        XMFLOAT2 aspectRatio = {};
	}

	namespace Direct3D {
		ID3D11Device1* d3d11Device = nullptr;
		ID3D11DeviceContext1* d3d11DeviceContext = nullptr;
		ID3D11Debug* d3dDebug = nullptr;
		IDXGISwapChain1* d3d11SwapChain = nullptr;

		ID3D11RenderTargetView* d3d11FrameBufferView = nullptr;
		ID3D11DepthStencilView* depthBufferView = nullptr;

		ID3D11SamplerState* samplerState = nullptr;
		ID3D11RasterizerState* rasterizerState = nullptr;
		ID3D11DepthStencilState* depthStencilState = nullptr;
	}
    
    namespace CUserCmd {
        bool userCmd[CUserCmd_Count] = {};
    }

    namespace Status {
        bool windowStatus[WindowStatus_COUNT] = {};
        extern int loadingStatus = LoadingStatus_NONE;
    }

    namespace PhysX {
        using namespace physx;

        PxFoundation* mFoundation = nullptr;
        PxPvd* mPvd = nullptr;
        PxPvdTransport* mTransport = nullptr;
        PxPhysics* mPhysics = nullptr;
        PxScene* mScene = nullptr;
        PxMaterial* mMaterial = nullptr;
        PxControllerManager* mControllerManager = nullptr;
        PxCapsuleController* mPlayerController = nullptr;

    }

    namespace Animation {
        float deltaTime = 0.0F;
        long double currTime = 0.0L;
    }
}

bool Globals::initGlobals(HINSTANCE hInstance) {
	
	using namespace Globals::Direct3D;
	using namespace Globals::Win32;

	bool result;
	result = initWin32(hInstance);
	result = initDirect3D();

	return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    ImGui_ImplWin32_WndProcHandler(hWnd, msg, wparam, lparam);

    LRESULT result = 0;
    switch (msg)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    case WM_SIZE:
    {
        Globals::Status::windowStatus[Globals::Status::WindowStatus_RESIZE] = true;
        break;
    }
    default:
        result = DefWindowProcW(hWnd, msg, wparam, lparam);
    }
    return result;
}
// Initialize the Windows Window and set Icons & Title
bool Globals::Win32::initWin32(HINSTANCE hInstance) {
	using namespace Globals::Win32;

	WNDCLASSEXW winClass = {};
	winClass.cbSize = sizeof(WNDCLASSEXW);
	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = &WndProc;
	winClass.hInstance = hInstance;
	winClass.hIcon = LoadIconW(0, IDI_APPLICATION);
	winClass.hCursor = NULL;
	winClass.lpszClassName = L"MyWindowClass";
	winClass.hIconSm = LoadIconW(0, IDI_APPLICATION);

	if (!RegisterClassExW(&winClass)) {
		MessageBoxA(0, "RegisterClassEx failed", "Fatal Error", MB_OK);
		return GetLastError();
	}

	RECT initialRect = { 0, 0, 1024, 768 };
	AdjustWindowRectEx(&initialRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);
	LONG initialWidth = initialRect.right - initialRect.left;
	LONG initialHeight = initialRect.bottom - initialRect.top;

	hWnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW,
		winClass.lpszClassName,
		L"BEngine",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		initialWidth,
		initialHeight,
		0, 0, hInstance, 0);


	if (!hWnd) {
		MessageBoxA(0, "CreateWindowEx failed", "Fatal Error", MB_OK);
		return GetLastError();
	}

    INT screenWidth = GetSystemMetrics(SM_CXSCREEN);
    INT screenHeight = GetSystemMetrics(SM_CYSCREEN);

    INT posX = (screenWidth - initialWidth) / 2;
    INT posY = (screenHeight - initialHeight) / 2;

    SetWindowPos(hWnd, NULL, posX, posY, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

    while (ShowCursor(FALSE) >= 0);
	
	return true;
}

bool Globals::Direct3D::initDirect3D() {
	using namespace Globals::Direct3D;
    using namespace Globals::Win32;

    // Create D3D11 Device and Context
    // ID3D11Device1* d3d11Device;
    // ID3D11DeviceContext1* d3d11DeviceContext;
    {
        ID3D11Device* baseDevice;
        ID3D11DeviceContext* baseDeviceContext;
        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        HRESULT hResult = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE,
            0, creationFlags,
            featureLevels, ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION, &baseDevice,
            0, &baseDeviceContext);
        if (FAILED(hResult)) {
            MessageBoxA(0, "D3D11CreateDevice() failed", "Fatal Error", MB_OK);
            return GetLastError();
        }


        // Get 1.1 interface of D3D11 Device and Context
        hResult = baseDevice->QueryInterface(__uuidof(ID3D11Device1), (void**)&d3d11Device);
        assert(SUCCEEDED(hResult));
        baseDevice->Release();

        hResult = baseDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), (void**)&d3d11DeviceContext);
        assert(SUCCEEDED(hResult));
        baseDeviceContext->Release();
    }

#ifdef _DEBUG
    // Set up debug layer to break on D3D11 errors
    // ID3D11Debug* d3dDebug = nullptr;
    d3d11Device->QueryInterface(__uuidof(ID3D11Debug), (void**)&d3dDebug);
    if (d3dDebug)
    {
        ID3D11InfoQueue* d3dInfoQueue = nullptr;
        if (SUCCEEDED(d3dDebug->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&d3dInfoQueue)))
        {
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
            d3dInfoQueue->Release();
        }
        d3dDebug->Release();
    }
#endif

    // Create Swap Chain
    // IDXGISwapChain1* d3d11SwapChain;
    {
        // Get DXGI Factory (needed to create Swap Chain)
        IDXGIFactory2* dxgiFactory;
        {
            IDXGIDevice1* dxgiDevice;
            HRESULT hResult = d3d11Device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice);
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
        d3d11SwapChainDesc.BufferCount = 2;
        d3d11SwapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        d3d11SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        d3d11SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        d3d11SwapChainDesc.Flags = 0;

        HRESULT hResult = dxgiFactory->CreateSwapChainForHwnd(d3d11Device, hWnd, &d3d11SwapChainDesc, 0, 0, &d3d11SwapChain);
        assert(SUCCEEDED(hResult));

        dxgiFactory->Release();
    }

    // Create Render Target and Depth Buffer
    // ID3D11RenderTargetView* d3d11FrameBufferView;
    // ID3D11DepthStencilView* depthBufferView;
    win32CreateD3D11RenderTargets(d3d11Device, d3d11SwapChain, &d3d11FrameBufferView, &depthBufferView);

    // Create Sampler State
    // ID3D11SamplerState* samplerState;
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

        d3d11Device->CreateSamplerState(&samplerDesc, &samplerState);
    }

    // ID3D11RasterizerState* rasterizerState;
    {
        D3D11_RASTERIZER_DESC rasterizerDesc = {};
        rasterizerDesc.FillMode = D3D11_FILL_SOLID; // Fülle die Dreiecke
        rasterizerDesc.CullMode = D3D11_CULL_BACK; // Aktiviere Backface Culling
        rasterizerDesc.FrontCounterClockwise = TRUE; // Die Dreiecksnormalen zeigen im Uhrzeigersinn (standardmäßig)
        rasterizerDesc.DepthBias = 0;
        rasterizerDesc.DepthBiasClamp = 0.0f;
        rasterizerDesc.SlopeScaledDepthBias = 0.0f;
        rasterizerDesc.DepthClipEnable = TRUE;
        rasterizerDesc.ScissorEnable = FALSE;
        rasterizerDesc.MultisampleEnable = FALSE;
        rasterizerDesc.AntialiasedLineEnable = FALSE;

        d3d11Device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    }

    // ID3D11DepthStencilState* depthStencilState;
    {
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
        depthStencilDesc.DepthEnable = TRUE;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

        d3d11Device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
    }

	return true;
};

bool Globals::Direct3D::win32CreateD3D11RenderTargets(ID3D11Device1* d3d11Device, IDXGISwapChain1* swapChain, ID3D11RenderTargetView** d3d11FrameBufferView, ID3D11DepthStencilView** depthBufferView)
{
    ID3D11Texture2D* d3d11FrameBuffer;
    HRESULT hResult = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&d3d11FrameBuffer);
    assert(SUCCEEDED(hResult));

    hResult = d3d11Device->CreateRenderTargetView(d3d11FrameBuffer, 0, d3d11FrameBufferView);
    assert(SUCCEEDED(hResult));

    D3D11_TEXTURE2D_DESC depthBufferDesc;
    d3d11FrameBuffer->GetDesc(&depthBufferDesc);

    d3d11FrameBuffer->Release();

    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* depthBuffer;
    d3d11Device->CreateTexture2D(&depthBufferDesc, nullptr, &depthBuffer);

    d3d11Device->CreateDepthStencilView(depthBuffer, nullptr, depthBufferView);

    depthBuffer->Release();

    return true;
}