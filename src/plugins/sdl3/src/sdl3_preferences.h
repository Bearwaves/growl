#pragma once

#include "growl/core/error.h"
#include "growl/core/system/preferences.h"
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
	SDL3SystemAPI& api;
	std::filesystem::path prefs_file;
	std::thread writer;
	std::mutex mutex;

	Error doStore();
};

} // namespace Growl
