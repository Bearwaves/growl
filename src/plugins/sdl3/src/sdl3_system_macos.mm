#include "sdl3_system.h"
#include <Foundation/Foundation.h>
#include <iostream>
#include <sys/utsname.h>

using Growl::SDL3SystemAPI;

std::string SDL3SystemAPI::getPlatformOSVersion() {
	struct utsname info;
	uname(&info);

	std::string version;
	int len = strlen(info.release);
	std::cout << info.release << std::endl;

	for (size_t i = 0; i < len; ++i) {
		if (!isdigit(info.release[i]) && info.release[i] != '.') {
			return std::string(info.release, info.release + i);
		}
	}

	return std::string(info.release);
	/*NSProcessInfo* info = [NSProcessInfo processInfo];
	auto os_info = [info operatingSystemVersion];
	return std::to_string(os_info.majorVersion) + "." +
		   std::to_string(os_info.minorVersion) + "." +
		   std::to_string(os_info.patchVersion);*/
}
