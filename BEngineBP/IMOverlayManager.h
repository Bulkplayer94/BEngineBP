#pragma once
#include "globals.h"
#include "3DMaths.h"

namespace BEngine {
	struct ImOverlayManager {
		static ImOverlayManager& GetInstance() {
			static ImOverlayManager instance;
			return instance;
		}

		void Initialize();
		bool Proc();
		bool EndProc();
		void Cleanup();

	private:

		bool IsConsoleVisible = true;
		bool DrawConsole();

		bool IsDebugVisible = true;
		bool DrawDebug();

	};
}
