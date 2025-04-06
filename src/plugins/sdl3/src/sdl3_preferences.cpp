#include "sdl3_preferences.h"
#include "SDL3/SDL_asyncio.h"
#include "growl/core/error.h"
#include "growl/core/log.h"
#include "growl/core/system/preferences.h"
#include "sdl3_system.h"

using Growl::Error;
using Growl::SDL3Preferences;
using Growl::SDL3SystemAPI;

SDL3Preferences::SDL3Preferences(
	SDL3SystemAPI& api, std::filesystem::path prefs_file, bool shared,
	nlohmann::json&& j)
	: Preferences{shared, std::move(j)}
	, api{api}
	, prefs_file{prefs_file} {
	queue = SDL_CreateAsyncIOQueue();
	file = SDL_AsyncIOFromFile(prefs_file.generic_u8string().c_str(), "w");
}

SDL3Preferences::~SDL3Preferences() {
	store();
	if (!SDL_CloseAsyncIO(file, true, queue, nullptr)) {
		api.log(
			LogLevel::Error, "SDL3Preferences",
			"Failed to close preferences: {}", SDL_GetError());
		return;
	}
	SDL_DestroyAsyncIOQueue(queue);
}

void SDL3Preferences::store() {
	// Let's clear any previous results
	processResults();
	writes++;
	strings[writes] = data().dump();
	if (!(SDL_WriteAsyncIO(
			file, strings[writes].data(), 0, strings[writes].size(), queue,
			reinterpret_cast<void*>(writes)))) {
		api.log(
			LogLevel::Error, "SDL3Preferences",
			"Failed to write preferences: {}", SDL_GetError());
	}
}

void SDL3Preferences::processResults() {
	SDL_AsyncIOOutcome outcome;
	while (SDL_GetAsyncIOResult(queue, &outcome)) {
		strings.erase(reinterpret_cast<uint64_t>(outcome.userdata));
	}
}
