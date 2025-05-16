#include "sdl3_system.h"
#include <emscripten/version.h>

using Growl::SDL3SystemAPI;

std::string SDL3SystemAPI::getPlatformOSVersion() {
	return std::to_string(__EMSCRIPTEN_major__) + "." +
		   std::to_string(__EMSCRIPTEN_minor__) + "." +
		   std::to_string(__EMSCRIPTEN_tiny__);
}
