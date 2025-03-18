#ifndef TIMER_HPP
#define TIMER_HPP

#include <string>
#include <unordered_map>
#include <chrono>

class Timer_t {
public:

	void Start(const std::string& Name);
	void Stop(const std::string& Name);

private:
	std::unordered_map<std::string, std::chrono::steady_clock::time_point> Timers;

} extern Timer;


#endif TIMER_HPP