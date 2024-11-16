#include "pch.h"
#include "TimeManager.h"

bool BEngine::TimeManager::Initialize() {
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

void BEngine::TimeManager::Frame() {
	double previousTimeInSeconds = m_currTime;
	LARGE_INTEGER perfCount;
	QueryPerformanceCounter(&perfCount);

	m_currTime = (double)(perfCount.QuadPart - m_startPerfCounter) / (double)m_perfCounterFreq;
	m_deltaTime = (float)(m_currTime - previousTimeInSeconds);
}
