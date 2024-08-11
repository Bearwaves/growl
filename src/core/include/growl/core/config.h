#pragma once

#include <string>
namespace Growl {

struct Config {
	std::string name = "Growl";

	int window_width = 1000;
	int window_height = 1000;
	bool window_centered = true;
};

} // namespace Growl
