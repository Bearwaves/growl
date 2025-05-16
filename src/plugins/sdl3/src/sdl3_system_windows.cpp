#include "sdl3_system.h"

#include <windows.h>

#include <VersionHelpers.h>

using Growl::SDL3SystemAPI;

bool IsWin11OrGreater() {
	constexpr DWORD MAJOR_VERSION = 10;
	constexpr DWORD MINOR_VERSION = 0;
	constexpr DWORD BUILD_NUM = 21996;

	return IsWindowsVersionOrGreater(MAJOR_VERSION, MINOR_VERSION, BUILD_NUM);
}

std::string SDL3SystemAPI::getPlatformOSVersion() {
#if (_MSC_VER >= 1900)
	if (IsWin11OrGreater()) {
		return "11";
	} else if (IsWindows10OrGreater()) {
		return "10";
	}
#endif

	if (IsWindows8Point1OrGreater()) {
		return "6.3";
	} else if (IsWindows8OrGreater()) {
		return "6.2";
	} else if (IsWindows7OrGreater()) {
		return "6.1";
	} else if (IsWindowsVistaOrGreater()) {
		return "6.0";
	} else if (IsWindowsXPOrGreater()) {
		return "5.1";
	}

	return "0";
}
