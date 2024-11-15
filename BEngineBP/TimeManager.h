#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace BEngine {
	inline struct TimeManager {

		LONGLONG m_startPerfCounter = 0.0L;
		LONGLONG m_perfCounterFreq = 0.0L;
		long double m_currTime = 0.0;
		float m_deltaTime = 0.0F;

		bool Initialize();

		void Tick();

	} timeManager;
}