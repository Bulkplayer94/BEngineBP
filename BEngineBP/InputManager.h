#pragma once
#include "3DMaths.h"

namespace BEngine {
	struct InputHandler {
		float2 mouseDrag = {0.0F, 0.0F};
		float2 mousePos = {0.0F, 0.0F};
		bool mouseIsReleased = true;

		void Proc();

	} extern inputHandler;
}