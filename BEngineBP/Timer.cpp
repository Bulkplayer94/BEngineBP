#include "pch.h"
#include "Timer.hpp"
#include <iostream>

Timer_t Timer;

void Timer_t::Start(const std::string& Name) {
	this->Timers[Name] = std::chrono::high_resolution_clock::now();
}

void Timer_t::Stop(const std::string& Name) { 
	
	bool IsMicro = false;
	long long Duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - this->Timers[Name]).count();
	if (Duration < 2) {
		Duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - this->Timers[Name]).count();
		IsMicro = true;
	}

	if (IsMicro) {
		std::cout << "'" << Name << "' took 0." << Duration << " milliseconds\n";
	} else {
		std::cout << "'" << Name << "' took " << Duration << " milliseconds\n";
	}

	
	auto Key = this->Timers.find(Name);
	if (Key == this->Timers.end())
		return;

	this->Timers.erase(Key);
}