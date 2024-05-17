#pragma once
#include "globals.h"
#include "3DMaths.h"

struct IMOverlayManager {

	IMOverlayManager();
	bool Proc();
	bool EndProc();
	~IMOverlayManager();

private:

	bool IsConsoleVisible = true;
	bool DrawConsole();

	bool IsDebugVisible = true;
	bool DrawDebug();

};

ImVec2 Convert3Dto2D(float3 point, float4x4* viewMat, float4x4* perspectiveMat, D3D11_VIEWPORT* viewPort);