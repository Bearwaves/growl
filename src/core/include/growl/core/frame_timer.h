#pragma once

// Frame timer is used to track and govern frame times across the engine.

#include <chrono>
namespace Growl {

class FrameTimer {
public:
	FrameTimer()
		: last_render{std::chrono::high_resolution_clock::now()}
		, delta_time{0}
		, total_time{0} {}

	double frame() {
		auto now = std::chrono::high_resolution_clock::now();
		delta_time =
			std::chrono::duration<double, std::chrono::seconds::period>(
				now - last_render)
				.count();
		total_time += delta_time;
		last_render = now;
		return delta_time;
	}

	double getDeltaTime() {
		return delta_time;
	}

	double getTotalTime() {
		return total_time;
	}

private:
	std::chrono::high_resolution_clock::time_point last_render;
	double delta_time;
	double total_time;
};

} // namespace Growl
