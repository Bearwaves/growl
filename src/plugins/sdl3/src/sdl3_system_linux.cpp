#include "sdl3_system.h"
#include <sys/utsname.h>

using Growl::SDL3SystemAPI;

std::string SDL3SystemAPI::getPlatformOSVersion() {
	struct utsname info;
	uname(&info);

	std::string version;
	int len = strlen(info.release);

	for (size_t i = 0; i < len; ++i) {
		if (!isdigit(info.release[i]) && info.release[i] != '.') {
			return std::string(info.release, info.release + i);
		}
	}

	return std::string(info.release);
}
