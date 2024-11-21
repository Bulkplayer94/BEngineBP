#pragma once
#include "globals.h"
#include "3DMaths.h"

namespace BEngine {
	inline struct IMOverlayManager {

		void Initialize();
		bool Proc();
		bool EndProc();
		void Cleanup();

	private:

		bool IsConsoleVisible = true;
		bool DrawConsole();

		bool IsDebugVisible = true;
		bool DrawDebug();

	} imOverlayManager;
}
