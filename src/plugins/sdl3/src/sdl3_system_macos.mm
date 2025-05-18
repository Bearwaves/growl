#include "sdl3_system.h"
#include <Foundation/Foundation.h>
#include <sys/sysctl.h>

using Growl::SDL3SystemAPI;

std::string SDL3SystemAPI::getPlatformOSVersion() {
	NSProcessInfo* info = [NSProcessInfo processInfo];
	auto os_info = [info operatingSystemVersion];
	return std::to_string(os_info.majorVersion) + "." +
		   std::to_string(os_info.minorVersion) + "." +
		   std::to_string(os_info.patchVersion);
}

std::string SDL3SystemAPI::getDeviceManufacturer() {
	return "Apple";
}

std::string SDL3SystemAPI::getDeviceModel() {
	size_t len = 0;
	sysctlbyname("hw.model", NULL, &len, NULL, 0);
	if (!len) {
		return "";
	}

	std::unique_ptr<char[]> buffer = std::make_unique<char[]>(len);
	sysctlbyname("hw.model", buffer.get(), &len, NULL, 0);

	return std::string(buffer.get(), len);
}
