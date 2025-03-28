#include "sdl3_preferences.h"
#include "growl/core/error.h"
#include "growl/core/log.h"
#include "growl/core/system/preferences.h"
#include "sdl3_error.h"
#include "sdl3_system.h"
#include <chrono>
#include <fstream>
#include <thread>

using Growl::Error;
using Growl::SDL3Preferences;
using Growl::SDL3SystemAPI;

SDL3Preferences::SDL3Preferences(
	SDL3SystemAPI& api, std::filesystem::path prefs_file, bool shared,
	nlohmann::json&& j)
	: Preferences{shared, std::move(j)}
	, prefs_file{prefs_file} {
	writer = std::thread([&]() {
		while (!stop) {
			if (dirty) {
				if (auto err = doStore()) {
					api.log(
						LogLevel::Error, "SDL3Preferences",
						"Failed to write preferences file: {}", err->message());
				}
				api.log(
					LogLevel::Debug, "SDL3Preferences",
					"Wrote preferences to file {}", this->prefs_file.string());
				dirty = false;
			} else {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
	});
}

SDL3Preferences::~SDL3Preferences() {
	dirty = true;
	stop = true;
	if (writer.joinable()) {
		writer.join();
	}
}

void SDL3Preferences::store() {
	dirty = true;
}

Error SDL3Preferences::doStore() {
	std::ofstream f(prefs_file);
	f << data() << std::endl;
	f.close();
	if (f.fail()) {
		return std::make_unique<SDL3Error>(
			"Writing preferences to file " + prefs_file.string() + " failed.");
	}
	return nullptr;
}
