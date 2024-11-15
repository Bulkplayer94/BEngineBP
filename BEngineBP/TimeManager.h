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

		bool Initialize() {
			LARGE_INTEGER perfCount;
			if (!QueryPerformanceCounter(&perfCount)) {
				return false;
			}
			m_startPerfCounter = perfCount.QuadPart;

			LARGE_INTEGER perfFreq;
			if (!QueryPerformanceFrequency(&perfFreq)) {
				return false;
			}
			m_perfCounterFreq = perfFreq.QuadPart;

			return true;
		}

		void Tick() {
			double previousTimeInSeconds = m_currTime;
			LARGE_INTEGER perfCount;
			QueryPerformanceCounter(&perfCount);

			m_currTime = (double)(perfCount.QuadPart - m_startPerfCounter) / (double)m_perfCounterFreq;
			m_deltaTime = (float)(m_currTime - previousTimeInSeconds);
		}

	} timeManager;
}