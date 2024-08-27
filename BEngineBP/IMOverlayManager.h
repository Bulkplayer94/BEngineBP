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