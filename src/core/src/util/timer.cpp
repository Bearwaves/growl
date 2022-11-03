#include "growl/core/util/timer.h"
#include "growl/core/api/system_api.h"
#include "growl/core/log.h"
#include <chrono>
#include <string>

using Growl::SystemAPI;
using Growl::Timer;
using std::chrono::duration;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

Timer::Timer(SystemAPI& system_api, std::string tag, std::string prefix)
	: system_api{system_api}
	, tag{tag}
	, prefix{prefix} {
	start = std::chrono::high_resolution_clock::now();
}

Timer::~Timer() {
	auto time_seconds = duration<double, milliseconds::period>(
							high_resolution_clock::now() - start)
							.count();
	system_api.log(LogLevel::Info, tag, "{} took {}ms", prefix, time_seconds);
}
