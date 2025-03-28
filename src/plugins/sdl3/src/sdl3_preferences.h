#pragma once

#include "growl/core/error.h"
#include "growl/core/system/preferences.h"
#include <atomic>
#include <thread>

namespace Growl {

class SDL3SystemAPI;

class SDL3Preferences final : public Preferences {
public:
	SDL3Preferences(
		SDL3SystemAPI& api, std::filesystem::path prefs_file, bool shared,
		nlohmann::json&& j);
	~SDL3Preferences();
	void store() override;

private:
	std::filesystem::path prefs_file;
	std::thread writer;
	std::atomic<bool> dirty = false;
	std::atomic<bool> stop = false;

	Error doStore();
};

} // namespace Growl
