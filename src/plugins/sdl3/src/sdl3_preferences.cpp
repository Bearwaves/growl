#include "sdl3_preferences.h"
#include "SDL3/SDL_asyncio.h"
#include "growl/core/error.h"
#include "growl/core/log.h"
#include "growl/core/system/preferences.h"
#include "sdl3_system.h"
#include <SDL3/SDL_timer.h>

using Growl::Error;
using Growl::SDL3Preferences;
using Growl::SDL3SystemAPI;

SDL3Preferences::SDL3Preferences(
	SDL3SystemAPI& api, std::filesystem::path prefs_file, bool shared,
	nlohmann::json&& j)
	: Preferences{shared, std::move(j)}
	, api{api}
	, prefs_file{prefs_file} {
	prefs_file_tmp = prefs_file;
	prefs_file_tmp.replace_extension(".json.tmp");
	queue = SDL_CreateAsyncIOQueue();
}

SDL3Preferences::~SDL3Preferences() {
	store();
	int count = 0;
	while (!strings.empty() && count++ < 500) {
		tick();
		SDL_Delay(1);
	}
	SDL_DestroyAsyncIOQueue(queue);
}

void SDL3Preferences::store() {
	dirty = true;
	if (!strings.empty()) {
		return;
	}
	auto file =
		SDL_AsyncIOFromFile(prefs_file_tmp.generic_u8string().c_str(), "w");
	if (!file) {
		api.log(
			LogLevel::Error, "SDL3Preferences", "Failed to open file at {}: {}",
			prefs_file.generic_u8string(), SDL_GetError());
		return;
	}

	writes++;
	strings[writes] = data().dump();

	if (!(SDL_WriteAsyncIO(
			file, strings[writes].data(), 0, strings[writes].size(), queue,
			reinterpret_cast<void*>(writes)))) {
		api.log(
			LogLevel::Error, "SDL3Preferences",
			"Failed to write preferences: {}", SDL_GetError());
		strings.erase(writes);
		return;
	}
	dirty = false;

	if (!SDL_CloseAsyncIO(file, true, queue, reinterpret_cast<void*>(writes))) {
		api.log(
			LogLevel::Error, "SDL3Preferences",
			"Failed to close preferences: {}", SDL_GetError());
	}
}

void SDL3Preferences::tick() {
	SDL_AsyncIOOutcome outcome;
	while (SDL_GetAsyncIOResult(queue, &outcome)) {
		std::uint64_t write_id = reinterpret_cast<uint64_t>(outcome.userdata);
		if (outcome.result == SDL_ASYNCIO_FAILURE && strings.count(write_id)) {
			// Remove from map, don't process this one any further
			strings.erase(write_id);
			continue;
		}
		if (outcome.result == SDL_ASYNCIO_COMPLETE &&
			outcome.type == SDL_ASYNCIO_TASK_CLOSE && strings.count(write_id)) {
			std::error_code ec;
			std::filesystem::rename(prefs_file_tmp, prefs_file, ec);
			if (ec) {
				api.log(
					LogLevel::Warn, "SDL3Preferences",
					"Error renaming file: {}", ec.message());
			}
		}
		if (outcome.type == SDL_ASYNCIO_TASK_CLOSE && strings.count(write_id)) {
			strings.clear();
		}
	}

	if (dirty) {
		store();
	}
}
