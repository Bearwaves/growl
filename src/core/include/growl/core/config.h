#pragma once

#include "growl/core/input/keyboard.h"
#include <string>
namespace Growl {

struct Config {
	std::string name = "Growl";

	int window_width = 1000;
	int window_height = 1000;
	bool window_centered = true;

	Key debug_mode_key = Key::FunctionF12;
};

} // namespace Growl
