#pragma once

#include "growl/core/api/system_api.h"
#include <chrono>
namespace Growl {

class Timer {
public:
	Timer(SystemAPI* system_api, std::string tag, std::string prefix);
	~Timer();

private:
	SystemAPI* system_api;
	std::string tag;
	std::string prefix;
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

} // namespace Growl
